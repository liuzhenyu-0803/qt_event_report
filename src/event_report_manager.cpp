#include "event_report/event_report_manager.h"
#include "event_report/http_service.h"
#include "event_report/identity_service.h"
#include "event_report/config_service.h"
#include "event_report/event_report_service.h"
#include "event_report/feature_flag_service.h"
#include <QDebug>
#include <QCoreApplication>
#include <QThread>

namespace event_report
{

    class EventReportManagerPrivate
    {
    public:
        explicit EventReportManagerPrivate(EventReportManager *q) : q_ptr(q) {}

        EventReportManager *q_ptr;
        QThread *workerThread = nullptr;

        HttpService *httpService = nullptr;
        IdentityService *identityService = nullptr;
        ConfigService *configService = nullptr;
        EventReportService *eventReportService = nullptr;
        FeatureFlagService *featureFlagService = nullptr;

        void init()
        {
            workerThread = new QThread(q_ptr);
            workerThread->setObjectName("EventReportWorkerThread");
            workerThread->start();

            httpService = new HttpService();
            identityService = new IdentityService();
            configService = new ConfigService(httpService);
            eventReportService = new EventReportService(identityService, configService, httpService);
            featureFlagService = new FeatureFlagService(identityService, configService, httpService);

            httpService->moveToThread(workerThread);
            identityService->moveToThread(workerThread);
            configService->moveToThread(workerThread);
            eventReportService->moveToThread(workerThread);
            featureFlagService->moveToThread(workerThread);

            QObject::connect(featureFlagService, &FeatureFlagService::signalFlagsChanged, q_ptr, &EventReportManager::signalFlagsChanged);

            QMetaObject::invokeMethod(httpService, [this]()
                                      { httpService->init(); }, Qt::QueuedConnection);
            QMetaObject::invokeMethod(identityService, [this]()
                                      { identityService->init(); }, Qt::QueuedConnection);
            QMetaObject::invokeMethod(configService, [this]()
                                      { configService->init(); }, Qt::QueuedConnection);
            QMetaObject::invokeMethod(eventReportService, [this]()
                                      { eventReportService->init(); }, Qt::QueuedConnection);
            QMetaObject::invokeMethod(featureFlagService, [this]()
                                      { featureFlagService->init(); }, Qt::QueuedConnection);

            qInfo() << "EventReportManager: Private initialized and services moved to worker thread.";
        }

        void shutdown()
        {
            if (workerThread && workerThread->isRunning())
            {
                workerThread->quit();
                workerThread->wait();
            }
        }

        ~EventReportManagerPrivate()
        {
            shutdown();
            delete featureFlagService;
            delete eventReportService;
            delete configService;
            delete identityService;
            delete httpService;
        }
    };

    EventReportManager::EventReportManager(QObject *parent)
        : QObject(parent), d_ptr(new EventReportManagerPrivate(this))
    {
    }

    EventReportManager::~EventReportManager()
    {
        delete d_ptr;
    }

    EventReportManager *EventReportManager::instance()
    {
        static EventReportManager *s_instance = new EventReportManager(QCoreApplication::instance());
        return s_instance;
    }

    void EventReportManager::init()
    {
        d_ptr->init();
    }

    void EventReportManager::shutdown()
    {
        d_ptr->shutdown();
    }

    void EventReportManager::reportEvent(const QString &eventType, const QVariantMap &eventProperties, const QVariantMap &userProperties)
    {
        QMetaObject::invokeMethod(d_ptr->eventReportService, [this, eventType, eventProperties, userProperties]()
                                  { d_ptr->eventReportService->reportEvent(eventType, eventProperties, userProperties); }, Qt::QueuedConnection);
    }

    void EventReportManager::reportEvents(const QList<TrackEvent> &events)
    {
        QMetaObject::invokeMethod(d_ptr->eventReportService, [this, events]()
                                  { d_ptr->eventReportService->reportEvents(events); }, Qt::QueuedConnection);
    }

    void EventReportManager::reportEventsFromJsonFile(const QString &filePath)
    {
        QMetaObject::invokeMethod(d_ptr->eventReportService, [this, filePath]()
                                  { d_ptr->eventReportService->reportEventsFromJsonFile(filePath); }, Qt::QueuedConnection);
    }

    void EventReportManager::fetchFlags(const QStringList &flagKeys)
    {
        QMetaObject::invokeMethod(d_ptr->featureFlagService, [this, flagKeys]()
                                  { d_ptr->featureFlagService->fetchFlags(flagKeys); }, Qt::QueuedConnection);
    }

    QMap<QString, VariantInfo> EventReportManager::getAllFlags() const
    {
        return d_ptr->featureFlagService->getAllFlags();
    }

    VariantInfo EventReportManager::getFlag(const QString &flagKey) const
    {
        return d_ptr->featureFlagService->getFlag(flagKey);
    }

} // namespace event_report
