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

class IRActions: public QMap<QPair<QString, QString>, QValueList<IRAction> >
{
private:
	void purgeAllBindings(KConfig &theConfig);

public:
	void addAction(const IRAction &theAction) { operator[](qMakePair(theAction.remote(), theAction.button())) += theAction; }

	void loadFromConfig(KConfig &theConfig);
	void saveToConfig(KConfig &theConfig);
};


#endif
