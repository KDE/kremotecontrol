/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood
  */

#ifndef IRACTIONS_H
#define IRACTIONS_H

#include <qpair.h>


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
