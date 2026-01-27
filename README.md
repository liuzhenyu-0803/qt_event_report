# EventReport - Qt Event Reporting Library

这是一个基于 Qt 框架的事件上报和特性标志管理库。它采用现代 CMake 项目结构，为 Qt 应用程序提供了完整的用户行为分析、事件上报和特性开关管理能力。

## 项目特点

- ✅ **现代 CMake 结构**: 遵循业界最佳实践的目录组织
- ✅ **清晰的接口设计**: 公共头文件与实现分离
- ✅ **完善的构建系统**: 支持内部构建和外部集成
- ✅ **Qt 深度集成**: 使用信号槽机制和网络模块
- ✅ **高性能架构**: 单线程 Worker 模型与无锁化设计
- ✅ **调试支持**: 支持 MSVC 在 Release 模式下生成 PDB 调试信息，方便线上故障排查

## 项目结构

```
event_report/
├── CMakeLists.txt          # 顶层 CMake 配置
├── README.md               # 项目说明文档
├── include/                # 公共头文件目录
│   └── event_report/       # 库的公共 API
│       ├── config_service.h
│       ├── event_report_manager.h
│       ├── event_report_service.h
│       ├── feature_flag_service.h
│       ├── http_service.h
│       ├── identity_service.h
│       ├── event_report_constants.h
│       └── event_report_export.h
├── src/                    # 库源代码实现
│   ├── CMakeLists.txt
│   ├── config_service.cpp
│   ├── event_report_manager.cpp
│   ├── event_report_service.cpp
│   ├── feature_flag_service.cpp
│   ├── http_service.cpp
│   └── identity_service.cpp
├── examples/               # 示例程序
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── MainWindow.cpp
│   └── MainWindow.h
└── third_party/            # 第三方依赖库
    ├── libcrypto-1_1-x64.dll
    └── libssl-1_1-x64.dll
```

## 核心功能

### 事件上报 (Event Reporting)
记录和上报用户行为事件，支持自定义事件属性和批量处理。

### 特性标志管理 (Feature Flags)
动态控制应用功能的开关，支持 A/B 测试和灰度发布。

### 网络通信 (HTTP Service)
内置 HTTP/HTTPS 请求支持，基于 Qt Network 模块实现。

### 身份管理 (Identity Service)
用户身份识别和设备指纹管理。

### 配置服务 (Config Service)
统一的配置管理，支持本地存储和远程同步。

## 快速开始

### 环境要求

- **操作系统**: Windows 10/11, Linux, macOS
- **Qt 版本**: Qt 5.15+ (建议使用 Qt 5.15.17)
- **编译器**: MSVC 2015+, GCC 7+, Clang 5+
- **CMake**: 3.16 或更高版本
- **依赖库**: OpenSSL 1.1.x (用于 HTTPS 通信)

### 构建步骤

#### 1. 克隆项目
```bash
git clone <repository-url>
cd event_report
```

#### 2. 配置 CMake（首次构建）
```bash
# Windows (Visual Studio 2015)
cmake -S . -B build -G "Visual Studio 14 2015" -A x64

# Windows (Visual Studio 2019)
cmake -S . -B build -G "Visual Studio 16 2019" -A x64

# Linux/macOS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

#### 3. 编译项目
```bash
# Windows
cmake --build build --config Release

# Linux/macOS
cmake --build build
```

#### 4. 运行示例
```bash
# Windows
.\build\bin\Release\event_report_example.exe

# Linux/macOS
./build/bin/event_report_example
```

### 自定义 Qt 路径

如果 Qt 未在系统路径中，可以通过 `CMAKE_PREFIX_PATH` 指定：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="D:/Qt/Qt5.15.17/5.15.17/msvc2015_64"
```

### 仅构建库（不构建示例）

```bash
cmake -S . -B build -DBUILD_EXAMPLES=OFF
cmake --build build --config Release
```

### 安装项目

本项目支持通过 CMake 的 `install` 命令进行安装。安装内容包括库文件、头文件、第三方 DLL 以及 CMake 配置文件。

#### 使用脚本一键安装 (Windows)
直接运行项目根目录下的对应脚本：
```bash
# 安装 Release 版本
.\install_release.bat

# 安装 Debug 版本
.\install_debug.bat
```

#### 使用命令行安装
```bash
# 1. 配置并设置安装路径（默认为 ./install）
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=./install

# 2. 编译
cmake --build build --config Release

# 3. 安装
cmake --install build --config Release
```

