# Qt Event Report Project

这是一个基于 Qt 框架的事件上报和特性标志管理示例项目。它通过封装 DLL 的方式，为 Qt 应用程序提供了完整的用户行为分析、事件上报和特性开关管理能力。

## 项目结构

项目分为两个主要部分：

*   **[`event_report_dll/`](event_report_dll/)**: 核心组件。
    *   提供核心管理类（Event Report Manager）。
    *   封装了事件上报服务（Event Report Service）。
    *   提供特性标志服务（Feature Flag Service）。
    *   支持与后端 API 的网络通信。
*   **[`event_report_example/`](event_report_example/)**: 示例程序。
    *   演示了如何链接并使用 `event_report_dll`。
    *   提供了一个完整的示例界面，展示各个服务的使用方法。

## 核心特性

*   **事件上报**: 记录和上报用户行为事件，支持自定义事件属性。
*   **特性标志管理**: 动态控制应用功能的开关，无需重新发布版本。
*   **网络通信**: 内置 HTTP/HTTPS 请求支持，可与后端服务无缝集成。
*   **Qt 深度集成**: 使用 Qt 的信号槽机制和网络模块，完美融入 Qt 生态。

## 快速开始

### 1. 环境要求
*   **操作系统**: Windows 10 或 Windows 11。
*   **开发环境**: Qt 5.15+ (MSVC 2015/2017/2019)。
*   **网络依赖**: OpenSSL 库（用于 HTTPS 通信）。

### 2. 构建项目
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 文档指引

*   如果你想了解 DLL 的详细接口和集成方法，请阅读 [**Event Report DLL 详细文档**](event_report_dll/README.md)。
*   如果你想查看如何编写调用代码，请参考 [`event_report_example/MainWindow.cpp`](event_report_example/MainWindow.cpp)。

## 许可证
本项目仅供学习与参考使用。
