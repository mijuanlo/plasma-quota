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
#include "plugin.h"
#include "lliurexDiskQuota.h"
#include "lliurexQuotaListModel.h"

#include <QtQml>

void lliurexDiskQuotaPlugin::registerTypes (const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.private.lliurexquota"));
    qmlRegisterType<lliurexDiskQuota>(uri, 1, 0, "lliurexDiskQuota");
    qmlRegisterType<lliurexQuotaListModel>(uri, 1, 0, "lliurexQuotaListModel");
}
