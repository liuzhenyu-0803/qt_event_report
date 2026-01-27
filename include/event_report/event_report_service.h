#ifndef EVENT_REPORT_SERVICE_H
#define EVENT_REPORT_SERVICE_H

#include "event_report_types.h"
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>

namespace event_report
{

    // 前置声明
    class IdentityService;
    class ConfigService;
    class HttpService;

    /**
     * @brief 底层上报服务，直接负责与 Amplitude 通信
     */
    class EventReportService : public QObject
    {
        Q_OBJECT
    public:
        explicit EventReportService(IdentityService *identityService,
                                    ConfigService *configService,
                                    HttpService *httpService,
                                    QObject *parent = nullptr);
        virtual ~EventReportService();

        /**
         * @brief 初始化 (在工作线程中执行)
         */
        void init();

        /**
         * @brief 发送原始埋点数据
         */
        void reportEvent(const TrackEvent &event);

        /**
         * @brief 发送原始埋点数据 (重载便捷接口)
         */
        void reportEvent(const QString &eventType, const QVariantMap &eventProperties = QVariantMap(), const QVariantMap &userProperties = QVariantMap());

        /**
         * @brief 批量发送埋点数据
         */
        void reportEvents(const QList<TrackEvent> &events);

        /**
         * @brief 从 JSON 文件中批量发送埋点 (通常用于补报)
         */
        void reportEventsFromJsonFile(const QString &filePath);

    private:
        // 禁止拷贝
        EventReportService(const EventReportService &) = delete;
        EventReportService &operator=(const EventReportService &) = delete;

    private:
        class EventReportServicePrivate *d_ptr;
    };

} // namespace event_report

#endif // EVENT_REPORT_SERVICE_H
