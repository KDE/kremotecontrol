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
#include <kconfig.h>

#include "iractions.h"
#include "iraction.h"

void IRActions::loadFromConfig(KConfig &theConfig)
{
	clear();
	int numBindings = theConfig.readNumEntry("Bindings");
	for(int i = 0; i < numBindings; i++)
		addAction(IRAction().loadFromConfig(theConfig, i));
}

void IRActions::purgeAllBindings(KConfig &theConfig)
{
	int numBindings = theConfig.readNumEntry("Bindings");
	for(int i = 0; i < numBindings; i++)
	{	QString Binding = "Binding" + QString().setNum(i);
		int numArguments = theConfig.readNumEntry(Binding + "Arguments");
		for(int j = 0; j < numArguments; j++)
		{	theConfig.deleteEntry(Binding + "Argument" + QString().setNum(j));
			theConfig.deleteEntry(Binding + "ArgumentType" + QString().setNum(j));
		}
		theConfig.deleteEntry(Binding + "Arguments"); theConfig.deleteEntry(Binding + "Program");
		theConfig.deleteEntry(Binding + "Object"); theConfig.deleteEntry(Binding + "Method");
		theConfig.deleteEntry(Binding + "Remote"); theConfig.deleteEntry(Binding + "Button");
		theConfig.deleteEntry(Binding + "Repeat");
	}
}

void IRActions::saveToConfig(KConfig &theConfig)
{
	int index = 0;
	purgeAllBindings(theConfig);
	for(IRActions::iterator k = begin(); k != end(); k++)
		for(QValueList<IRAction>::iterator i = k.data().begin(); i != k.data().end(); i++,index++)
			(*i).saveToConfig(theConfig, index);
	theConfig.writeEntry("Bindings", index + 1);
}
