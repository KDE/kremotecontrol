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

#include <qpair.h>

#include <QMap>

#include "iraction.h"
#include "mode.h"

/**
@author Gav Wood
*/

class KConfig;

class IRActions: public QList<IRAction*>
{
private:
    void purgeAllBindings(KConfig &theConfig);

public:
    void addAction(IRAction *theAction);
    IRActions findByButton(const QString &remote, const QString &button);
    IRActions findByMode(const Mode &mode);
    IRActions findByModeButton(const Mode &mode, const QString &button);

    void erase(IRAction *action);
    void renameMode(const Mode &mode, const QString &to);

    void loadFromConfig(KConfig &theConfig);
    void saveToConfig(KConfig &theConfig);
};


#endif
