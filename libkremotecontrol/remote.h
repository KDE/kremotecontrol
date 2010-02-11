/*
    Copyright (C) <2010> Michael Zanetti <michael_zanetti@gmx.net>

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

class ModeChangeHandler;

class KREMOTECONTROL_EXPORT Remote
{
    
    friend class GroupModeChangeHandler;
    
    public:
        enum ModeChangeMode { Group, Cycle };
      
        Remote();

        Remote(const QString& remote, const QList<Mode*>& modes = QList<Mode*>());

        bool isAvailable() const;

        void addMode(Mode *mode);
        void removeMode(Mode *mode);
        QList<Mode*> allModes() const;

        Mode *masterMode() const;

        Mode *defaultMode() const;
        void setDefaultMode(Mode *mode);
        void setDefaultMode(const QString &name);

        Mode *currentMode() const;
        void setCurrentMode(Mode *mode);

        QString name() const;

        bool nextMode(const QString &button);
        
        ModeChangeMode modeChangeMode() const;
        QStringList availableModeSwitchButtons() const;
        
        QString nextModeButton() const;
        QString previousModeButton() const;

    private:
        QList<Mode*> m_modeList;
        Mode *m_defaultMode;
        QString m_remoteName;
        Mode *m_currentMode;
        ModeChangeHandler *m_modechangeHandler;
        QString m_nextModeButton;
        QString m_previousModeButton;
};





Q_DECLARE_METATYPE(Remote*)

#endif // REMOTE_H
