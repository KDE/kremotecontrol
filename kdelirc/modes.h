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
#ifndef MODES_H
#define MODES_H

#include <qstring.h>
#include <qmap.h>
#include <qpair.h>
#include <qvaluelist.h>

#include "mode.h"

/**
@author Gav Wood
*/

class KConfig;

typedef QValueList<Mode> ModeList;

class Modes : protected QMap<QString, QMap<QString, Mode> >
{
	void purgeAllModes(KConfig &theConfig);

public:
	void loadFromConfig(KConfig &theConfig);
	void saveToConfig(KConfig &theConfig);

	const Mode &getMode(const QString &remote, const QString &mode) const;
	ModeList getModes(const QString &remote) const;

	void erase(const Mode &mode);
	void erase(const QString &remote, const QString &mode);
	void add(const Mode &mode);

	// dont use this without renaming all the modes in the actions!!!
	void rename(Mode &mode, const QString name);

	Modes();
	~Modes();
};

#endif
