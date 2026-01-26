#include "event_report/config_service.h"
#include "event_report/http_service.h"
#include <QSettings>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "event_report/event_report_constants.h"

namespace event_report {

ConfigService::ConfigService(HttpService* httpService, QObject* parent)
    : QObject(parent)
    , m_httpService(httpService)
{
}

ConfigService::~ConfigService()
{
}

void ConfigService::init()
{
    // 先从注册表加载（兜底逻辑）
    loadApiKeyFromRegistry();

    // 尝试从服务端拉取最新的 API Key
    fetchApiKeyFromServer();
}

QString ConfigService::getAmplitudeApiKey() const
{
    return m_apiKey;
}

QString ConfigService::getEventTrackEndpoint() const
{
    return EVENT_TRACK_ENDPOINT;
}

QString ConfigService::getFeatureFlagEndpoint() const
{
    return FEATURE_FLAG_ENDPOINT;
}

void ConfigService::fetchApiKeyFromServer()
{
    qInfo() << "ConfigService:fetchApiKeyFromServer: Fetching API Key from server:" << STORE_SERVER_URL;

    QNetworkReply* reply = m_httpService->get(STORE_SERVER_URL);
    connect(reply, &QNetworkReply::finished, this, [this, reply]()
    {
        onServerReplyFinished(reply);
    });
}

void ConfigService::loadApiKeyFromRegistry()
{
    QSettings settings(REGISTRY_PATH, QSettings::NativeFormat);
    
    QString registryApiKey = settings.value(REG_KEY_AMPLITUDE_API_KEY).toString();
    if (!registryApiKey.isEmpty())
    {
        m_apiKey = registryApiKey;
        qInfo() << "ConfigService:loadApiKeyFromRegistry: API Key loaded from registry";
    }
    else
    {
        // 注册表中没有，使用硬编码默认值并写入注册表
        m_apiKey = DEFAULT_AMPLITUDE_API_KEY;
        saveApiKeyToRegistry(m_apiKey);
        qInfo() << "ConfigService:loadApiKeyFromRegistry: Using default API Key and saved to registry";
    }
}

void ConfigService::saveApiKeyToRegistry(const QString& apiKey)
{
    QSettings settings(REGISTRY_PATH, QSettings::NativeFormat);
    settings.setValue(REG_KEY_AMPLITUDE_API_KEY, apiKey);
    qInfo() << "ConfigService:saveApiKeyToRegistry: API Key saved to registry";
}

void ConfigService::onServerReplyFinished(QNetworkReply* reply)
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
    if (m_apiKey != serverApiKey)
    {
        m_apiKey = serverApiKey;
        saveApiKeyToRegistry(m_apiKey);
        qInfo() << "ConfigService:onServerReplyFinished: API Key updated from server and saved to registry";
    }
    else
    {
        qInfo() << "ConfigService:onServerReplyFinished: API Key from server matches current value";
    }
}

} // namespace event_report
