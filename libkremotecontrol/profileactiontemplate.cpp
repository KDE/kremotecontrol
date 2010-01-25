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


ProfileActionTemplate::ProfileActionTemplate(const QString& profile, const QString& actionTemplateID, const QString& application, const QString& appName, const QString& node, const QString& function, const QList< Argument >& defaultArguments, ProfileAction::ActionDestination destination, bool autostart, bool repeat, const QString& description, const QString &buttonName) {
  m_profile = profile;
  m_actionTemplateID = actionTemplateID;
  m_application = application;
  m_node = node;
  m_appName = appName;
  m_function = function;
  m_description = description;
  m_defaultArguments = defaultArguments;
  m_description = destination;
  m_autostart = autostart;
  m_repeat = repeat;
  m_buttonName = buttonName;
}

QString ProfileActionTemplate::profile() const
{
  return m_profile;
}

QString ProfileActionTemplate::actionTemplateID() const
{
  return m_actionTemplateID;
}

QString ProfileActionTemplate::appName() const {
  return m_appName;
}

QString ProfileActionTemplate::description() const {
  return m_description;
}

QList<Argument> ProfileActionTemplate::defaultArguments() const
{
  return m_defaultArguments;
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

ProfileAction *ProfileActionTemplate::createAction(const Solid::Control::RemoteControlButton& button, const Mode &mode) const
{
  ProfileAction *action = new ProfileAction(button, mode, m_profile, QString());
  action->setApplication(m_application);
  action->setNode(m_node);
  action->setFunction(m_function);
  QList<Argument> newArgs;
  foreach(const Argument &arg, m_defaultArguments){
    newArgs.append(Argument(arg.defaultValue()));
  }
  action->setArguments(newArgs);
  action->setDestination(m_destination);
  action->setAutostart(m_autostart);
  action->setRepeat(m_repeat);
  return action;
}

