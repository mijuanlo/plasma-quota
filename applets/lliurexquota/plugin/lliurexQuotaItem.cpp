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
#include "lliurexQuotaItem.h"

#include <QDebug>

lliurexQuotaItem::lliurexQuotaItem()
    : m_iconName(QStringLiteral("lliurexquota"))
    , m_mountPoint()
    , m_usage(0)
    , m_mountString()
    , m_usedString()
{
}

QString lliurexQuotaItem::iconName() const
{
    return m_iconName;
}

void lliurexQuotaItem::setIconName(const QString &name)
{
    m_iconName = name;
}

QString lliurexQuotaItem::mountPoint() const
{
    return m_mountPoint;
}

void lliurexQuotaItem::setMountPoint(const QString &mountPoint)
{
    m_mountPoint = mountPoint;
}

int lliurexQuotaItem::usage() const
{
    return m_usage;
}

void lliurexQuotaItem::setUsage(int usage)
{
    m_usage = usage;
}

QString lliurexQuotaItem::mountString() const
{
    return m_mountString;
}

void lliurexQuotaItem::setMountString(const QString &mountString)
{
    m_mountString = mountString;
}

QString lliurexQuotaItem::usedString() const
{
    return m_usedString;
}

void lliurexQuotaItem::setUsedString(const QString &usedString)
{
    m_usedString = usedString;
}

QString lliurexQuotaItem::freeString() const
{
    return m_freeString;
}

void lliurexQuotaItem::setFreeString(const QString &freeString)
{
    m_freeString = freeString;
}

bool lliurexQuotaItem::operator==(const lliurexQuotaItem &other) const
{
    return m_mountPoint == other.m_mountPoint
        && m_iconName == other.m_iconName
        && m_usage == other.m_usage
        && m_mountString == other.m_mountString
        && m_usedString == other.m_usedString
        && m_freeString == other.m_freeString;
}

bool lliurexQuotaItem::operator!=(const lliurexQuotaItem &other) const
{
    return ! (*this == other);
}
