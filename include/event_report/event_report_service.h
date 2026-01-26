#ifndef EVENT_REPORT_SERVICE_H
#define EVENT_REPORT_SERVICE_H

#include "event_report_export.h"
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>

namespace event_report {

// 前置声明
class IdentityService;
class ConfigService;
class HttpService;

/**
 * @brief 埋点数据结构
 */
struct TrackEvent
{
    QString eventType;
    QVariantMap eventProperties;
    QVariantMap userProperties;
};

/**
 * @brief 底层上报服务，直接负责与 Amplitude 通信
 */
class EVENT_REPORT_API EventReportService : public QObject
{
    Q_OBJECT
public:
    explicit EventReportService(IdentityService* identityService,
                               ConfigService* configService,
                               HttpService* httpService,
                               QObject* parent = nullptr);
    virtual ~EventReportService();

    /**
     * @brief 初始化 (在工作线程中执行)
     */
    void init();

    /**
     * @brief 发送原始埋点数据 (重载便捷接口)
     */
    void reportEvent(const QString& eventType, const QVariantMap& eventProperties = QVariantMap(), const QVariantMap& userProperties = QVariantMap());

    /**
     * @brief 批量发送埋点数据
     */
    void reportEvents(const QList<TrackEvent>& events);

    /**
     * @brief 从 JSON 文件中批量发送埋点 (通常用于补报)
     */
    void reportEventsFromJsonFile(const QString& filePath);

protected slots:
    virtual void onReplyFinished(QNetworkReply* reply, const QJsonArray& eventsData);

private:
    void reportEvent(const TrackEvent& event);
    void saveEventsToFile(const QJsonArray& eventsData);
    void sendEvents(const QJsonArray& eventsArray);
    QJsonObject createEventObject(const QString& eventType, const QVariantMap& eventProperties = QVariantMap(), const QVariantMap& userProperties = QVariantMap());

private:
    IdentityService* m_identityService = nullptr;
    ConfigService* m_configService = nullptr;
    HttpService* m_httpService = nullptr;
};

} // namespace event_report

#endif // EVENT_REPORT_SERVICE_H
