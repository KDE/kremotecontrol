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
	clear();
	int numModes = theConfig.readNumEntry("Modes");
	for(int i = 0; i < numModes; i++)
		add(Mode().loadFromConfig(theConfig, i));

	for(iterator i = begin(); i != end(); i++)
		theDefaults[i.key()] = theConfig.readEntry("Default" + i.key());
}

bool Modes::isDefault(const Mode &mode) const
{
	if(theDefaults[mode.remote()] == mode.name())
		return true;
	if(theDefaults[mode.remote()] == "" || theDefaults[mode.remote()] == QString::null)
		return mode.name() == "";
	return false;
}

const Mode Modes::getDefault(const QString &remote) const
{
	if(theDefaults[remote] == QString())
		return Mode(remote, "");
	if(contains(remote))
		if(operator[](remote).contains(theDefaults[remote]))
			return operator[](remote)[theDefaults[remote]];
		else return Mode(remote, "");
	else return Mode(remote, "");

}

void Modes::purgeAllModes(KConfig &theConfig)
{
	int numModes = theConfig.readNumEntry("Modes");
	for(int i = 0; i < numModes; i++)
	{	QString Prefix = "Mode" + QString().setNum(i);
		theConfig.deleteEntry(Prefix + "Name");
		theConfig.deleteEntry(Prefix + "Remote");
	}
}

void Modes::saveToConfig(KConfig &theConfig)
{
	int index = 0;
	purgeAllModes(theConfig);
	for(iterator i = begin(); i != end(); i++)
		for(QMap<QString, Mode>::iterator j = (*i).begin(); j != (*i).end(); j++,index++)
			(*j).saveToConfig(theConfig, index);
	theConfig.writeEntry("Modes", index);

	for(iterator i = begin(); i != end(); i++)
		if(theDefaults[i.key()] == QString())
			theConfig.writeEntry("Default" + i.key(), "");
		else
			theConfig.writeEntry("Default" + i.key(), theDefaults[i.key()]);
}

const Mode &Modes::getMode(const QString &remote, const QString &mode) const
{
	return operator[](remote)[mode];
}

ModeList Modes::getModes(const QString &remote) const
{
	ModeList ret;
	for(QMap<QString, Mode>::const_iterator i = operator[](remote).begin(); i != operator[](remote).end(); i++)
		ret += *i;
	return ret;
}

void Modes::erase(const Mode &mode)
{
	operator[](mode.remote()).erase(mode.name());
}

void Modes::add(const Mode &mode)
{
	operator[](mode.remote())[mode.name()] = mode;
}

void Modes::rename(Mode &mode, const QString name)
{
	erase(mode);
	mode.setName(name);
	add(mode);
}

