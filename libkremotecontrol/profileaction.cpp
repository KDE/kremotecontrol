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

ProfileAction::ProfileAction(): DBusAction() {
  //change type from DBusAction (c'tor) to ProfileAction
  m_type = Action::ProfileAction;
  
}

ProfileAction::ProfileAction(const QString& button, const QString &profileName, const QString &actionTemplate): DBusAction(button) {
  //change type from DBusAction (c'tor) to ProfileAction
  m_type = Action::ProfileAction;
  m_profileName = profileName;
  m_template = actionTemplate;
}

QString ProfileAction::profileName() const
{
  return m_profileName;
}

QString ProfileAction::actionTemplateID() const
{
  return m_template;
}

void ProfileAction::operator=(const ProfileAction& action) {
  DBusAction::operator=(action);
  m_profileName = action.profileName();
  m_template = action.actionTemplateID();
}

bool ProfileAction::operator==(const ProfileAction& other) const {
  return DBusAction::operator==(other) &&
	  m_profileName == other.profileName() &&
	  m_template == other.actionTemplateID();
}

QString ProfileAction::name() const {
  return m_template;
}

QString ProfileAction::description() const {
  return m_function.name();
}

void ProfileAction::saveToConfig(KConfigGroup &config) {
    DBusAction::saveToConfig(config);
    config.writeEntry("ProfileName", m_profileName);
    config.writeEntry("Template", m_template);
}

void ProfileAction::loadFromConfig(const KConfigGroup &config) {
    DBusAction::loadFromConfig(config);
    m_profileName = config.readEntry("ProfileName");
    m_template = config.readEntry("Template");
}
