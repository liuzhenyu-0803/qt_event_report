#include "event_report/http_service.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>

namespace event_report
{

    class HttpServicePrivate
    {
    public:
        explicit HttpServicePrivate(HttpService *q) : q_ptr(q) {}

        HttpService *q_ptr;
        QNetworkAccessManager *networkManager = nullptr;
    };

    HttpService::HttpService(QObject *parent)
        : QObject(parent), d_ptr(new HttpServicePrivate(this))
    {
    }

    HttpService::~HttpService()
    {
        delete d_ptr;
    }

    void HttpService::init()
    {
        d_ptr->networkManager = new QNetworkAccessManager(d_ptr->q_ptr);
    }

    QNetworkReply *HttpService::post(const QString &url, const QByteArray &data, const QMap<QByteArray, QByteArray> &headers)
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

        return d_ptr->networkManager->post(request, data);
    }

    QNetworkReply *HttpService::get(const QString &url, const QMap<QByteArray, QByteArray> &headers)
    {
        QNetworkRequest request(url);
        for (auto it = headers.begin(); it != headers.end(); ++it)
        {
            request.setRawHeader(it.key(), it.value());
        }

        return d_ptr->networkManager->get(request);
    }

} // namespace event_report
