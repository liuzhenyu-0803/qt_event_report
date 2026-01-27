#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include <QObject>
#include <QString>
namespace event_report
{

    /**
     * @brief 身份服务类（单例模式），管理用户和设备标识
     */
    class IdentityService : public QObject
    {
        Q_OBJECT
    public:
        explicit IdentityService(QObject *parent = nullptr);
        virtual ~IdentityService();

        /**
         * @brief 初始化 (在工作线程中执行)
         */
        void init();

        /**
         * @brief 获取当前用户唯一标识
         */
        QString getUserID();

        /**
         * @brief 获取当前设备唯一标识
         */
        QString getDeviceID();

    private:
        // 禁止拷贝
        IdentityService(const IdentityService &) = delete;
        IdentityService &operator=(const IdentityService &) = delete;

    private:
        class IdentityServicePrivate *d_ptr;
    };

} // namespace event_report

#endif // IDENTITY_SERVICE_H
