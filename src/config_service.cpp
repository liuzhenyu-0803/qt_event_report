#include "event_report/config_service.h"
#include "event_report/http_service.h"
#include <QSettings>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "event_report/event_report_constants.h"

namespace event_report
{

    class ConfigServicePrivate
    {
    public:
        explicit ConfigServicePrivate(ConfigService *q, HttpService *http)
            : q_ptr(q), httpService(http) {}

        ConfigService *q_ptr;
        HttpService *httpService = nullptr;
        QString apiKey = "";

        void fetchApiKeyFromServer()
        {
            qInfo() << "ConfigService:fetchApiKeyFromServer: Fetching API Key from server:" << STORE_SERVER_URL;

            QNetworkReply *reply = httpService->get(STORE_SERVER_URL);
            QObject::connect(reply, &QNetworkReply::finished, q_ptr, [this, reply]()
                             { onServerReplyFinished(reply); });
        }

        void loadApiKeyFromRegistry()
        {
            QSettings settings(REGISTRY_PATH, QSettings::NativeFormat);

            QString registryApiKey = settings.value(REG_KEY_AMPLITUDE_API_KEY).toString();
            if (!registryApiKey.isEmpty())
            {
                apiKey = registryApiKey;
                qInfo() << "ConfigService:loadApiKeyFromRegistry: API Key loaded from registry";
            }
            else
            {
                // 注册表中没有，使用硬编码默认值并写入注册表
                apiKey = DEFAULT_AMPLITUDE_API_KEY;
                saveApiKeyToRegistry(apiKey);
                qInfo() << "ConfigService:loadApiKeyFromRegistry: Using default API Key and saved to registry";
            }
        }

        void saveApiKeyToRegistry(const QString &apiKeyVal)
        {
            QSettings settings(REGISTRY_PATH, QSettings::NativeFormat);
            settings.setValue(REG_KEY_AMPLITUDE_API_KEY, apiKeyVal);
            qInfo() << "ConfigService:saveApiKeyToRegistry: API Key saved to registry";
        }

        void onServerReplyFinished(QNetworkReply *reply)
        {
            QByteArray responseData = reply->readAll();
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

            reply->deleteLater();

            if (reply->error() != QNetworkReply::NoError)
            {
                qWarning() << "ConfigService:onServerReplyFinished: Failed to fetch API Key from server [Status:" << statusCode << "]"
                           << "\nError:" << reply->errorString()
                           << "\nUsing fallback API Key";
                return;
            }

            qInfo() << "ConfigService:onServerReplyFinished: Server response [Status:" << statusCode << "]:" << responseData;

            // 解析服务端返回的 JSON
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (!doc.isObject())
            {
                qWarning() << "ConfigService:onServerReplyFinished: Invalid JSON response from server";
                return;
            }

            QJsonObject root = doc.object();
            QString serverApiKey = root.value(REG_KEY_AMPLITUDE_API_KEY).toString();

            if (serverApiKey.isEmpty())
            {
                qWarning() << "ConfigService:onServerReplyFinished: Server response does not contain 'amplitude_api_key' field";
                return;
            }

            // 更新 API Key
            if (apiKey != serverApiKey)
            {
                apiKey = serverApiKey;
                saveApiKeyToRegistry(apiKey);
                qInfo() << "ConfigService:onServerReplyFinished: API Key updated from server and saved to registry";
            }
            else
            {
                qInfo() << "ConfigService:onServerReplyFinished: API Key from server matches current value";
            }
        }
    };

    ConfigService::ConfigService(HttpService *httpService, QObject *parent)
        : QObject(parent), d_ptr(new ConfigServicePrivate(this, httpService))
    {
    }

    ConfigService::~ConfigService()
    {
        delete d_ptr;
    }

    void ConfigService::init()
    {
        // 先从注册表加载（兜底逻辑）
        d_ptr->loadApiKeyFromRegistry();

        // 尝试从服务端拉取最新的 API Key
        d_ptr->fetchApiKeyFromServer();
    }

    QString ConfigService::getAmplitudeApiKey() const
    {
        return d_ptr->apiKey;
    }

    QString ConfigService::getEventTrackEndpoint() const
    {
        return EVENT_TRACK_ENDPOINT;
    }

    QString ConfigService::getFeatureFlagEndpoint() const
    {
        return FEATURE_FLAG_ENDPOINT;
    }

} // namespace event_report
