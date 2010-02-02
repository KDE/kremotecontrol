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

DBusAction::DBusAction(const QString &button, const Mode &mode): Action(Action::DBusAction, button, mode)
{

}

DBusAction::DBusAction(): Action(Action::DBusAction){
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

void DBusAction::operator=(const DBusAction& action) {
  m_type = action.type();
  m_mode = action.mode();
  m_button = action.button();
  m_application = action.application();
  m_function = action.function();
  m_destination = action.destination();
  m_autostart = action.autostart();
  m_repeat = action.repeat();
}

bool DBusAction::operator==(const DBusAction& action) const {
  return m_type == action.type() &&
	  m_application == action.application() &&
	  m_autostart == action.autostart() &&
	  m_repeat == action.repeat() &&
	  m_destination == action.destination() &&
	  m_function == action.function() &&
	  m_node == action.node() &&
	  m_button == action.button();
}

QString DBusAction::name() const {
  return m_application + ":" + m_node;
}

QString DBusAction::description() const {
  return m_function.name();
}
