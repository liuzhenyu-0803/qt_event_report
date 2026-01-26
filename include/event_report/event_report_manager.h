#ifndef EVENT_REPORT_MANAGER_H
#define EVENT_REPORT_MANAGER_H

#include "event_report_export.h"
#include "http_service.h"
#include "identity_service.h"
#include "config_service.h"
#include "event_report_service.h"
#include "feature_flag_service.h"
#include <QObject>
#include <QThread>
#include <QMutex>

namespace event_report {

/**
 * @brief 核心管理类，负责维护后台线程和所有服务的生命周期
 * 所有公共接口都是线程安全的，会自动将任务调度到工作线程执行
 */
class EVENT_REPORT_API EventReportManager : public QObject
{
    Q_OBJECT
public:
    static EventReportManager* instance();

    /**
     * @brief 初始化所有服务并启动后台线程
     */
    void init();

    /**
     * @brief 停止后台线程
     */
    void shutdown();

    // ========== 事件上报接口 ==========
    /**
     * @brief 上报单个事件
     */
    void reportEvent(const QString& eventType, const QVariantMap& eventProperties = QVariantMap(), const QVariantMap& userProperties = QVariantMap());

    /**
     * @brief 批量上报事件
     */
    void reportEvents(const QList<TrackEvent>& events);

    /**
     * @brief 从文件上报事件
     */
    void reportEventsFromJsonFile(const QString& filePath);

    // ========== Feature Flag 接口 ==========
    /**
     * @brief 拉取功能标志
     */
    void fetchFlags(const QStringList& flagKeys = QStringList());

    /**
     * @brief 获取所有标志（线程安全）
     */
    QMap<QString, VariantInfo> getAllFlags() const;

    /**
     * @brief 获取指定标志（线程安全）
     */
    VariantInfo getFlag(const QString& flagKey) const;

signals:
    /**
     * @brief Feature Flags 状态变化信号（包含更新成功和错误情况）
     * @param success 是否成功，失败时 error 参数包含错误信息
     * @param error 错误信息（仅在失败时有效）
     */
    void signalFlagsChanged(bool success, const QString& error = QString());

private:
    explicit EventReportManager(QObject* parent = nullptr);
    virtual ~EventReportManager();

    EventReportManager(const EventReportManager&) = delete;
    EventReportManager& operator=(const EventReportManager&) = delete;

private:
    QThread* m_workerThread = nullptr;
    
    HttpService* m_httpService = nullptr;
    IdentityService* m_identityService = nullptr;
    ConfigService* m_configService = nullptr;
    EventReportService* m_eventReportService = nullptr;
    FeatureFlagService* m_featureFlagService = nullptr;
};

} // namespace event_report

#endif // EVENT_REPORT_MANAGER_H
