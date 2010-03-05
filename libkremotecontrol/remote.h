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
    friend class CycleModeChangeHandler;

    public:
        enum ModeChangeMode { Group, Cycle };

        Remote();
        explicit Remote(const QString& remote, Remote::ModeChangeMode changeMode = Remote::Group);

        /**
          * Destrys the Remote and all of its Modes
          */
        ~Remote();

        bool isAvailable() const;

        /**
          * Add the given Mode to this Remote. The Remote takes ownership of this Mode. Don't delete it!
          */
        void addMode(Mode* mode);

        /**
          * Remove the given Mode from this Remote and delete it. Don't try to access the Mode after this call!
          * This function will not remove the Master mode! If the deleted Mode is the default mode, the Master will be set
          * to as the new default Mode.
          */
        void removeMode(Mode *mode);
        QList<Mode*> allModes() const;

        Mode *masterMode() const;

        Mode *defaultMode() const;
        void setDefaultMode(Mode *mode);
        void setDefaultMode(const QString &name);
        Mode *modeByName(const QString &name) const;

        Mode *currentMode() const;
        void setCurrentMode(Mode *mode);

        QString name() const;

        void moveModeUp(Mode *mode);
        void moveModeDown(Mode *mode);

        bool nextMode(const QString &button);

        ModeChangeMode modeChangeMode() const;
        void setModeChangeMode(ModeChangeMode modeChangeMode);

        /**
          * Get buttons available for this mode (Free buttons + the current button for this mode)
          */
        QStringList availableModeSwitchButtons(const Mode *mode = 0) const;
        /**
          * Get buttons available for switching to the next mode (Free buttons + the current button nextMode)
          */
        QStringList availableNextModeButtons() const;
        /**
          * Get buttons available for switching to the previous mode (Free buttons + the current button previousMode)
          */
        QStringList availablePreviousModeButtons() const;

        QString nextModeButton() const;
        void setNextModeButton(const QString &button);
        QString previousModeButton() const;
        void setPreviousModeButton(const QString &button);

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
