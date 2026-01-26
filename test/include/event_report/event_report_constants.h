#ifndef EVENT_REPORT_CONSTANTS_H
#define EVENT_REPORT_CONSTANTS_H

#include <QString>

namespace event_report
{
    /**
     * @brief 注册表相关常量
     */
    const QString REGISTRY_PATH = "HKEY_CURRENT_USER\\Software\\EventReport";
    const QString REG_KEY_AMPLITUDE_API_KEY = "amplitude_api_key";
    const QString REG_KEY_USER_ID = "user_id";

    /**
     * @brief 服务端相关常量
     */
    const QString STORE_SERVER_URL = "https://your-server.com/api/get-amplitude-key";

    /**
     * @brief Amplitude 相关常量
     */
    const QString DEFAULT_AMPLITUDE_API_KEY = "14283a6672ee87df21326b38aa4a5604";
    const QString EVENT_TRACK_ENDPOINT = "https://api2.amplitude.com/2/httpapi";
    const QString FEATURE_FLAG_ENDPOINT = "https://api.lab.amplitude.com/v1/vardata";
}

#endif // EVENT_REPORT_CONSTANTS_H
