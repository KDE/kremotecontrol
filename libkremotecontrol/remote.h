/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef REMOTE_H
#define REMOTE_H

#include "mode.h"

#include <solid/control/remotecontrol.h>


#include <QSet>

class KREMOTECONTROL_EXPORT Remote
{
    public:
        Remote();

        Remote(const QString& remote, const QList<Mode>& modes = QList<Mode>() );

        bool isAvailable() const;
        
        void addMode(const Mode &mode) ;
        void removeMode(const Mode &mode);
        QList<Mode> allModes() const;
        
        Mode defaultMode() const;
        void setDefaultMode(const Mode &mode );

//        QSet<QString> buttonNames() const;
        QString name() const;

//         void remote(const Solid::Control::RemoteControl& remote);
//         void modeList(QList< Mode > modeList);

//         QStringList modesToStringList();

    private:
        QList<Mode> m_modeList;
        QString m_defaultModeName;
        QString m_remoteName;
        QSet<QString> m_buttonNameSet;
        bool m_availableInSolid;

};

Q_DECLARE_METATYPE(Remote*)

#endif // REMOTE_H
