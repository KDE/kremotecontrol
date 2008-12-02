//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>
#include <klocale.h>

#include "iraction.h"
#include "profileserver.h"
#include "remoteserver.h"

IRAction::IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newMode, const QString &newButton, const bool newRepeat, const bool newAutoStart, const bool newDoBefore, const bool newDoAfter, const bool newUnique, const IfMulti newIfMulti)
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

IRAction *IRAction::loadFromConfig(KConfig &theConfig, int index)
{
	IRAction *action = new IRAction();
	KConfigGroup actionGroup = theConfig.group("Actions");
	QString Binding = "Binding" + QString().setNum(index);
	QString helperString= actionGroup.readEntry(Binding + "Arguments", QString());
	int numArguments = helperString.toInt();
	action->theArguments.clear();
	for(int j = 0; j < numArguments; j++)
	{
		QVariant::Type theType = QVariant::nameToType(actionGroup.readEntry(Binding + "ArgumentType" + QString().setNum(j), QString("").toLocal8Bit()));
		kDebug() << "Readentry type is:" << actionGroup.readEntry(Binding + "ArgumentType" + QString().setNum(j), QString().toLocal8Bit());
//		theArguments += actionGroup.readEntry(Binding + "Argument" + QString().setNum(j), theType == QVariant::CString ? QVariant::String : theType);
		action->theArguments += actionGroup.readEntry(Binding + "Argument" + QString().setNum(j), QString());
		action->theArguments.last().convert(theType);
	}

	kDebug() << "Arguments: " << action->theArguments;

	action->theProgram = actionGroup.readEntry(Binding + "Program",QString());
	action->theObject = actionGroup.readEntry(Binding + "Object",QString());
	action->theMethod.setPrototype(actionGroup.readEntry(Binding + "Method",QString()));
	action->theRemote = actionGroup.readEntry(Binding + "Remote",QString());
	action->theMode = actionGroup.readEntry(Binding + "Mode",QString());
	action->theButton = actionGroup.readEntry(Binding + "Button",QString());
	action->theRepeat = actionGroup.readEntry(Binding + "Repeat", QVariant(QVariant::Bool)).toBool();
	action->theDoBefore = actionGroup.readEntry(Binding + "DoBefore", QVariant(QVariant::Bool)).toBool();
	action->theDoAfter = actionGroup.readEntry(Binding + "DoAfter", QVariant(QVariant::Bool)).toBool();
	action->theAutoStart = actionGroup.readEntry(Binding + "AutoStart", QVariant(QVariant::Bool)).toBool();
	action->theUnique = actionGroup.readEntry(Binding + "Unique", QVariant(QVariant::Bool)).toBool();
	helperString = actionGroup.readEntry(Binding + "IfMulti", QString());
	action->theIfMulti = (IfMulti)helperString.toInt();

	return action;
}

void IRAction::saveToConfig(KConfig &theConfig, int index) const
{
	KConfigGroup actionGroup = theConfig.group("Actions");

	QString Binding = "Binding" + QString().setNum(index);
	actionGroup.writeEntry(Binding + "Arguments", theArguments.count());

	for(int j = 0; j < theArguments.count(); j++)
	{
		QVariant arg = theArguments[j];
 		QVariant::Type preType = arg.type();
		if(preType == QVariant::StringList && arg.toStringList().isEmpty()){
			arg = "";
		}
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

const QString IRAction::function() const
{
	ProfileServer *theServer = ProfileServer::profileServer();
	if(theProgram.isEmpty())
		if(theObject.isEmpty())
			return i18n("Exit mode");
		else
			return i18n("Switch to %1", theObject);
	else
		if(theObject.isEmpty())
			return i18n("Just start");
		else
		{
			const ProfileAction *a = theServer->getAction(theProgram, theObject, theMethod.prototype());
			if(a)
				return a->name();
			else
				return theObject + "::" + theMethod.name();
		}
}

const QString IRAction::notes() const
{

	if(isModeChange())
		return QString(theDoBefore ? i18n("Do actions before. ") : "") +
			QString(theDoAfter ? i18n("Do actions after. ") : "");
	else if(isJustStart())
		return "";
	else
		return QString(theAutoStart ? i18n("Auto-start. ") : "")
			+ QString(theRepeat ? i18n("Repeatable. ") : "")
			+ QString(!theUnique ? (theIfMulti == IM_DONTSEND ? i18n("Do nothing if many instances. ")
						: theIfMulti == IM_SENDTOTOP ? i18n("Send to top instance. ")
						: theIfMulti == IM_SENDTOBOTTOM ? i18n("Send to bottom instance. ") : i18n("Send to all instances. "))
						: "");
}

const QString IRAction::application() const
{
	ProfileServer *theServer = ProfileServer::profileServer();
	if(theProgram.isEmpty())
		return "";
	else
	{
		const Profile *a = theServer->profiles()[theProgram];
		if(a)
			return a->name();
		else
			return theProgram;
	}
}

const QString IRAction::remoteName() const
{
	return RemoteServer::remoteServer()->getRemoteName(theRemote);
}

const QString IRAction::buttonName() const
{
	return RemoteServer::remoteServer()->getButtonName(theRemote, theButton);
}

