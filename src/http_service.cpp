#include "event_report/http_service.h"

namespace event_report {

HttpService::HttpService(QObject* parent) : QObject(parent)
{
}

void HttpService::init()
{
    m_networkManager = new QNetworkAccessManager(this);
}

HttpService::~HttpService()
{
}

QNetworkReply* HttpService::post(const QString& url, const QByteArray& data, const QMap<QByteArray, QByteArray>& headers)
{
    QNetworkRequest request(url);
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        request.setRawHeader(it.key(), it.value());
    }
    
    if (!request.hasRawHeader("Content-Type"))
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }

    return m_networkManager->post(request, data);
}

QNetworkReply* HttpService::get(const QString& url, const QMap<QByteArray, QByteArray>& headers)
{
    QNetworkRequest request(url);
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        request.setRawHeader(it.key(), it.value());
    }

    return m_networkManager->get(request);
}

} // namespace event_report
