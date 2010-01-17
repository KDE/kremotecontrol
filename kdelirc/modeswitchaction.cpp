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

#include "modeswitchaction.h"


ModeSwitchAction::ModeSwitchAction(const Solid::Control::RemoteControlButton &button): Action(Action::ModeSwitchAction, button)
{

}

bool ModeSwitchAction::executeActionsAfterSwitch() const{
  return m_executeActionsAfterSwitch;
}

Mode ModeSwitchAction::newMode() const {
  return m_mode;
}

void ModeSwitchAction::operator=(const ModeSwitchAction& action) {
  Action::operator=(action);
  m_mode = action.newMode();
  m_executeActionsAfterSwitch = action.executeActionsAfterSwitch();
}

bool ModeSwitchAction::operator==(const ModeSwitchAction& other) const {
  return Action::operator==(other) &&
	  m_mode == other.mode() &&
	  m_executeActionsAfterSwitch == other.executeActionsAfterSwitch();
}
