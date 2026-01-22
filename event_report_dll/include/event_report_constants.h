#ifndef EVENT_REPORT_CONSTANTS_H
#define EVENT_REPORT_CONSTANTS_H

#include <QString>

namespace EventReport
{
    /**
     * @brief 注册表根路径
     */
    inline const QString REGISTRY_PATH = "HKEY_CURRENT_USER\\Software\\EventReport";

    /**
     * @brief 注册表中的 API Key 键名
     */
    inline const QString REG_KEY_AMPLITUDE_API_KEY = "amplitude_api_key";

    /**
     * @brief 注册表中的用户唯一标识键名
     */
    inline const QString REG_KEY_USER_ID = "user_id";
}

#endif // EVENT_REPORT_CONSTANTS_H
