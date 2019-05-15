/*
 * Copyright (C) 2015 Dominik Haumann <dhaumann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "LliurexDiskQuota.h"
#include "LliurexQuotaItem.h"
#include "LliurexQuotaListModel.h"

#include <KLocalizedString>
#include <KFormat>

#include <QTimer>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDebug>

LliurexDiskQuota::LliurexDiskQuota(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_quotaProcess(new QProcess(this))
    , m_model(new LliurexQuotaListModel(this))
{
    connect(m_timer, &QTimer::timeout, this, &LliurexDiskQuota::updateQuota);
    m_timer->start(1 * 60 * 1000); // check every 2 minutes

    connect(m_quotaProcess, (void (QProcess::*)(int, QProcess::ExitStatus))&QProcess::finished,
            this, &LliurexDiskQuota::quotaProcessFinished);
    updateQuota();
}

bool LliurexDiskQuota::quotaInstalled() const
{
    return m_quotaInstalled;
}

void LliurexDiskQuota::setQuotaInstalled(bool installed)
{
    if (m_quotaInstalled != installed) {
        m_quotaInstalled = installed;

        if (!installed) {
            m_model->clear();
            setStatus(PassiveStatus);
            setToolTip(i18n("Lliurex Disk Quota"));
            setSubToolTip(i18n("Please install 'lliurex-quota'"));
        }

        emit quotaInstalledChanged();
    }
}

bool LliurexDiskQuota::cleanUpToolInstalled() const
{
    return m_cleanUpToolInstalled;
}

void LliurexDiskQuota::setCleanUpToolInstalled(bool installed)
{
    if (m_cleanUpToolInstalled != installed) {
        m_cleanUpToolInstalled = installed;
        emit cleanUpToolInstalledChanged();
    }
}

LliurexDiskQuota::TrayStatus LliurexDiskQuota::status() const
{
    return m_status;
}

void LliurexDiskQuota::setStatus(LliurexDiskQuota::TrayStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QString LliurexDiskQuota::iconName() const
{
    return m_iconName;
}

void LliurexDiskQuota::setIconName(const QString &name)
{
    if (m_iconName != name) {
        m_iconName = name;
        emit iconNameChanged();
    }
}

QString LliurexDiskQuota::toolTip() const
{
    return m_toolTip;
}

void LliurexDiskQuota::setToolTip(const QString &toolTip)
{
    if (m_toolTip != toolTip) {
        m_toolTip = toolTip;
        emit toolTipChanged();
    }
}

QString LliurexDiskQuota::subToolTip() const
{
    return m_subToolTip;
}

void LliurexDiskQuota::setSubToolTip(const QString &subToolTip)
{
    if (m_subToolTip != subToolTip) {
        m_subToolTip = subToolTip;
        emit subToolTipChanged();
    }
}

static QString iconNameForQuota(int quota)
{
    if (quota < 50) {
        return QStringLiteral("lliurexquota");
    } else if (quota < 75) {
        return QStringLiteral("lliurexquota-low");
    } else if (quota < 90) {
        return QStringLiteral("lliurexquota-high");
    }

    // quota >= 90%
    return QStringLiteral("lliurexquota-critical");
}

static bool isQuotaLine(const QString &line)
{
    QStringList parts = line.split(QLatin1Char(','), QString::SkipEmptyParts);
    if (parts.size() == 4){
        if (parts[0] == "True"){
            return true;
        }
        return false;
    }
    return false;
}

void LliurexDiskQuota::updateQuota()
{
    const bool quotaFound = ! QStandardPaths::findExecutable(QStringLiteral("lliurex-quota")).isEmpty();
    setQuotaInstalled(quotaFound);
    if (!quotaFound) {
        return;
    }

    // for now, only filelight is supported
    //setCleanUpToolInstalled(! QStandardPaths::findExecutable(QStringLiteral("filelight")).isEmpty());
    setCleanUpToolInstalled(false);

    // kill running process in case it hanged for whatever reason
    if (m_quotaProcess->state() != QProcess::NotRunning) {
        m_quotaProcess->kill();
    }

    // Try to run 'quota'
    const QStringList args{
        QStringLiteral("-mq"),
    };
    //qDebug() << "Initiating lliurex-quota";
    m_quotaProcess->start(QStringLiteral("lliurex-quota"), args, QIODevice::ReadOnly);
}

void LliurexDiskQuota::quotaProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    //qDebug() << "Ending process lliurex-quota";
    if (exitStatus != QProcess::NormalExit) {
        //qDebug() << "Wrong EXITCODE" ;
        m_model->clear();
        setToolTip(i18n("Lliurex Disk Quota"));
        setSubToolTip(i18n("Running lliurex-quota failed"));
        return;
    }

    // get quota output
    const QString rawData = QString::fromLocal8Bit(m_quotaProcess->readAllStandardOutput());
    //qDebug() << "Result from lliurex-quota cmd:" << rawData;

    const QStringList lines = rawData.split(QRegularExpression(QStringLiteral("[\r\n]")), QString::SkipEmptyParts);
    // Testing
//     QStringList lines = QStringList()
//         << QStringLiteral("/home/peterpan 3975379*  5000000 7000000           57602 0       0")
//         << QStringLiteral("/home/archive 2263536  6000000 5100000            3932 0       0")
//         << QStringLiteral("/home/shared 4271196*  10000000 7000000           57602 0       0");
//         << QStringLiteral("/home/peterpan %1*  5000000 7000000           57602 0       0").arg(qrand() % 5000000)
//         << QStringLiteral("/home/archive %1  5000000 5100000            3932 0       0").arg(qrand() % 5000000)
//         << QStringLiteral("/home/shared %1*  5000000 7000000           57602 0       0").arg(qrand() % 5000000);
//     lines.removeAt(qrand() % lines.size());

    // format class needed for GiB/MiB/KiB formatting
    KFormat fmt;
    int maxQuota = 0;
    QVector<LliurexQuotaItem> items;

    // assumption: Filesystem starts with slash
    for (const QString &line : lines) {
        //qDebug() << "Procesing line + isQuotaLine" << line << " + " << isQuotaLine(line);
        if (!isQuotaLine(line)) {
            continue;
        }

        QStringList parts = line.split(QLatin1Char(','), QString::SkipEmptyParts);
        // True,lliurex,182,0 // parts: 0,1,2,3

        // 'quota' uses kilo bytes -> factor 1024
        // NOTE: int is not large enough, hence qint64

        const qint64 hardLimit = parts[3].toLongLong() * 1024;
        const qint64 used = parts[2].toLongLong() * 1024;
        const qint64 freeSize = hardLimit - used;
        const int percent = qMin(100, qMax(0, qRound(used * 100.0 / hardLimit)));

        LliurexQuotaItem item;
        item.setIconName(iconNameForQuota(percent));
        item.setMountPoint(i18n("Assigned space"));
        item.setUsage(percent);
        item.setMountString(i18nc("usage of quota, e.g.: '/home/bla: 38\% used'", "%1: %2% used", i18n("Assigned space"), percent));
        item.setUsedString(i18nc("e.g.: 12 GiB of 20 GiB", "%1 of %2", fmt.formatByteSize(used), fmt.formatByteSize(hardLimit)));
        item.setFreeString(i18nc("e.g.: 8 GiB free", "%1 free", fmt.formatByteSize(qMax(qint64(0), freeSize))));

        items.append(item);

        maxQuota = qMax(maxQuota, percent);
    }

//     qDebug() << "QUOTAS:" << quotas;

    // make sure max quota is 100. Could be more, due to the
    // hard limit > soft limit, and we take soft limit as 100%
    maxQuota = qMin(100, maxQuota);

    // update icon in panel
    setIconName(iconNameForQuota(maxQuota));

    if (maxQuota < 50){
        //qDebug() << "Setting status passive";
        setStatus(PassiveStatus);
    }else if (maxQuota < 90){
        //qDebug() << "Setting status active";
        setStatus(ActiveStatus);
    }else{
        //qDebug() << "Setting status need attention";
        setStatus(NeedsAttentionStatus);
    }
    // update status
    //setStatus(maxQuota < 50 ? PassiveStatus
    //        : maxQuota < 98 ? ActiveStatus
    //        : NeedsAttentionStatus);

    if (!items.isEmpty()) {
        setToolTip(i18nc("example: Quota: 83% used",
                         "Quota: %1% used", maxQuota));
        setSubToolTip(QString());
    } else {
        setToolTip(i18n("Disk Quota"));
        setSubToolTip(i18n("No quota restrictions found."));
    }

    // merge new items, add new ones, remove old ones
    m_model->updateItems(items);
}

LliurexQuotaListModel *LliurexDiskQuota::model() const
{
    return m_model;
}

void LliurexDiskQuota::openCleanUpTool(const QString &mountPoint)
{
    Q_UNUSED(mountPoint);
    if (!cleanUpToolInstalled()) {
        return;
    }

    //QProcess::startDetached(QStringLiteral("filelight"), {mountPoint});
}