## 使用方法

### 基础示例

```cpp
#include <event_report/event_report_manager.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 1. 获取单例实例
    auto manager = EventReportManager::instance();
    
    // 2. 初始化
    manager->init();
    
    // 3. 上报事件
    manager->reportEvent("app_start");
    
    // 4. 上报带属性的事件
    QVariantMap props;
    props["button_name"] = "login_button";
    props["page_name"] = "main_page";
    manager->reportEvent("button_clicked", props);
    
    // 5. 拉取特性标志
    manager->fetchFlags();
    
    // 6. 连接信号监听结果
    QObject::connect(manager, &EventReportManager::signalFlagsChanged,
        [](bool success, const QString& error) {
            if (success) {
                qInfo() << "Feature flags updated successfully";
            }
        });
    
    return app.exec();
}
```

### 在其他项目中集成

#### 作为 CMake 子项目

在你的 `CMakeLists.txt` 中添加：

```cmake
add_subdirectory(path/to/event_report)

target_link_libraries(your_target PRIVATE EventReport::event_report)
```

#### 使用已安装的库

```cmake
find_package(EventReport REQUIRED)
target_link_libraries(your_target PRIVATE EventReport::event_report)
```

#### 包含头文件

```cpp
#include <event_report/event_report_manager.h>
#include <event_report/feature_flag_service.h>
```

## CMake 配置选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_EXAMPLES` | `ON` | 是否构建示例程序 |
| `CMAKE_PREFIX_PATH` | (自动检测) | Qt 安装路径 |
| `CMAKE_BUILD_TYPE` | `Release` | 构建类型 (Release/Debug) |

## API 文档

详细的 API 文档请参考各个头文件中的注释说明：

- [`event_report_manager.h`](include/event_report/event_report_manager.h) - 核心管理类
- [`event_report_service.h`](include/event_report/event_report_service.h) - 事件上报服务
- [`feature_flag_service.h`](include/event_report/feature_flag_service.h) - 特性标志服务
- [`http_service.h`](include/event_report/http_service.h) - HTTP 请求服务
- [`identity_service.h`](include/event_report/identity_service.h) - 身份识别服务
- [`config_service.h`](include/event_report/config_service.h) - 配置管理服务

## 示例代码

完整的使用示例请参考：
- [`examples/MainWindow.cpp`](examples/MainWindow.cpp) - 主窗口示例
- [`examples/main.cpp`](examples/main.cpp) - 应用入口

## 技术架构

### 线程模型
- 主线程：Qt GUI 和信号槽通信
- Worker 线程：事件批量处理和网络请求

### 核心设计
- **单例模式**: EventReportManager 确保全局唯一实例
- **观察者模式**: 使用 Qt 信号槽实现事件通知
- **策略模式**: 支持不同的上报策略和存储策略
- **工厂模式**: 服务实例的创建和管理

## 构建系统说明

本项目采用现代 CMake（3.16+）最佳实践：

1. **目标传递性**: 使用 `target_*` 命令而非全局变量
2. **生成器表达式**: 支持构建时和安装时的不同路径
3. **命名空间**: 使用 `EventReport::` 前缀避免命名冲突
4. **安装规则**: 支持标准的 CMake 安装流程
5. **导出配置**: 可被其他 CMake 项目直接引用

## 故障排除

### 找不到 Qt

确保设置了正确的 `CMAKE_PREFIX_PATH`：
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="你的Qt路径"
```

### 缺少 OpenSSL

将 `libcrypto-1_1-x64.dll` 和 `libssl-1_1-x64.dll` 放入 `third_party/` 目录。

### 链接错误

确保 DLL 和可执行文件在同一目录，或者 DLL 路径在系统 PATH 中。

## 性能特点

- **批量上报**: 事件批量提交，减少网络请求
- **异步处理**: 非阻塞的后台处理机制
- **内存优化**: 事件队列限制和内存池管理
- **网络优化**: 请求合并和失败重试机制

## 许可证

本项目仅供学习与参考使用。

## 贡献

欢迎提交 Issue 和 Pull Request。

## 更新日志

### v1.0.0 (2024)
- ✅ 采用现代 CMake 项目结构
- ✅ 重构目录布局（include/, src/, examples/）
- ✅ 优化构建系统配置
- ✅ 改进 DLL 导出宏处理
- ✅ 完善文档和示例代码
- ✅ 支持 MSVC 在 Release 模式下生成 PDB 调试信息
