#include "event_report/identity_service.h"
#include <QSettings>
#include <QUuid>
#include <QProcess>
#include <QRegExp>
#include <QSysInfo>
#include <QDebug>
#include <QThread>
#include "event_report/event_report_constants.h"

namespace event_report
{

    class IdentityServicePrivate
    {
    public:
        explicit IdentityServicePrivate(IdentityService *q) : q_ptr(q) {}

        IdentityService *q_ptr;
        QString userId = "";
        QString deviceId = "";
    };

    IdentityService::IdentityService(QObject *parent)
        : QObject(parent), d_ptr(new IdentityServicePrivate(this))
    {
    }

    IdentityService::~IdentityService()
    {
        delete d_ptr;
    }

    QString IdentityService::getUserID()
    {
        if (!d_ptr->userId.isEmpty())
            return d_ptr->userId;

        QSettings settings(REGISTRY_PATH, QSettings::NativeFormat);
        d_ptr->userId = settings.value(REG_KEY_USER_ID).toString();
        if (d_ptr->userId.isEmpty())
        {
            d_ptr->userId = QUuid::createUuid().toString(QUuid::WithoutBraces);
            settings.setValue(REG_KEY_USER_ID, d_ptr->userId);
        }
        return d_ptr->userId;
    }

    QString IdentityService::getDeviceID()
    {
        if (!d_ptr->deviceId.isEmpty())
            return d_ptr->deviceId;

        QString cpuInfo = "Unknown CPU";
        QString memInfo = "Unknown Mem";
        QString gpuInfo = "Unknown GPU";

        QProcess process;
        process.start("wmic", QStringList() << "cpu" << "get" << "name");
        if (process.waitForFinished())
        {
            QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
            QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.size() > 1)
                cpuInfo = lines[1].trimmed();
        }

        process.start("wmic", QStringList() << "ComputerSystem" << "get" << "TotalPhysicalMemory");
        if (process.waitForFinished())
        {
            QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
            QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.size() > 1)
            {
                bool ok;
                qlonglong bytes = lines[1].trimmed().toLongLong(&ok);
                if (ok)
                    memInfo = QString("%1GB").arg(qRound(bytes / (1024.0 * 1024.0 * 1024.0)));
            }
        }

        process.start("wmic", QStringList() << "path" << "win32_VideoController" << "get" << "name");
        if (process.waitForFinished())
        {
            QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
            QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
            if (lines.size() > 1)
                gpuInfo = lines[1].trimmed();
        }

        d_ptr->deviceId = QString("CPU:%1; Mem:%2; GPU:%3").arg(cpuInfo).arg(memInfo).arg(gpuInfo);
        return d_ptr->deviceId;
    }

    void IdentityService::init()
    {
        getUserID();
        getDeviceID();
        qInfo() << "IdentityService:init: Init finished in thread:" << QThread::currentThread() << "DeviceID:" << d_ptr->deviceId;
    }

} // namespace event_report
