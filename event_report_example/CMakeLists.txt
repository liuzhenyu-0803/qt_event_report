cmake_minimum_required(VERSION 3.16)

project(event_report_example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 如果不是作为子项目构建，则需要设置这些变量
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.15.17/5.15.17/msvc2015_64")
endif()

find_package(Qt5 REQUIRED COMPONENTS Widgets Network)

# 项目源文件
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

# 创建可执行文件
add_executable(event_report_example
    ${PROJECT_SOURCES}
)

# 链接逻辑
if(TARGET event_report_dll)
    # ==========================================
    # 场景 A：作为子项目构建 (顶层集成)
    # ==========================================
    target_link_libraries(event_report_example PRIVATE
        Qt5::Widgets
        Qt5::Network
        event_report_dll
    )

    # 在集成构建模式下，由于我们在顶层 CMakeLists.txt 中设置了统一的输出目录 (CMAKE_RUNTIME_OUTPUT_DIRECTORY)，
    # event_report_dll 和第三方 DLL 已经生成或拷贝到了统一的 bin 目录。
    # 因此这里不再需要重复执行拷贝操作，避免 wildcard 通配符在 cmake -E 中失效以及路径冲突问题。
else()
    # ==========================================
    # 场景 B：独立构建模式 (使用预编译库)
    # ==========================================
    
    # 设置包含目录
    target_include_directories(event_report_example PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../event_report_dll/include"
    )

    # 库文件位于同级库目录下的 bin 文件夹中
    set(EVENT_REPORT_DLL_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../event_report_dll/bin")
    
    # 手动链接具体的 .lib 文件路径
    target_link_libraries(event_report_example PRIVATE
        Qt5::Widgets
        Qt5::Network
        debug     "${EVENT_REPORT_DLL_BASE_DIR}/Debug/event_report.lib"
        optimized "${EVENT_REPORT_DLL_BASE_DIR}/Release/event_report.lib"
    )

    # 独立构建时，必须手动拷贝 DLL 到本工程输出目录
    add_custom_command(TARGET event_report_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${EVENT_REPORT_DLL_BASE_DIR}/$<CONFIG>/event_report.dll"
            "$<TARGET_FILE_DIR:event_report_example>/event_report.dll"
        COMMENT "Independent Build: Copying event_report.dll to output directory..."
        VERBATIM
    )
    
    # 拷贝第三方库（OpenSSL DLL）
    file(GLOB THIRD_PARTY_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/../event_report_dll/third_party/*.dll")
    if(THIRD_PARTY_DLLS)
        add_custom_command(TARGET event_report_example POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${THIRD_PARTY_DLLS}
                "$<TARGET_FILE_DIR:event_report_example>/"
            COMMENT "Independent Build: Copying third party DLLs to output directory..."
            VERBATIM
        )
    endif()
endif()

# 设置可执行文件属性
set_target_properties(event_report_example PROPERTIES
    WIN32_EXECUTABLE TRUE
)
