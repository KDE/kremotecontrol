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

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

#include "modes.h"
#include "mode.h"

Modes::Modes()
{
}


Modes::~Modes()
{
}

void Modes::loadFromConfig(KConfig &theConfig)
{
    KConfigGroup modesGroup = theConfig.group("Modes");
    clear();
    QString helperString = modesGroup.readEntry("Modes", QString());
    int numModes = helperString.toInt();
    for (int i = 0; i < numModes; i++) {
        add(Mode().loadFromConfig(theConfig, i));
    }

    for (iterator i = begin(); i != end(); ++i)
        theDefaults[i.key()] = modesGroup.readEntry("Default" + i.key(), QString());
}

void Modes::generateNulls(const QStringList &theRemotes)
{
    for (QStringList::const_iterator i = theRemotes.begin(); i != theRemotes.end(); ++i) {
        if (!contains(*i) || !operator[](*i).contains("")) operator[](*i)[""] = Mode(*i, "");
        if (!theDefaults.contains(*i)) theDefaults[*i].isEmpty();
    }
}

bool Modes::isDefault(const Mode &mode) const
{
    if (theDefaults[mode.remote()] == mode.name())
        return true;
    if (theDefaults[mode.remote()].isEmpty() || theDefaults[mode.remote()].isNull())
        return mode.name().isEmpty();
    return false;
}

const Mode Modes::getDefault(const QString &remote) const
{
    if (theDefaults[remote] == QString())
        return Mode(remote, "");
    if (contains(remote))
        if (operator[](remote).contains(theDefaults[remote]))
            return operator[](remote)[theDefaults[remote]];
        else return Mode(remote, "");
    else return Mode(remote, "");

}

void Modes::purgeAllModes(KConfig &theConfig)
{
    KConfigGroup modesGroup = theConfig.group("Modes");
    QString helperString = modesGroup.readEntry("Modes", QString());
    int numModes = helperString.toInt();
    for (int i = 0; i < numModes; i++) {
        QString Prefix = "Mode" + QString().setNum(i);
        modesGroup.deleteEntry(Prefix + "Name");
        modesGroup.deleteEntry(Prefix + "Remote");
    }
}

void Modes::saveToConfig(KConfig &theConfig)
{
    KConfigGroup modesGroup = theConfig.group("Modes");
    int index = 0;
    purgeAllModes(theConfig);
    for (iterator i = begin(); i != end(); ++i)
        for (QMap<QString, Mode>::iterator j = (*i).begin(); j != (*i).end(); ++j, index++)
            (*j).saveToConfig(theConfig, index);
    modesGroup.writeEntry("Modes", index);

    for (iterator i = begin(); i != end(); ++i)
        if (theDefaults[i.key()] == QString())
            modesGroup.writeEntry("Default" + i.key(), "");
        else
            modesGroup.writeEntry("Default" + i.key(), theDefaults[i.key()]);
}

const Mode &Modes::getMode(const QString &remote, const QString &mode) const
{
    kDebug() << "found mode: " << operator[](remote)[mode].name() << operator[](remote)[mode].remote();
    return operator[](remote)[mode];
}

ModeList Modes::getModes(const QString &remote) const
{
    ModeList ret;
    for (QMap<QString, Mode>::const_iterator i = operator[](remote).begin(); i != operator[](remote).end(); ++i)
        ret += *i;
    return ret;
}

void Modes::erase(const Mode &mode)
{
    operator[](mode.remote()).remove(mode.name());
    kDebug() << "should erease mode...";
}

void Modes::add(const Mode &mode)
{
    kDebug() << "adding a mode " << mode.name() << " to remote " << mode.remote() ;
    operator[](mode.remote())[mode.name()] = mode;
}

void Modes::rename(Mode &mode, const QString name)
{
    bool was = isDefault(mode);
    erase(mode);
    mode.setName(name);
    if (was) setDefault(mode);
    add(mode);
}

