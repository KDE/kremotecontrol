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

#include <qvaluelist.h>
#include <qpair.h>
#include <qstring.h>
#include <qmap.h>

#include "iraction.h"

/**
@author Gav Wood
*/

class KConfig;

typedef QValueListIterator<IRAction> IRAIt;
typedef QValueList<IRAIt> IRAItList;

class IRActions: protected QValueList<IRAction>
{
private:
	void purgeAllBindings(KConfig &theConfig);

public:
	IRAIt addAction(const IRAction &theAction);
	IRAItList findByButton(const QString &remote, const QString &button);
	IRAItList findByMode(const QString &remote, const QString &mode);
	IRAItList findByModeButton(const QString &remote, const QString &mode, const QString &button);

	void erase(QValueListIterator<IRAction> action) { erase(action); }

	void loadFromConfig(KConfig &theConfig);
	void saveToConfig(KConfig &theConfig);
};


#endif
