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

#ifndef MODE_H
#define MODE_H


#include <QString>
#include <qmetatype.h>

/**
@author Gav Wood
*/

class KConfig;

class Mode
{
    QString theName, theRemote, theIconFile;

public:
    void setName(const QString &a) {
        theName = a;
    }
    void setRemote(const QString &a) {
        theRemote = a;
    }
    void setIconFile(const QString &a) {
        theIconFile = a;
    }

    const QString &name() const {
        return theName;
    }
    const QString &remote() const {
        return theRemote;
    }
    const QString &iconFile() const {
        return theIconFile;
    }

    const Mode &loadFromConfig(KConfig &theConfig, int index);
    void saveToConfig(KConfig &theConfig, int index);

    bool operator==(const Mode &mode) const {
        return mode.theName == theName && mode.theRemote == theRemote;
    }

    Mode();
    Mode(const QString &remote, const QString &name, const QString &iconFile = QString());
    ~Mode();
};

Q_DECLARE_METATYPE(Mode)
#endif
