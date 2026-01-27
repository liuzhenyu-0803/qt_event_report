#ifndef FEATURE_FLAG_SERVICE_H
#define FEATURE_FLAG_SERVICE_H

#include "event_report_types.h"
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QVariant>
#include <QMutex>
#include <QStringList>

namespace event_report
{

    // 前置声明
    class IdentityService;
    class ConfigService;
    class HttpService;

    /**
     * @brief Feature Flag 服务，负责从 Amplitude 获取功能开关内容
     */
    class FeatureFlagService : public QObject
    {
        Q_OBJECT
    
    signals:
        /**
         * @brief Feature Flags 状态变化信号
         * @param success 是否成功
         * @param error 错误信息
         */
        void signalFlagsChanged(bool success, const QString &error = QString());

    public:
        explicit FeatureFlagService(IdentityService *identityService,
                                    ConfigService *configService,
                                    HttpService *httpService,
                                    QObject *parent = nullptr);
        virtual ~FeatureFlagService();

        /**
         * @brief 初始化 (在工作线程中执行)
         */
        void init();

        /**
         * @brief 从服务器拉取最新的 Flags
         */
        void fetchFlags(const QStringList &flagKeys = QStringList());

        /**
         * @brief 获取所有已加载的 Flags
         */
        QMap<QString, VariantInfo> getAllFlags() const;

        /**
         * @brief 获取指定 Key 的 Flag 信息
         */
        VariantInfo getFlag(const QString &flagKey) const;

    private:
        // 禁止拷贝
        FeatureFlagService(const FeatureFlagService &) = delete;
        FeatureFlagService &operator=(const FeatureFlagService &) = delete;

    private:
        class FeatureFlagServicePrivate *d_ptr;
    };

} // namespace event_report

#endif // FEATURE_FLAG_SERVICE_H
