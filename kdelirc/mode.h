//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MODE_H
#define MODE_H

#include <qstring.h>

#include "remoteserver.h"

/**
@author Gav Wood
*/

class KConfig;

class Mode
{
	QString theName, theRemote;

public:
	void setName(const QString &newName) { theName = newName; }
	void setRemote(const QString &newRemote) { theRemote = newRemote; }

	const QString &name() const { return theName; }
	const QString &remote() const { return theRemote; }
	const QString &remoteName() const { return RemoteServer::remoteServer()->getRemoteName(theRemote); }

	const Mode &loadFromConfig(KConfig &theConfig, int index);
	void saveToConfig(KConfig &theConfig, int index);

	Mode();
	Mode(const QString &remote, const QString &name);
	~Mode();
};

#endif
