#ifndef CONFIG_SERVICE_H
#define CONFIG_SERVICE_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include "http_service.h"

namespace event_report
{

    /**
     * @brief 配置服务类（单例模式），统一管理 API Key 和 Endpoint
     */
    class ConfigService : public QObject
    {
        Q_OBJECT
    public:
        explicit ConfigService(HttpService *httpService, QObject *parent = nullptr);
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
        // 禁止拷贝
        ConfigService(const ConfigService &) = delete;
        ConfigService &operator=(const ConfigService &) = delete;

    private:
        class ConfigServicePrivate *d_ptr;
    };

} // namespace event_report

#endif // CONFIG_SERVICE_H
