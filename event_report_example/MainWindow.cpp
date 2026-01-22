#include "MainWindow.h"
#include "event_report_manager.h"
#include "feature_flag_service.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // 1. 初始化 EventReportManager
    auto manager = EventReportManager::instance();
    manager->init();

    // 2. 连接信号
    connect(manager, &EventReportManager::signalFlagsChanged, [manager](bool success, const QString& error)
    {
        if (success)
        {
            auto flags = manager->getAllFlags();
            qInfo() << "MainWindow: Flags updated successfully, count:" << flags.size();
            for (auto it = flags.begin(); it != flags.end(); ++it)
            {
                qInfo() << "MainWindow: Flag:" << it.key() << "Variant:" << it.value().key << "Payload:" << it.value().payload;
            }
        }
        else
        {
            qWarning() << "MainWindow: Failed to fetch flags:" << error;
        }
    });

    // 拉取 Flags
    manager->fetchFlags();

    // 设置界面
    auto layout = new QVBoxLayout(this);
    auto btnTrack = new QPushButton("Send Test Event", this);
    layout->addWidget(btnTrack);

    connect(btnTrack, &QPushButton::clicked, [manager]()
    {
        QVariantMap props;
        props["button_name"] = "test_button";
        props["page_name"] = "main_page";
        manager->reportEvent("button_clicked", props);
    });

    manager->reportEvent("app_start");
}

MainWindow::~MainWindow()
{
}
