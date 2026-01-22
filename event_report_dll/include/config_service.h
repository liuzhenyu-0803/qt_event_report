#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QMutex>
#include "event_report_dll_export.h"
#include "http_service.h"

/**
 * @brief 配置服务类（单例模式），统一管理 API Key 和 Endpoint
 */
class EVENT_REPORT_DLL_API ConfigService : public QObject
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
    mutable QMutex m_mutex;
    const QString m_defaultApiKey = "14283a6672ee87df21326b38aa4a5604";
    const QString m_eventTrackEndpoint = "https://api2.amplitude.com/2/httpapi";
    const QString m_featureFlagEndpoint = "https://api.lab.amplitude.com/v1/vardata";
    const QString m_apiKeyServerUrl = "https://your-server.com/api/get-amplitude-key";  // 配置服务端地址
};

#endif // CONFIG_SERVICE_H
