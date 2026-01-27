#ifndef EVENT_REPORT_TYPES_H
#define EVENT_REPORT_TYPES_H

#include <QString>
#include <QVariantMap>
#include <QVariant>
#include <QList>
#include <QMap>

namespace event_report
{

    /**
     * @brief 埋点数据结构
     */
    struct TrackEvent
    {
        QString eventType;
        QVariantMap eventProperties;
        QVariantMap userProperties;
    };

    /**
     * @brief 变体信息结构
     */
    struct VariantInfo
    {
        QString key;      ///< 变体 Key (value)
        QVariant payload; ///< 变体负载数据 (JSON)
    };

} // namespace event_report

#endif // EVENT_REPORT_TYPES_H
