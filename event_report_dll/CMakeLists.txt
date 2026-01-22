cmake_minimum_required(VERSION 3.16)

project(event_report_dll VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 如果不是作为子项目构建，则需要设置这些变量
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.15.17/5.15.17/msvc2015_64")
endif()

# 开启多核编译
if(MSVC)
    add_compile_options(/MP)
endif()

# 定义导出宏
add_compile_definitions(EVENT_REPORT_DLL_EXPORTS)

# 查找 Qt 依赖
find_package(Qt5 REQUIRED COMPONENTS Core Network Concurrent)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 使用通配符方式自动导入头文件和源文件
file(GLOB INC_FILES CONFIGURE_DEPENDS "${INC_DIR}/*.h")
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")

# 生成动态库
add_library(event_report_dll SHARED
    ${INC_FILES}
    ${SRC_FILES}
)

# 设置包含目录
target_include_directories(event_report_dll
    PUBLIC
        ${INC_DIR}
)

# 链接 Qt 库
target_link_libraries(event_report_dll
    PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Concurrent
)

# 设置输出文件名
set_target_properties(event_report_dll PROPERTIES
    OUTPUT_NAME "event_report"
)

# 仅在独立构建时设置输出目录到项目下的 bin 目录
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set_target_properties(event_report_dll PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
        PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    )
endif()

# Release 模式下生成 PDB
if(MSVC)
    target_compile_options(event_report_dll PRIVATE $<$<CONFIG:Release>:/Zi>)
    target_link_options(event_report_dll PRIVATE $<$<CONFIG:Release>:/DEBUG /OPT:REF /OPT:ICF>)
endif()

# 拷贝第三方库到生成目录
file(GLOB THIRD_PARTY_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/*.dll")

if(THIRD_PARTY_LIBS)
    add_custom_command(TARGET event_report_dll POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${THIRD_PARTY_LIBS}
            $<TARGET_FILE_DIR:event_report_dll>
        COMMENT "Copying third party libraries to output directory..."
    )
endif()
