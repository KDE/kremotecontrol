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

#include "iraction.h"

IRAction::IRAction(const QString &newProgram, const QString &newObject, const QString &newMethod, const Arguments &newArguments, const QString &newRemote, const QString &newButton, bool newRepeat)
{
	theProgram = newProgram;
	theObject = newObject;
	theMethod = newMethod;
	theArguments = newArguments;
	theRemote = newRemote;
	theButton = newButton;
	theRepeat = newRepeat;
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
	theButton = theConfig.readEntry(Binding + "Button");
	theRepeat = theConfig.readBoolEntry(Binding + "Repeat");

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
	theConfig.writeEntry(Binding + "Button", theButton);
	theConfig.writeEntry(Binding + "Repeat", theRepeat);
}
