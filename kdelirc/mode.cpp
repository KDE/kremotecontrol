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

Mode::Mode() : theName(QString::null)	//krazy:exclude=nullstrassign for old broken gcc
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
	QString Prefix = "Mode" + QString().setNum(index);
	theName = theConfig.readEntry(Prefix + "Name",QString());
	theRemote = theConfig.readEntry(Prefix + "Remote",QString());
	theIconFile = theConfig.readEntry(Prefix + "IconFile",QString());
	if(theIconFile.isEmpty()) theIconFile = QString();
	return *this;
}

void Mode::saveToConfig(KConfig &theConfig, int index)
{
	QString Prefix = "Mode" + QString().setNum(index);
	theConfig.writeEntry(Prefix + "Name", theName);
	theConfig.writeEntry(Prefix + "Remote", theRemote);
	theConfig.writeEntry(Prefix + "IconFile", theIconFile);
}

