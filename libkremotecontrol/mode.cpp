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

#include "mode.h"
#include "action.h"

Mode::Mode() {

}

Mode::Mode(const QString &name, const QString &iconName)
{
  m_name = name;
  m_iconName = iconName;
}

QString Mode::name() const {
  return m_name;
}

QString Mode::iconName() const {
  return m_iconName;
}

void Mode::setName(const QString& name) {
  m_name = name;
}

void Mode::setIconName(const QString& iconName)
{
  m_iconName = iconName;
}

bool Mode::operator==(const Mode& mode) const {
  return m_name == mode.name() && m_iconName == mode.iconName();
}

void Mode::addAction(Action* action) {
    m_actionList.append(action);
}

QList< Action* > Mode::actions() const {
    return m_actionList;
}

QList<Action*> Mode::actionsForButton(const QString &button) const{
    QList<Action*> retList;
    foreach(Action *action, m_actionList){
        if(action->button() == button){
            retList.append(action);
        }
    }
    return retList;
}

