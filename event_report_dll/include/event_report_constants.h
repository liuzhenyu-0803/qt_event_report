#ifndef EVENT_REPORT_CONSTANTS_H
#define EVENT_REPORT_CONSTANTS_H

#include <QString>

namespace EventReport
{
    /**
     * @brief 注册表相关常量
     */
    inline const QString REGISTRY_PATH = "HKEY_CURRENT_USER\\Software\\EventReport";
    inline const QString REG_KEY_AMPLITUDE_API_KEY = "amplitude_api_key";
    inline const QString REG_KEY_USER_ID = "user_id";

    /**
     * @brief 配置相关默认值与端点
     */
    inline const QString DEFAULT_AMPLITUDE_API_KEY = "14283a6672ee87df21326b38aa4a5604";
    inline const QString EVENT_TRACK_ENDPOINT = "https://api2.amplitude.com/2/httpapi";
    inline const QString FEATURE_FLAG_ENDPOINT = "https://api.lab.amplitude.com/v1/vardata";
    inline const QString API_KEY_SERVER_URL = "https://your-server.com/api/get-amplitude-key";
}

#endif // EVENT_REPORT_CONSTANTS_H
