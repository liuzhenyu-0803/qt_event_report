#include "event_report/event_report_service.h"
#include "event_report/identity_service.h"
#include "event_report/config_service.h"
#include "event_report/http_service.h"
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QUuid>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QLocale>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>

namespace event_report
{

    class EventReportServicePrivate
    {
    public:
        explicit EventReportServicePrivate(EventReportService *q,
                                           IdentityService *identity,
                                           ConfigService *config,
                                           HttpService *http)
            : q_ptr(q), identityService(identity), configService(config), httpService(http) {}

        EventReportService *q_ptr;
        IdentityService *identityService = nullptr;
        ConfigService *configService = nullptr;
        HttpService *httpService = nullptr;

        void reportEvent(const TrackEvent &event)
        {
            QJsonObject eventObj = createEventObject(event.eventType, event.eventProperties, event.userProperties);

            QJsonArray eventsArray;
            eventsArray.append(eventObj);

            sendEvents(eventsArray);

            qInfo() << "EventReportService:reportEvent: reportEvent (struct) ->" << event.eventType << "in thread:" << QThread::currentThread();
        }

        void saveEventsToFile(const QJsonArray &eventsData)
        {
            QString appName = QCoreApplication::applicationName();
            QString dirPath = QString("C:/Users/Public/%1/event_report").arg(appName);

            QDir dir;
            if (!dir.exists(dirPath))
                dir.mkpath(dirPath);

            QString filePath = dirPath + "/failed_events.jsonl";
            QFile file(filePath);

            if (file.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                for (const auto &val : eventsData)
                {
                    QJsonDocument doc(val.toObject());
                    file.write(doc.toJson(QJsonDocument::Compact));
                    file.write("\n");
                }
                file.close();
                qInfo() << "EventReportService:saveEventsToFile: Saved" << eventsData.size() << "failed events to" << filePath;
            }
        }

        void sendEvents(const QJsonArray &eventsArray)
        {
            if (eventsArray.isEmpty())
                return;

            const int maxBatchSize = 1000;
            int totalEvents = eventsArray.size();

            for (int i = 0; i < totalEvents; i += maxBatchSize)
            {
                QJsonArray currentBatch;
                int currentBatchCount = qMin(maxBatchSize, totalEvents - i);
                for (int j = 0; j < currentBatchCount; ++j)
                    currentBatch.append(eventsArray.at(i + j));

                QJsonObject root;
                root["api_key"] = configService->getAmplitudeApiKey();
                root["events"] = currentBatch;

                QString endpoint = configService->getEventTrackEndpoint();
                qInfo() << "EventReportService:sendEvents: Sending batch of" << currentBatchCount << "events to" << endpoint;

                QJsonDocument doc(root);
                QNetworkReply *reply = httpService->post(endpoint, doc.toJson());

                QObject::connect(reply, &QNetworkReply::finished, q_ptr, [this, reply, currentBatch]()
                                 { onReplyFinished(reply, currentBatch); });
            }
        }

        void onReplyFinished(QNetworkReply *reply, const QJsonArray &eventsData)
        {
            QByteArray responseData = reply->readAll();
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            if (reply->error() == QNetworkReply::NoError)
            {
                qInfo() << "EventReportService:onReplyFinished: Success [Status:" << statusCode << "] Response:" << responseData;
            }
            else
            {
                qWarning() << "EventReportService:onReplyFinished: Failed [Status:" << statusCode << "]"
                           << "\nError:" << reply->errorString()
                           << "\nResponse:" << responseData;

                saveEventsToFile(eventsData);
            }
            reply->deleteLater();
        }

        QJsonObject createEventObject(const QString &eventType, const QVariantMap &eventProperties, const QVariantMap &userProperties)
        {
            QJsonObject eventObj;
            eventObj["user_id"] = identityService->getUserID();
            eventObj["device_id"] = identityService->getDeviceID();
            eventObj["event_type"] = eventType;
            eventObj["time"] = QDateTime::currentMSecsSinceEpoch();
            eventObj["event_properties"] = QJsonObject::fromVariantMap(eventProperties);
            eventObj["user_properties"] = QJsonObject::fromVariantMap(userProperties);
            eventObj["app_version"] = QCoreApplication::applicationVersion();
            eventObj["platform"] = "Windows Desktop";
            eventObj["os_name"] = QSysInfo::productType();
            eventObj["os_version"] = QSysInfo::productVersion();
            eventObj["country"] = QLocale::countryToString(QLocale::system().country());
            eventObj["language"] = QLocale::system().name();
            eventObj["ip"] = "$remote";
            return eventObj;
        }
    };

    EventReportService::EventReportService(IdentityService *identityService,
                                           ConfigService *configService,
                                           HttpService *httpService,
                                           QObject *parent)
        : QObject(parent), d_ptr(new EventReportServicePrivate(this, identityService, configService, httpService))
    {
    }

    EventReportService::~EventReportService()
    {
        delete d_ptr;
    }

    void EventReportService::init()
    {
        // 目前无需特殊初始化逻辑，作为接口预留
    }

    void EventReportService::reportEvent(const TrackEvent &event)
    {
        d_ptr->reportEvent(event);
    }

    void EventReportService::reportEvent(const QString &eventType, const QVariantMap &eventProperties, const QVariantMap &userProperties)
    {
        TrackEvent event;
        event.eventType = eventType;
        event.eventProperties = eventProperties;
        event.userProperties = userProperties;
        reportEvent(event);
    }

    void EventReportService::reportEvents(const QList<TrackEvent> &events)
    {
        QJsonArray eventsArray;
        for (const auto &event : events)
        {
            eventsArray.append(d_ptr->createEventObject(event.eventType, event.eventProperties, event.userProperties));
        }

        d_ptr->sendEvents(eventsArray);
        qInfo() << "EventReportService:reportEvents: reportEvents (struct list), count:" << eventsArray.size() << "in thread:" << QThread::currentThread();
    }

    void EventReportService::reportEventsFromJsonFile(const QString &filePath)
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qWarning() << "EventReportService:reportEventsFromJsonFile: Failed to open file for reportEventsFromJsonFile:" << filePath;
            return;
        }

        QJsonArray eventsArray;
        QTextStream in(&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.trimmed().isEmpty())
                continue;

            QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
            if (doc.isObject())
            {
                eventsArray.append(doc.object());
            }
            else if (doc.isArray())
            {
                for (const auto &val : doc.array())
                    eventsArray.append(val.toObject());
            }
        }
        file.close();

        if (!eventsArray.isEmpty())
        {
            d_ptr->sendEvents(eventsArray);
            qInfo() << "EventReportService:reportEventsFromJsonFile: reportEventsFromJsonFile, count:" << eventsArray.size();
        }
    }

} // namespace event_report
