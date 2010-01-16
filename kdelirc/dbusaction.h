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

#ifndef DBUSACTION_H
#define DBUSACTION_H

#include "action.h"
#include "prototype.h"
#include "arguments.h"

#include <solid/control/remotecontrolbutton.h>

#include <QString>

class DBusAction : public Action
{
  public:
    enum ActionDestination {Unique, Top, Bottom, None, All};
    
    DBusAction(const Solid::Control::RemoteControlButton &button);
    
    QString application() const;
    QString node() const;
    Prototype function() const;
    Arguments arguments() const;
    bool repeat() const;
    bool autostart() const;
    ActionDestination destination() const;
    
  protected:
    QString m_application, m_node;
    Prototype m_function;
    Arguments m_arguments;
    bool m_repeat, m_autostart;
    ActionDestination m_destination;
};

#endif // DBUSACTION_H
