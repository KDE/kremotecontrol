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

#include "action.h"

Action::Action(ActionType type, const Solid::Control::RemoteControlButton &button): m_type(type), m_button(button){
}

Action::Action(const Action& action): m_button(action.button()){
  m_type = action.type();
}

Action::ActionType Action::type() const{
  return m_type;
}

QString Action::remote() const
{
  return m_remote;
}

Solid::Control::RemoteControlButton Action::button() const {
  return m_button;
}

Mode Action::mode() const {
  return m_mode;
}

void Action::operator=(const Action& action) {
  m_type = action.type();
  m_remote = action.remote();
  m_mode = action.mode();
  m_button = action.button();
}

bool Action::operator==(const Action& action) const {
  return m_type == action.type() &&
	  m_remote == action.remote() &&
	  m_mode == action.mode() &&
	  m_button.id() == action.button().id() &&
	  m_button.remoteName() == action.button().remoteName();
}

