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

#include "profileaction.h"

NewProfileAction::NewProfileAction(const Solid::Control::RemoteControlButton& button, const Mode &mode, const QString &profileName, const QString &actionTemplate): DBusAction(button, mode)
{
  //change type from DBusAction (c'tor) to ProfileAction
  m_type = Action::ProfileAction;
  m_profileName = profileName;
  m_template = actionTemplate;
}

QString NewProfileAction::profileName() const
{
  return m_profileName;
}

QString NewProfileAction::actionTemplateID() const
{
  return m_template;
}

void NewProfileAction::operator=(const NewProfileAction& action) {
  DBusAction::operator=(action);
  m_profileName = action.profileName();
  m_template = action.actionTemplateID();
}

bool NewProfileAction::operator==(const NewProfileAction& other) const {
  return DBusAction::operator==(other) &&
	  m_profileName == other.profileName() &&
	  m_template == other.actionTemplateID();
}

