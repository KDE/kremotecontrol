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

#include "profileactiontemplate.h"


  ProfileActionTemplate::ProfileActionTemplate(
	  const QString &profileId,
	  const QString &actionTemplateId,
	  const QString &actionName,
	  const QString &serviceName,
	  const QString &node,
	  const Prototype &function,
	  const ProfileAction::ActionDestination destination,
	  bool autostart,
	  bool repeat,
	  const QString &description,
	  const QString &buttonName)
  {
  m_profileId = profileId;
  m_actionTemplateId = actionTemplateId;
  m_actionName = actionName;
  m_node = node;
  m_serviceName = serviceName;
  m_function = function;
  m_description = description;
  m_destination= destination;
  m_autostart = autostart;
  m_repeat = repeat;
  m_buttonName = buttonName;
}

QString ProfileActionTemplate::profileId() const
{
 return m_profileId;
}


QString ProfileActionTemplate::actionTemplateId() const
{
  return m_actionTemplateId;
}


QString ProfileActionTemplate::actionName() const
{
  return m_actionName;
}

QString ProfileActionTemplate::service() const {
  return m_serviceName;
}

QString ProfileActionTemplate::node() const {
  return m_node;
}

QString ProfileActionTemplate::description() const {
  return m_description;
}

Prototype ProfileActionTemplate::function() const
{
  return m_function;
}

DBusAction::ActionDestination ProfileActionTemplate::destination() const
{
  return m_destination;
}

bool ProfileActionTemplate::autostart() const
{
  return m_autostart;
}

bool ProfileActionTemplate::repeat() const
{
  return m_repeat;
}

QString ProfileActionTemplate::buttonName() const
{
  return m_buttonName;
}

ProfileAction *ProfileActionTemplate::createAction(const Solid::Control::RemoteControlButton& button) const {
  ProfileAction *action = new ProfileAction(button.name(), m_profileId, QString());
  action->setApplication(m_serviceName);
  action->setNode(m_node);
  action->setFunction(m_function);
  action->setDestination(m_destination);
  action->setAutostart(m_autostart);
  action->setRepeat(m_repeat);
  return action;
}

