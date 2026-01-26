#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include "event_report_export.h"
#include "http_service.h"

namespace event_report {

/**
 * @brief 配置服务类（单例模式），统一管理 API Key 和 Endpoint
 */
class EVENT_REPORT_API ConfigService : public QObject
{
    Q_OBJECT
public:
    explicit ConfigService(HttpService* httpService, QObject* parent = nullptr);
    virtual ~ConfigService();

    /**
     * @brief 初始化 (在工作线程中执行)
     */
    void init();

    /**
     * @brief 获取 Amplitude API Key
     */
    QString getAmplitudeApiKey() const;

    /**
     * @brief 获取事件追踪 Endpoint
     */
    QString getEventTrackEndpoint() const;

    /**
     * @brief 获取 Feature Flag Endpoint
     */
    QString getFeatureFlagEndpoint() const;

private:
    /**
     * @brief 从服务端异步拉取 API Key
     */
    void fetchApiKeyFromServer();

    // 禁止拷贝
    ConfigService(const ConfigService&) = delete;
    ConfigService& operator=(const ConfigService&) = delete;

    void loadApiKeyFromRegistry();
    void saveApiKeyToRegistry(const QString& apiKey);
    void onServerReplyFinished(QNetworkReply* reply);

private:
    HttpService* m_httpService = nullptr;
    QString m_apiKey = "";
};

} // namespace event_report

#endif // CONFIG_SERVICE_H
