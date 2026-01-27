#include "event_report/feature_flag_service.h"
#include "event_report/identity_service.h"
#include "event_report/config_service.h"
#include "event_report/http_service.h"
#include <QUrl>
#include <QUrlQuery>
#include <QMutexLocker>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

namespace event_report
{

    class FeatureFlagServicePrivate
    {
    public:
        explicit FeatureFlagServicePrivate(FeatureFlagService *q,
                                           IdentityService *identity,
                                           ConfigService *config,
                                           HttpService *http)
            : q_ptr(q), identityService(identity), configService(config), httpService(http) {}

        FeatureFlagService *q_ptr;
        IdentityService *identityService = nullptr;
        ConfigService *configService = nullptr;
        HttpService *httpService = nullptr;
        QMap<QString, VariantInfo> flags;
        mutable QMutex mutex;

        void onReplyFinished(QNetworkReply *reply)
        {
            QByteArray responseData = reply->readAll();
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            reply->deleteLater();

            if (reply->error() != QNetworkReply::NoError)
            {
                qWarning() << "FeatureFlagService:onReplyFinished: Failed [Status:" << statusCode << "]"
                           << "\nError:" << reply->errorString()
                           << "\nResponse:" << responseData;
                emit q_ptr->signalFlagsChanged(false, reply->errorString());
                return;
            }
    
            qInfo() << "FeatureFlagService:onReplyFinished: Success [Status:" << statusCode << "] Response:" << responseData;

            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (!doc.isObject())
            {
                qWarning() << "FeatureFlagService:onReplyFinished: Invalid JSON response";
                return;
            }

            {
                QMutexLocker locker(&mutex);
                flags.clear();
                QJsonObject root = doc.object();
                for (auto it = root.begin(); it != root.end(); ++it)
                {
                    QString flagKey = it.key();
                    QJsonObject variantObj = it.value().toObject();

                    VariantInfo info;
                    info.key = variantObj.value("key").toString();
                    info.payload = variantObj.value("payload").toVariant();

                    flags.insert(flagKey, info);
                }
            }
    
            qInfo() << "FeatureFlagService:onReplyFinished: Successfully loaded" << flags.size() << "flags";
            emit q_ptr->signalFlagsChanged(true);
        }
    };

    FeatureFlagService::FeatureFlagService(IdentityService *identityService,
                                           ConfigService *configService,
                                           HttpService *httpService,
                                           QObject *parent)
        : QObject(parent), d_ptr(new FeatureFlagServicePrivate(this, identityService, configService, httpService))
    {
    }

    FeatureFlagService::~FeatureFlagService()
    {
        delete d_ptr;
    }

    void FeatureFlagService::init()
    {
        // 目前无需特殊初始化逻辑，作为接口预留
    }

    void FeatureFlagService::fetchFlags(const QStringList &flagKeys)
    {
        QString endpoint = d_ptr->configService->getFeatureFlagEndpoint();
        QUrl url(endpoint);
        QUrlQuery query;
        query.addQueryItem("user_id", d_ptr->identityService->getUserID());
        query.addQueryItem("device_id", d_ptr->identityService->getDeviceID());

        if (!flagKeys.isEmpty())
        {
            query.addQueryItem("flag_keys", flagKeys.join(","));
        }
        url.setQuery(query);

        QMap<QByteArray, QByteArray> headers;
        headers.insert("Authorization", QString("Api-Key %1").arg(d_ptr->configService->getAmplitudeApiKey()).toUtf8());

        qInfo() << "FeatureFlagService:fetchFlags: Requesting flags from URL:" << url.toString();

        QNetworkReply *reply = d_ptr->httpService->get(url.toString(), headers);
        connect(reply, &QNetworkReply::finished, this, [this, reply]()
                { d_ptr->onReplyFinished(reply); });
    }

    QMap<QString, VariantInfo> FeatureFlagService::getAllFlags() const
    {
        QMutexLocker locker(&d_ptr->mutex);
        return d_ptr->flags;
    }

    VariantInfo FeatureFlagService::getFlag(const QString &flagKey) const
    {
        QMutexLocker locker(&d_ptr->mutex);
        return d_ptr->flags.value(flagKey);
    }

} // namespace event_report
