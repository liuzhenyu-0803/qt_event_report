#include "identity_service.h"
#include <QSettings>
#include <QUuid>
#include <QProcess>
#include <QRegExp>
#include <QSysInfo>
#include <QDebug>
#include <QMutexLocker>
#include <QThread>
#include "event_report_constants.h"

IdentityService::IdentityService(QObject* parent) : QObject(parent)
{
}

IdentityService::~IdentityService()
{
}

QString IdentityService::getUserID()
{
    QMutexLocker locker(&m_mutex);
    if (!m_userId.isEmpty()) return m_userId;

    QSettings settings(EventReport::REGISTRY_PATH, QSettings::NativeFormat);
    m_userId = settings.value(EventReport::REG_KEY_USER_ID).toString();
    if (m_userId.isEmpty())
    {
        m_userId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        settings.setValue(EventReport::REG_KEY_USER_ID, m_userId);
    }
    return m_userId;
}

QString IdentityService::getDeviceID()
{
    QMutexLocker locker(&m_mutex);
    if (!m_deviceId.isEmpty()) return m_deviceId;

    QString cpuInfo = "Unknown CPU";
    QString memInfo = "Unknown Mem";
    QString gpuInfo = "Unknown GPU";

    QProcess process;
    process.start("wmic", QStringList() << "cpu" << "get" << "name");
    if (process.waitForFinished())
    {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (lines.size() > 1) cpuInfo = lines[1].trimmed();
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
            if (ok) memInfo = QString("%1GB").arg(qRound(bytes / (1024.0 * 1024.0 * 1024.0)));
        }
    }

    process.start("wmic", QStringList() << "path" << "win32_VideoController" << "get" << "name");
    if (process.waitForFinished())
    {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        QStringList lines = output.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        if (lines.size() > 1) gpuInfo = lines[1].trimmed();
    }

    m_deviceId = QString("CPU:%1; Mem:%2; GPU:%3").arg(cpuInfo).arg(memInfo).arg(gpuInfo);
    return m_deviceId;
}


void IdentityService::init()
{
    getUserID();
    getDeviceID();
    qInfo() << "IdentityService:init: Init finished in thread:" << QThread::currentThread() << "DeviceID:" << m_deviceId;
}
