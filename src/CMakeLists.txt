# 库源文件和头文件
set(EVENT_REPORT_SOURCES
    config_service.cpp
    event_report_manager.cpp
    event_report_service.cpp
    feature_flag_service.cpp
    http_service.cpp
    identity_service.cpp
)

set(EVENT_REPORT_HEADERS
    ${PROJECT_SOURCE_DIR}/include/event_report/config_service.h
    ${PROJECT_SOURCE_DIR}/include/event_report/event_report_constants.h
    ${PROJECT_SOURCE_DIR}/include/event_report/event_report_export.h
    ${PROJECT_SOURCE_DIR}/include/event_report/event_report_manager.h
    ${PROJECT_SOURCE_DIR}/include/event_report/event_report_service.h
    ${PROJECT_SOURCE_DIR}/include/event_report/feature_flag_service.h
    ${PROJECT_SOURCE_DIR}/include/event_report/http_service.h
    ${PROJECT_SOURCE_DIR}/include/event_report/identity_service.h
)

# 创建共享库
add_library(event_report SHARED
    ${EVENT_REPORT_SOURCES}
    ${EVENT_REPORT_HEADERS}
)

# 为库添加别名，遵循现代 CMake 命名约定
add_library(EventReport::event_report ALIAS event_report)

# 设置目标属性
set_target_properties(event_report PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "event_report"
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

# Qt 自动化工具
set_target_properties(event_report PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# DLL 导出宏定义（仅在编译库时生效）
target_compile_definitions(event_report PRIVATE EVENT_REPORT_EXPORTS)

# 头文件包含路径（使用生成器表达式支持构建和安装）
target_include_directories(event_report
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 查找 Qt5 组件
find_package(Qt5 REQUIRED COMPONENTS Core Network Concurrent)

# 链接 Qt 库（PUBLIC 确保依赖传递）
target_link_libraries(event_report
    PUBLIC
        Qt5::Core
        Qt5::Network
        Qt5::Concurrent
)

# 拷贝第三方库到输出目录
set(THIRD_PARTY_DIR "${PROJECT_SOURCE_DIR}/third_party")
file(GLOB THIRD_PARTY_DLLS "${THIRD_PARTY_DIR}/*.dll")

if(THIRD_PARTY_DLLS)
    foreach(dll_file ${THIRD_PARTY_DLLS})
        add_custom_command(TARGET event_report POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${dll_file}"
                $<TARGET_FILE_DIR:event_report>
            COMMENT "Copying ${dll_file} to output directory"
            VERBATIM
        )
    endforeach()
endif()

# 安装规则
include(GNUInstallDirs)

install(TARGETS event_report
    EXPORT EventReportTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/event_report
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 导出目标配置
install(EXPORT EventReportTargets
    FILE EventReportTargets.cmake
    NAMESPACE EventReport::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/EventReport
)
