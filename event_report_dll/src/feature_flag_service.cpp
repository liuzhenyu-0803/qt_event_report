#include "feature_flag_service.h"
#include "identity_service.h"
#include "config_service.h"
#include "http_service.h"
#include <QUrl>
#include <QUrlQuery>
#include <QMutexLocker>
#include <QNetworkRequest>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

FeatureFlagService::FeatureFlagService(IdentityService* identityService,
                                       ConfigService* configService,
                                       HttpService* httpService,
                                       QObject* parent)
    : QObject(parent)
    , m_identityService(identityService)
    , m_configService(configService)
    , m_httpService(httpService)
{
}

FeatureFlagService::~FeatureFlagService()
{
}

void FeatureFlagService::init()
{
    // 目前无需特殊初始化逻辑，作为接口预留
}

void FeatureFlagService::fetchFlags(const QStringList& flagKeys)
{
    QString endpoint = m_configService->getFeatureFlagEndpoint();
    QUrl url(endpoint);
    QUrlQuery query;
    query.addQueryItem("user_id", m_identityService->getUserID());
    query.addQueryItem("device_id", m_identityService->getDeviceID());
    
    if (!flagKeys.isEmpty())
    {
        query.addQueryItem("flag_keys", flagKeys.join(","));
    }
    url.setQuery(query);

    QMap<QByteArray, QByteArray> headers;
    headers.insert("Authorization", QString("Api-Key %1").arg(m_configService->getAmplitudeApiKey()).toUtf8());
    
    qInfo() << "FeatureFlagService:fetchFlags: Requesting flags from URL:" << url.toString();

    QNetworkReply* reply = m_httpService->get(url.toString(), headers);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

QMap<QString, VariantInfo> FeatureFlagService::getAllFlags() const
{
    QMutexLocker locker(&m_mutex);
    return m_flags;
}

VariantInfo FeatureFlagService::getFlag(const QString& flagKey) const
{
    QMutexLocker locker(&m_mutex);
    return m_flags.value(flagKey);
}

void FeatureFlagService::onReplyFinished(QNetworkReply* reply)
{
    QByteArray responseData = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        qWarning() << "FeatureFlagService:onReplyFinished: Failed [Status:" << statusCode << "]"
                   << "\nError:" << reply->errorString()
                   << "\nResponse:" << responseData;
        emit signalFlagsChanged(false, reply->errorString());
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
        QMutexLocker locker(&m_mutex);
        m_flags.clear();
        QJsonObject root = doc.object();
        for (auto it = root.begin(); it != root.end(); ++it)
        {
            QString flagKey = it.key();
            QJsonObject variantObj = it.value().toObject();
            
            VariantInfo info;
            info.key = variantObj.value("key").toString();
            info.payload = variantObj.value("payload").toVariant();
            
            m_flags.insert(flagKey, info);
        }
    }

    qInfo() << "FeatureFlagService:onReplyFinished: Successfully loaded" << m_flags.size() << "flags";
    emit signalFlagsChanged(true);
}
