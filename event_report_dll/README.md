# Event Report DLL

基于 Qt 框架封装的事件上报和特性标志管理库。采用单工作线程架构，确保主线程 UI 的流畅性，同时提供线程安全的接口。

## 技术栈
*   **网络通信**: Qt Network 模块（支持 HTTP/HTTPS）
*   **框架**: Qt 5.15.x
*   **构建工具**: CMake

## 操作系统要求
*   **支持系统**: Windows 7/8/10/11
*   **编译器**: MSVC 2015 或更高版本

## 核心依赖
*   Qt 5.15+ (Core、Network 模块)
*   OpenSSL (用于 HTTPS 通信，DLL 文件位于 `third_party` 目录)

## 架构设计
本库采用 **"Manager + Worker Thread + Services"** 架构：
1.  **EventReportManager**: 核心管理类，单例模式。负责维护工作线程生命周期，并统一调度所有服务。
2.  **Worker Thread**: 独立的后台线程，所有 I/O 密集型（文件、网络、注册表）操作均在此线程执行。
3.  **Services**: 各业务服务模块。在主线程创建后被移动至工作线程，并通过 `init()` 方法在目标线程完成初始化。

## 主要功能
*   **事件上报服务（Event Report Service）**
    *   记录和上报用户行为事件，支持自定义属性和批量上报。
*   **特性标志服务（Feature Flag Service）**
    *   动态控制功能开关，支持远程拉取。
*   **身份服务（Identity Service）**
    *   管理用户唯一标识和设备指纹（基于 WMIC 获取硬件信息）。
*   **配置服务（Config Service）**
    *   管理 API Key 和 Endpoint，支持从远程服务器动态更新 Key 并持久化到注册表。
*   **HTTP 服务（Http Service）**
    *   统一管理网络请求资源。

## 接口说明

### EventReportManager 类 (核心接口)
所有应用层交互应通过 `EventReportManager::instance()` 进行。

#### 初始化与停止
*   `void init()`: 初始化所有子服务并启动工作线程。
*   `void shutdown()`: 停止工作线程并清理资源。

#### 事件上报
*   `void reportEvent(const QString& eventType, const QVariantMap& eventProperties, const QVariantMap& userProperties)`: 上报单个事件（线程安全）。
*   `void reportEvents(const QList<TrackEvent>& events)`: 批量上报事件（线程安全）。
*   `void reportEventsFromJsonFile(const QString& filePath)`: 从 JSON 文件补报事件。

#### Feature Flag
*   `void fetchFlags(const QStringList& flagKeys)`: 异步拉取特性标志。
*   `QMap<QString, VariantInfo> getAllFlags() const`: 获取当前所有标志（线程安全）。
*   `VariantInfo getFlag(const QString& flagKey) const`: 获取指定标志（线程安全）。

#### 信号
*   `void signalFlagsChanged(bool success, const QString& error)`: 标志状态变化信号。

## 使用示例

### 1. 初始化
在应用启动时执行初始化：
```cpp
#include "event_report_manager.h"

// 启动服务
EventReportManager::instance()->init();
```

### 2. 事件上报
可以在任何线程直接调用：
```cpp
#include "event_report_manager.h"

QVariantMap props;
props["button_id"] = "login_btn";
EventReportManager::instance()->reportEvent("click", props);
```

### 3. 获取特性标志
```cpp
#include "event_report_manager.h"

// 获取缓存的标志
auto flag = EventReportManager::instance()->getFlag("new_ui_theme");
if (flag.key == "dark_mode") {
    // 启用深色模式
}

// 监听异步更新
connect(EventReportManager::instance(), &EventReportManager::signalFlagsChanged, 
        [](bool success, const QString& error) {
    if (success) {
        qDebug() << "Flags updated successfully";
    }
});
```

## 构建说明
1.  确保已安装 Qt (推荐 MSVC 版本)。
2.  OpenSSL DLL 已内置在 `third_party` 目录下。
3.  使用 CMake 进行配置和构建。

## 第三方库
*   `libcrypto-1_1-x64.dll`: OpenSSL 加密库
*   `libssl-1_1-x64.dll`: OpenSSL SSL/TLS 库

这些库会在构建时自动拷贝到输出目录。
