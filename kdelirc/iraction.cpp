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

#include <qvariant.h>

#include <kconfig.h>
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

const IRAction &IRAction::loadFromConfig(KConfig &theConfig, int index)
{
	QString Binding = "Binding" + QString().setNum(index);
	int numArguments = theConfig.readNumEntry(Binding + "Arguments");
	theArguments.clear();
	for(int j = 0; j < numArguments; j++)
	{	QVariant::Type theType = (QVariant::Type)theConfig.readNumEntry(Binding + "ArgumentType" + QString().setNum(j), QVariant::String);
		theArguments += theConfig.readPropertyEntry(Binding + "Argument" + QString().setNum(j), theType == QVariant::CString ? QVariant::String : theType);
		theArguments.last().cast(theType);
	}

	theProgram = theConfig.readEntry(Binding + "Program");
	theObject = theConfig.readEntry(Binding + "Object");
	theMethod.setPrototype(theConfig.readEntry(Binding + "Method"));
	theRemote = theConfig.readEntry(Binding + "Remote");
	theMode = theConfig.readEntry(Binding + "Mode");
	theButton = theConfig.readEntry(Binding + "Button");
	theRepeat = theConfig.readBoolEntry(Binding + "Repeat");
	theDoBefore = theConfig.readBoolEntry(Binding + "DoBefore");
	theDoAfter = theConfig.readBoolEntry(Binding + "DoAfter");
	theAutoStart = theConfig.readBoolEntry(Binding + "AutoStart");
	theUnique = theConfig.readBoolEntry(Binding + "Unique", true);
	theIfMulti = (IfMulti)theConfig.readNumEntry(Binding + "IfMulti", IM_DONTSEND);

	return *this;
}

void IRAction::saveToConfig(KConfig &theConfig, int index) const
{
	QString Binding = "Binding" + QString().setNum(index);

	theConfig.writeEntry(Binding + "Arguments", theArguments.count());
	for(unsigned j = 0; j < theArguments.count(); j++)
	{	QVariant arg = theArguments[j];
		QVariant::Type preType = arg.type();
		if(preType == QVariant::CString) arg.cast(QVariant::String);
		theConfig.writeEntry(Binding + "Argument" + QString().setNum(j), arg);
		theConfig.writeEntry(Binding + "ArgumentType" + QString().setNum(j), preType);
	}
	theConfig.writeEntry(Binding + "Program", theProgram);
	theConfig.writeEntry(Binding + "Object", theObject);
	theConfig.writeEntry(Binding + "Method", theMethod.prototype());
	theConfig.writeEntry(Binding + "Remote", theRemote);
	theConfig.writeEntry(Binding + "Mode", theMode);
	theConfig.writeEntry(Binding + "Button", theButton);
	theConfig.writeEntry(Binding + "Repeat", theRepeat);
	theConfig.writeEntry(Binding + "DoBefore", theDoBefore);
	theConfig.writeEntry(Binding + "DoAfter", theDoAfter);
	theConfig.writeEntry(Binding + "AutoStart", theAutoStart);
	theConfig.writeEntry(Binding + "Unique", theUnique);
	theConfig.writeEntry(Binding + "IfMulti", theIfMulti);
}

const QString IRAction::function() const
{
	ProfileServer *theServer = ProfileServer::profileServer();
	if(theProgram.isEmpty())
		if(theObject.isEmpty())
			return i18n("Exit mode");
		else
			return i18n("Switch to %1").arg(theObject);
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

