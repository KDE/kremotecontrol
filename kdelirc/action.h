/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef ACTION_H
#define ACTION_H

#include "mode.h"
#include "kdelirc_export.h"

#include <solid/control/remotecontrolbutton.h>
#include <solid/control/remotecontrol.h>

class KDELIRC_EXPORT Action: public QObject
{
  Q_OBJECT
  
  public:
    enum ActionType {ModeSwitchAction, DBusAction, ProfileAction};
  
    Action(ActionType type, const Solid::Control::RemoteControlButton &button);
    Action(const Action &action);
  
    ActionType type() const;
    QString remote() const;
    Solid::Control::RemoteControlButton button() const;
    Mode mode() const;
    
    virtual void operator=(const Action &action);
    virtual bool operator==(const Action &action) const;
    
  protected:
    ActionType m_type;
    QString m_remote;
    Solid::Control::RemoteControlButton m_button;
    Mode m_mode;
    
};

#endif // ACTION_H