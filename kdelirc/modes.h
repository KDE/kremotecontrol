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

#ifndef MODES_H
#define MODES_H


#include <QMap>
#include <qpair.h>
#include <QList>

#include "mode.h"

/**
@author Gav Wood
*/

class KConfig;

typedef QList<Mode> ModeList;

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
    void updateMode(const Mode &mode) {
        operator[](mode.remote())[mode.name()] = mode;
    }
    void setDefault(const Mode &mode) {
        theDefaults[mode.remote()] = mode.name();
    }
    void erase(const Mode &mode);
    void add(const Mode &mode);

    // don't use this without renaming all the modes in the actions!!!
    void rename(Mode &mode, const QString name);

    Modes();
    ~Modes();
};

#endif
