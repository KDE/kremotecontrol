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
	QString theName, theRemote, theIconFile;

public:
	void setName(const QString &a) { theName = a; }
	void setRemote(const QString &a) { theRemote = a; }
	void setIconFile(const QString &a) { theIconFile = a; }

	const QString &name() const { return theName; }
	const QString &remote() const { return theRemote; }
	const QString &iconFile() const { return theIconFile; }
	const QString &remoteName() const { return RemoteServer::remoteServer()->getRemoteName(theRemote); }

	const Mode &loadFromConfig(KConfig &theConfig, int index);
	void saveToConfig(KConfig &theConfig, int index);

	bool operator==(const Mode &mode) const { return mode.theName == theName && mode.theRemote == theRemote; }

	Mode();
	Mode(const QString &remote, const QString &name, const QString &iconFile = QString::null);
	~Mode();
};

#endif
