#ifndef HTTP_SERVICE_H
#define HTTP_SERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMap>
#include "event_report_dll_export.h"

/**
 * @brief 统一的网络服务类 (单例)，管理 QNetworkAccessManager 资源
 */
class EVENT_REPORT_DLL_API HttpService : public QObject
{
    Q_OBJECT
public:
    explicit HttpService(QObject* parent = nullptr);
    virtual ~HttpService();

    /**
     * @brief 初始化 (在工作线程中执行)
     */
    void init();

    /**
     * @brief 发送 POST 请求
     */
    QNetworkReply* post(const QString& url, const QByteArray& data, const QMap<QByteArray, QByteArray>& headers = {});

    /**
     * @brief 发送 GET 请求
     */
    QNetworkReply* get(const QString& url, const QMap<QByteArray, QByteArray>& headers = {});

private:
    // 禁止拷贝
    HttpService(const HttpService&) = delete;
    HttpService& operator=(const HttpService&) = delete;
private:
    QNetworkAccessManager* m_networkManager = nullptr;
};

#endif // HTTP_SERVICE_H
