/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood
  */

#include "mode.h"
#include "modes.h"

#include <kconfig.h>
#include <kconfiggroup.h>

Mode::Mode() : theName(QString::null) //krazy:exclude=nullstrassign for old broken gcc
{
}

Mode::Mode(const QString &remote, const QString &name, const QString &iconFile)
{
    theRemote = remote;
    theName = name;
    theIconFile = iconFile;
}

Mode::~Mode()
{
}

const Mode &Mode::loadFromConfig(KConfig &theConfig, int index)
{
    KConfigGroup modesGroup = theConfig.group("Modes");
    QString Prefix = "Mode" + QString().setNum(index);
    theName = modesGroup.readEntry(Prefix + "Name", QString());
    theRemote = modesGroup.readEntry(Prefix + "Remote", QString());
    theIconFile = modesGroup.readEntry(Prefix + "IconFile", QString());
    if (theIconFile.isEmpty()) theIconFile = QString();
    return *this;
}

void Mode::saveToConfig(KConfig &theConfig, int index)
{
    KConfigGroup modesGroup = theConfig.group("Modes");
    QString Prefix = "Mode" + QString().setNum(index);
    modesGroup.writeEntry(Prefix + "Name", theName);
    modesGroup.writeEntry(Prefix + "Remote", theRemote);
    modesGroup.writeEntry(Prefix + "IconFile", theIconFile);
}

