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

#include "iractions.h"
#include "iraction.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <kdebug.h>

void IRActions::loadFromConfig(KConfig &theConfig)
{
    clear();
    KConfigGroup actionGroup = theConfig.group("Actions");
    QString helperString = actionGroup.readEntry("Bindings", QString());
    int numBindings = helperString.toInt();

    for (int i = 0; i < numBindings; i++) {
        addAction(IRAction::loadFromConfig(theConfig, i));
    }
}

void IRActions::purgeAllBindings(KConfig &theConfig)
{
    KConfigGroup actionGroup = theConfig.group("Actions");
    QString helperString = actionGroup.readEntry("Bindings", QString());
    int numBindings = helperString.toInt();
    for (int i = 0; i < numBindings; i++) {
        QString Binding = "Binding" + QString().setNum(i);
        helperString = actionGroup.readEntry(Binding + "Arguments", QString());
        int numArguments = helperString.toInt();
        for (int j = 0; j < numArguments; j++) {
            actionGroup.deleteEntry(Binding + "Argument" + QString().setNum(j));
            actionGroup.deleteEntry(Binding + "ArgumentType" + QString().setNum(j));
        }
        actionGroup.deleteEntry(Binding + "Arguments"); actionGroup.deleteEntry(Binding + "Program");
        actionGroup.deleteEntry(Binding + "Object"); actionGroup.deleteEntry(Binding + "Method");
        actionGroup.deleteEntry(Binding + "Remote"); actionGroup.deleteEntry(Binding + "Button");
        actionGroup.deleteEntry(Binding + "Repeat"); actionGroup.deleteEntry(Binding + "Mode");
    }
}

void IRActions::saveToConfig(KConfig &theConfig)
{
    KConfigGroup actionGroup = theConfig.group("Actions");
    int index = 0;
    purgeAllBindings(theConfig);
    for (iterator i = begin(); i != end(); ++i, index++)
        (*i)->saveToConfig(theConfig, index);
    actionGroup.writeEntry("Bindings", index);
}

void IRActions::addAction(IRAction *theAction)
{
    append(theAction);
}

IRActions IRActions::findByButton(const QString &remote, const QString &button)
{
    IRActions ret;
    for (iterator i = begin(); i != end(); ++i)
        if ((*i)->remote() == remote && (*i)->button() == button)
            ret += *i;
    return ret;
}

void IRActions::renameMode(const Mode &mode, const QString &to)
{
    for (iterator i = begin(); i != end(); ++i) {
        if ((*i)->remote() == mode.remote() && (*i)->mode() == mode.name())(*i)->setMode(to);
        if ((*i)->isModeChange() && (*i)->modeChange() == mode.name())(*i)->setModeChange(to);
    }
}

IRActions IRActions::findByMode(const Mode &mode)
{
    IRActions ret;
    kDebug() << "IRActions size: " << size();
    for (iterator i = begin(); i != end(); ++i) {
        kDebug() << "Searching action: " << (*i)->remote() << (*i)->button();
        if ((*i)->remote() == mode.remote() && (*i)->mode() == mode.name()) {
            kDebug() << "Action " << (*i)->remote() << (*i)->button() << "matches";
            ret += *i;
        }
    }
    return ret;
}

IRActions IRActions::findByModeButton(const Mode &mode, const QString &button)
{
    IRActions ret;
    for (iterator i = begin(); i != end(); ++i)
        if ((*i)->remote() == mode.remote() && (*i)->mode() == mode.name() && (*i)->button() == button)
            ret.append(*i);
    return ret;
}

void IRActions::erase(IRAction *action)
{
    for (int i = 0; i < size(); i++) {
        if (operator[](i) == action) {
            takeAt(i);
        }
    }
}
