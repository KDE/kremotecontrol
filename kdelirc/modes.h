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
	QMap<QString, QString> theDefaults;

public:
	void loadFromConfig(KConfig &theConfig);
	void saveToConfig(KConfig &theConfig);
	void generateNulls(const QStringList &theRemotes);

	const Mode &getMode(const QString &remote, const QString &mode) const;
	ModeList getModes(const QString &remote) const;
	const Mode getDefault(const QString &remote) const;
	bool isDefault(const Mode &mode) const;

	/**
	 * Call when you've changed a previously getMode'd mode and you want the changes
	 *   to be recorded
	 **/
	void updateMode(const Mode &mode) { operator[](mode.remote())[mode.name()] = mode; }
	void setDefault(const Mode &mode) { theDefaults[mode.remote()] = mode.name(); }
	void erase(const Mode &mode);
	void add(const Mode &mode);

	// dont use this without renaming all the modes in the actions!!!
	void rename(Mode &mode, const QString name);

	Modes();
	~Modes();
};

#endif
