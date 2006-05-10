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
#ifndef IRACTIONS_H
#define IRACTIONS_H

#include <q3valuelist.h>
#include <qpair.h>
#include <QString>
#include <QMap>

#include "iraction.h"
#include "mode.h"

/**
@author Gav Wood
*/

class KConfig;

typedef Q3ValueListIterator<IRAction> IRAIt;
typedef Q3ValueList<IRAIt> IRAItList;

class IRActions: protected Q3ValueList<IRAction>
{
private:
	void purgeAllBindings(KConfig &theConfig);

public:
	IRAIt addAction(const IRAction &theAction);
	IRAItList findByButton(const QString &remote, const QString &button);
	IRAItList findByMode(const Mode &mode);
	IRAItList findByModeButton(const Mode &mode, const QString &button);

	void erase(const IRAIt &action) { Q3ValueList<IRAction>::erase(action); }
	void renameMode(const Mode &mode, const QString &to);

	void loadFromConfig(KConfig &theConfig);
	void saveToConfig(KConfig &theConfig);
};


#endif
