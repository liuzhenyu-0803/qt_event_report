#ifndef EVENT_REPORT_DLL_EXPORT_H
#define EVENT_REPORT_DLL_EXPORT_H

#ifdef _WIN32
    #ifdef EVENT_REPORT_DLL_EXPORTS
        #define EVENT_REPORT_DLL_API __declspec(dllexport)
    #else
        #define EVENT_REPORT_DLL_API __declspec(dllimport)
    #endif
#else
    #define EVENT_REPORT_DLL_API
#endif

#endif // EVENT_REPORT_DLL_EXPORT_H

