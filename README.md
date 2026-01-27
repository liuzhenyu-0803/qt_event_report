# EventReport - Qt 事件上报库

这是一个基于 Qt 框架的轻量级事件上报和特性标志管理库。

## 核心能力

- ✅ **异步上报**: 独立工作线程处理网络请求，不阻塞主线程。
- ✅ **断网持久化**: 请求失败时自动保存至本地 AppData，支持后续重发。
- ✅ **特性管理**: 动态拉取和配置特性开关（Feature Flags）。
- ✅ **现代构建**: 完善的 CMake 支持，易于集成到现有 Qt 项目。

## 项目结构

```
event_report/
├── include/                # 公共 API 头文件
├── src/                    # 源代码实现
├── examples/               # 使用示例
├── scripts/                # 构建与安装脚本
└── third_party/            # 依赖库 (OpenSSL DLLs)
```

## 快速集成

### 1. 编译与安装 (Windows)
推荐使用脚本一键安装：
```bash
# 安装到 ./install/release
.\scripts\install_release.bat
```

### 2. 在项目中使用
在 `CMakeLists.txt` 中引用：
```cmake
find_package(EventReport REQUIRED)
target_link_libraries(your_target PRIVATE EventReport::event_report)
```

## 基础示例

```cpp
#include <event_report/event_report_manager.h>
#include <QApplication>

using namespace event_report;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    auto manager = EventReportManager::instance();
    manager->init(); // 初始化服务
    
    // 1. 上报事件
    QVariantMap props;
    props["source"] = "main_window";
    manager->reportEvent("app_start", props);
    
    // 2. 拉取特性标志
    manager->fetchFlags();
    
    // 3. 监听标志变化
    QObject::connect(manager, &EventReportManager::signalFlagsChanged, [](bool success, const QString& err) {
        if (success) qInfo() << "Flags updated";
    });
        
    int result = app.exec();
    manager->shutdown();
    return result;
}
```

## 数据持久化说明

- **失败重试**: 库会自动处理网络异常。失败的事件将以 JSONL 格式保存。
- **本地路径**: `C:/Users/<User>/AppData/Local/<AppName>/event_report/failed_events.jsonl`
- **手动重发**: 可调用 `manager->reportEventsFromJsonFile(path)` 批量重发历史失败数据。

## API 文档摘要

- [`event_report_manager.h`](include/event_report/event_report_manager.h) - 核心入口
- [`event_report_service.h`](include/event_report/event_report_service.h) - 事件服务
- [`feature_flag_service.h`](include/event_report/feature_flag_service.h) - 特性标志
- [`event_report_types.h`](include/event_report/event_report_types.h) - 数据类型

---
*注：本项目主要面向 Windows 平台 Qt 开发，依赖 OpenSSL 1.1.x。*
