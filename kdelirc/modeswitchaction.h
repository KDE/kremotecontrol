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

#ifndef MODESWITCHACTION_H
#define MODESWITCHACTION_H

#include "action.h"
#include "mode.h"

class ModeSwitchAction : public Action
{
  
  public:
    ModeSwitchAction(const Solid::Control::RemoteControlButton &button);
    
    Mode newMode() const;
    void setNewMode(const Mode &mode);
    
    bool executeActionsAfterSwitch() const;
    void setExecuteActionsAfterSwitch(bool execute);
    
    void operator=(const ModeSwitchAction &action);
    bool operator==(const ModeSwitchAction &other) const;
  
  protected:
    Mode m_mode;
    bool m_executeActionsAfterSwitch;
    
};

#endif // MODESWITCHACTION_H
