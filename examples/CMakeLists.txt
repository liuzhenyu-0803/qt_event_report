# 示例程序源文件
set(EXAMPLE_SOURCES
    main.cpp
    MainWindow.cpp
)

set(EXAMPLE_HEADERS
    MainWindow.h
)

# 创建可执行文件
add_executable(event_report_example
    ${EXAMPLE_SOURCES}
    ${EXAMPLE_HEADERS}
)

# 设置目标属性
set_target_properties(event_report_example PROPERTIES
    WIN32_EXECUTABLE TRUE
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# 查找 Qt5 组件
find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

# 链接库
target_link_libraries(event_report_example
    PRIVATE
        Qt5::Widgets
        Qt5::Network
        EventReport::event_report
)
