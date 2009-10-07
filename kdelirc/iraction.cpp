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
#include "remoteserver.h"

#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocale.h>

KDE_EXPORT IRAction::IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newMode, const QString &newButton, const bool newRepeat, const bool newAutoStart, const bool newDoBefore, const bool newDoAfter, const bool newUnique, const IfMulti newIfMulti)
{
    theProgram = newProgram;
    theObject = newObject;
    theMethod = newMethod;
    theArguments = newArguments;
    theRemote = newRemote;
    theMode = newMode;
    theButton = newButton;
    theRepeat = newRepeat;
    theDoAfter = newDoAfter;
    theDoBefore = newDoBefore;
    theAutoStart = newAutoStart;
    theUnique = newUnique;
    theIfMulti = newIfMulti;
}

KDE_EXPORT IRAction *IRAction::loadFromConfig(KConfig &theConfig, int index)
{
    IRAction *action = new IRAction();
    KConfigGroup actionGroup = theConfig.group("Actions");
    QString Binding = "Binding" + QString().setNum(index);
    QString helperString = actionGroup.readEntry(Binding + "Arguments", QString());
    int numArguments = helperString.toInt();
    action->theArguments.clear();
    for (int j = 0; j < numArguments; j++) {
        QVariant::Type theType = QVariant::nameToType(actionGroup.readEntry(Binding + "ArgumentType" + QString().setNum(j), QString().toLocal8Bit()));
	QVariant newArg = actionGroup.readEntry(Binding + "Argument" + QString().setNum(j), QVariant(theType));
        action->theArguments.append(newArg);
    }

    kDebug() << "Arguments: " << action->theArguments.getArgumentsList();

    action->theProgram = actionGroup.readEntry(Binding + "Program", QString());
    action->theObject = actionGroup.readEntry(Binding + "Object", QString());
    action->theMethod.setPrototype(actionGroup.readEntry(Binding + "Method", QString()));
    action->theRemote = actionGroup.readEntry(Binding + "Remote", QString());
    action->theMode = actionGroup.readEntry(Binding + "Mode", QString());
    action->theButton = actionGroup.readEntry(Binding + "Button", QString());
    action->theRepeat = actionGroup.readEntry(Binding + "Repeat", QVariant(QVariant::Bool)).toBool();
    action->theDoBefore = actionGroup.readEntry(Binding + "DoBefore", QVariant(QVariant::Bool)).toBool();
    action->theDoAfter = actionGroup.readEntry(Binding + "DoAfter", QVariant(QVariant::Bool)).toBool();
    action->theAutoStart = actionGroup.readEntry(Binding + "AutoStart", QVariant(QVariant::Bool)).toBool();
    action->theUnique = actionGroup.readEntry(Binding + "Unique", QVariant(QVariant::Bool)).toBool();
    helperString = actionGroup.readEntry(Binding + "IfMulti", QString());
    action->theIfMulti = (IfMulti)helperString.toInt();

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
        /*  if(preType == QVariant::StringList && arg.toString().isNull()){
           arg = "";
          }*/
        actionGroup.writeEntry(Binding + "Argument" + QString().setNum(j), arg);
        actionGroup.writeEntry(Binding + "ArgumentType" + QString().setNum(j), QVariant::typeToName(preType));
        kDebug() << "writeEntryType is" << QVariant::typeToName(preType) << "argument" << arg;
    }
    actionGroup.writeEntry(Binding + "Program", theProgram);
    actionGroup.writeEntry(Binding + "Object", theObject);
    actionGroup.writeEntry(Binding + "Method", theMethod.prototype());
    actionGroup.writeEntry(Binding + "Remote", theRemote);
    actionGroup.writeEntry(Binding + "Mode", theMode);
    actionGroup.writeEntry(Binding + "Button", theButton);
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

KDE_EXPORT const QString IRAction::notes() const
{

    if (isModeChange())
        return QString(theDoBefore ? i18n("Do actions before. ") : "") +
               QString(theDoAfter ? i18n("Do actions after. ") : "");
    else if (isJustStart())
        return "";
    else
        return QString(theAutoStart ? i18n("Auto-start. ") : "")
               + QString(theRepeat ? i18n("Repeatable. ") : "")
               + QString(!theUnique ? (theIfMulti == IM_DONTSEND ? i18n("Do nothing if many instances. ")
                                       : theIfMulti == IM_SENDTOTOP ? i18n("Send to top instance. ")
                                       : theIfMulti == IM_SENDTOBOTTOM ? i18n("Send to bottom instance. ") : i18n("Send to all instances. "))
                                 : "");
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

const QString IRAction::remoteName() const
{
    return RemoteServer::remoteServer()->getRemoteName(theRemote);
}

KDE_EXPORT const QString IRAction::buttonName() const
{
    return RemoteServer::remoteServer()->getButtonName(theRemote, theButton);
}

