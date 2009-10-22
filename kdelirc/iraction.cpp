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

#include "iraction.h"
#include "profileserver.h"

#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocale.h>

KDE_EXPORT IRAction *IRAction::loadFromConfig(KConfig &theConfig, int index)
{
  
    //Read Config
    QString Binding = "Binding" + QString().setNum(index);
    KConfigGroup actionGroup = theConfig.group("Actions");    
    QString helperString = actionGroup.readEntry(Binding + "Arguments", QString());
    int numArguments = helperString.toInt();
    Arguments tArguments = Arguments();
    
    for (int j = 0; j < numArguments; j++) {
        QVariant::Type theType = QVariant::nameToType(actionGroup.readEntry(Binding + "ArgumentType" + QString().setNum(j), QString().toLocal8Bit()));
	QVariant newArg = actionGroup.readEntry(Binding + "Argument" + QString().setNum(j), QVariant(theType));
        tArguments.append(newArg);
    }

    QString tProgram = actionGroup.readEntry(Binding + "Program", QString());
    QString tObject = actionGroup.readEntry(Binding + "Object", QString());
    QString tPrototype = actionGroup.readEntry(Binding + "Method", QString());
    QString tRemote  = actionGroup.readEntry(Binding + "Remote", QString());
    QString tMode  = actionGroup.readEntry(Binding + "Mode", QString());
    QString tButton =  actionGroup.readEntry(Binding + "Button", QString());
    bool tRepeat = actionGroup.readEntry(Binding + "Repeat", QVariant(QVariant::Bool)).toBool();
    bool tDoBefore = actionGroup.readEntry(Binding + "DoBefore", QVariant(QVariant::Bool)).toBool();
    bool tDoAfter = actionGroup.readEntry(Binding + "DoAfter", QVariant(QVariant::Bool)).toBool();
    bool tAutoStart = actionGroup.readEntry(Binding + "AutoStart", QVariant(QVariant::Bool)).toBool();
    bool tUnique = actionGroup.readEntry(Binding + "Unique", QVariant(QVariant::Bool)).toBool();
    
    IfMulti ifMulti = (IfMulti) actionGroup.readEntry(Binding + "IfMulti", QVariant(QVariant::Int).toInt());
    
    //Fill irAction
    IRAction *action = new IRAction(tRemote,tButton);    
    action->theProgram = tProgram;
    action->theObject  = tObject;
    action->theMethod.setPrototype(tPrototype);
    action->theMode = tMode;
    action->theRepeat = tRepeat;    
    action->theDoBefore = tDoBefore;
    action->theDoAfter=tDoAfter;
    action->theAutoStart = tAutoStart;
    action->theUnique = tUnique;
    action->theIfMulti = ifMulti;    
    action->theArguments = tArguments;
    return action;
}

KDE_EXPORT void IRAction::saveToConfig(KConfig &theConfig, int index) const
{
    KConfigGroup actionGroup = theConfig.group("Actions");

    QString Binding = "Binding" + QString().setNum(index);
    actionGroup.writeEntry(Binding + "Arguments", theArguments.count());

    for (int j = 0; j < theArguments.count(); j++) {
        QVariant arg = theArguments.at(j);
        QVariant::Type preType = arg.type();
        actionGroup.writeEntry(Binding + "Argument" + QString().setNum(j), arg);
        actionGroup.writeEntry(Binding + "ArgumentType" + QString().setNum(j), QVariant::typeToName(preType));
        kDebug() << "writeEntryType is" << QVariant::typeToName(preType) << "argument" << arg;
    }
    actionGroup.writeEntry(Binding + "Program", theProgram);
    actionGroup.writeEntry(Binding + "Object", theObject);
    actionGroup.writeEntry(Binding + "Method", theMethod.prototype());
    actionGroup.writeEntry(Binding + "Remote", theRemoteButton.remoteName());
    actionGroup.writeEntry(Binding + "Mode", theMode);
    actionGroup.writeEntry(Binding + "Button", theRemoteButton.name());
    actionGroup.writeEntry(Binding + "Repeat", theRepeat);
    actionGroup.writeEntry(Binding + "DoBefore", theDoBefore);
    actionGroup.writeEntry(Binding + "DoAfter", theDoAfter);
    actionGroup.writeEntry(Binding + "AutoStart", theAutoStart);
    actionGroup.writeEntry(Binding + "Unique", theUnique);
    actionGroup.writeEntry(Binding + "IfMulti", int(theIfMulti));
}

KDE_EXPORT const QString IRAction::function() const
{
    ProfileServer *theServer = ProfileServer::getInstance();
    if (theProgram.isEmpty())
        if (theObject.isEmpty())
            return i18n("Exit mode");
        else
            return i18n("Switch to %1", theObject);
    else
        if (theObject.isEmpty())
            return i18n("Just start");
        else {
            const ProfileAction *a = theServer->getAction(theProgram, theObject, theMethod.prototype());
            if (a)
                return a->name();
            else
                return theObject + "::" + theMethod.name();
        }
}


KDE_EXPORT const QString IRAction::application() const
{
    ProfileServer *theServer = ProfileServer::getInstance();
    if (theProgram.isEmpty())
        return QString();
    else {
	const Profile *a = theServer->getProfileById(theProgram);
        if (a)
            return a->name();
        else
            return theProgram;
    }
}
