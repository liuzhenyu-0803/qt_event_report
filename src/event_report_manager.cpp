#include "event_report/event_report_manager.h"
#include <QDebug>
#include <QCoreApplication>

namespace event_report {

EventReportManager::EventReportManager(QObject* parent)
    : QObject(parent)
{
    m_workerThread = new QThread(this);
    m_workerThread->setObjectName("EventReportWorkerThread");
    m_workerThread->start();
}

EventReportManager::~EventReportManager()
{
    shutdown();

    // 手动按依赖关系的反序释放服务实例（因为它们没有父对象）
    // 销毁顺序应为：业务服务 -> 支撑服务 -> 基础服务
    delete m_featureFlagService;  // 依赖 Identity, Config, Http
    delete m_eventReportService;  // 依赖 Identity, Config, Http
    delete m_configService;       // 依赖 Http
    delete m_identityService;     // 无外部服务依赖
    delete m_httpService;         // 被多个服务依赖，最后释放
}

EventReportManager* EventReportManager::instance()
{
    static EventReportManager* s_instance = new EventReportManager(QCoreApplication::instance());
    return s_instance;
}

void EventReportManager::init()
{
    // 无依赖的基础服务
    // 注意：不要指定父对象（this），因为后续需要 moveToThread
    m_httpService = new HttpService();
    m_identityService = new IdentityService();
    
    // 依赖 HttpService 的服务
    m_configService = new ConfigService(m_httpService);
    
    // 依赖多个服务的业务服务
    m_eventReportService = new EventReportService(m_identityService, m_configService, m_httpService);
    m_featureFlagService = new FeatureFlagService(m_identityService, m_configService, m_httpService);

    // 移动到工作线程
    m_httpService->moveToThread(m_workerThread);
    m_identityService->moveToThread(m_workerThread);
    m_configService->moveToThread(m_workerThread);
    m_eventReportService->moveToThread(m_workerThread);
    m_featureFlagService->moveToThread(m_workerThread);

    // 连接信号转发
    connect(m_featureFlagService, &FeatureFlagService::signalFlagsChanged, this, &EventReportManager::signalFlagsChanged);

    // 在工作线程中执行初始化任务
    QMetaObject::invokeMethod(m_httpService, [this]()
    {
        m_httpService->init();
    }, Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_identityService, [this]()
    {
        m_identityService->init();
    }, Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_configService, [this]()
    {
        m_configService->init();
    }, Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_eventReportService, [this]()
    {
        m_eventReportService->init();
    }, Qt::QueuedConnection);

    QMetaObject::invokeMethod(m_featureFlagService, [this]()
    {
        m_featureFlagService->init();
    }, Qt::QueuedConnection);

    qInfo() << "EventReportManager:init: Initialized. Services moved to worker thread:" << m_workerThread;
}

void EventReportManager::shutdown()
{
    if (m_workerThread->isRunning())
    {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

// ========== 事件上报接口 ==========
void EventReportManager::reportEvent(const QString& eventType, const QVariantMap& eventProperties, const QVariantMap& userProperties)
{
    QMetaObject::invokeMethod(m_eventReportService, [this, eventType, eventProperties, userProperties]()
    {
        m_eventReportService->reportEvent(eventType, eventProperties, userProperties);
    }, Qt::QueuedConnection);
}

void EventReportManager::reportEvents(const QList<TrackEvent>& events)
{
    QMetaObject::invokeMethod(m_eventReportService, [this, events]()
    {
        m_eventReportService->reportEvents(events);
    }, Qt::QueuedConnection);
}

void EventReportManager::reportEventsFromJsonFile(const QString& filePath)
{
    QMetaObject::invokeMethod(m_eventReportService, [this, filePath]()
    {
        m_eventReportService->reportEventsFromJsonFile(filePath);
    }, Qt::QueuedConnection);
}

// ========== Feature Flag 接口 ==========
void EventReportManager::fetchFlags(const QStringList& flagKeys)
{
    QMetaObject::invokeMethod(m_featureFlagService, [this, flagKeys]()
    {
        m_featureFlagService->fetchFlags(flagKeys);
    }, Qt::QueuedConnection);
}

QMap<QString, VariantInfo> EventReportManager::getAllFlags() const
{
    return m_featureFlagService->getAllFlags();
}

VariantInfo EventReportManager::getFlag(const QString& flagKey) const
{
    return m_featureFlagService->getFlag(flagKey);
}

} // namespace event_report
