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

#include "dbusaction.h"

DBusAction::DBusAction(const Solid::Control::RemoteControlButton &button): Action(Action::DBusAction, button)
{

}

QString DBusAction::application() const{
  return m_application;
}

void DBusAction::setApplication(const QString& application)
{
  m_application = application;
}

QString DBusAction::node() const{
  return m_node;
}

void DBusAction::setNode(const QString& node)
{
  m_node = node;
}

Prototype DBusAction::function() const{
  return m_function;
}

void DBusAction::setFunction(const Prototype& function)
{
  m_function = function;
}

QList<NewArgument> DBusAction::arguments() const{
  return m_arguments;
}

void DBusAction::setArguments(const QList< NewArgument >& arguments)
{
  m_arguments = arguments;
}

bool DBusAction::repeat() const{
  return m_repeat;
}

void DBusAction::setRepeat(bool repeat)
{
  m_repeat = repeat;
}

bool DBusAction::autostart() const{
  return m_autostart;
}

void DBusAction::setAutostart(bool autostart)
{
  m_autostart = autostart;
}

DBusAction::ActionDestination DBusAction::destination() const{
  return m_destination;
}

void DBusAction::setDestination(DBusAction::ActionDestination destination)
{
  m_destination = destination;
}
