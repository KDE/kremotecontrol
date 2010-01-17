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

#include "actionlist.h"

ActionList::ActionList()
{

}

QList<Action*> ActionList::allActions() const
{
  return m_actions;
}

QList<Action*> ActionList::findActions(const QString& remote) const
{
  QList<Action*> retList;
  foreach(Action *action, m_actions){
    if(action->remote() == remote){
      retList.append(action);
    }
  }
  return retList;
}

QList<Action*> ActionList::findActions(const QString& remote, const Mode& mode) const
{
  QList<Action*> retList;
  foreach(Action *action, m_actions){
    if(action->remote() == remote && action->mode() == mode){
      retList.append(action);
    }
  }
  return retList;
}

QList<Action*> ActionList::findActions(const QString& remote, const Mode& mode, const Solid::Control::RemoteControlButton& button) const
{
  QList<Action*> retList;
  foreach(Action *action, m_actions){
    if(action->remote() == remote && action->mode() == mode && action->button().id() == button.id() && action->button().remoteName() == button.remoteName()){
      retList.append(action);
    }
  }
  return retList;
}

void ActionList::addAction(Action *action)
{
  m_actions.append(action);
}

void ActionList::remove(Action* action)
{
  for(int i = 0; i < m_actions.size(); i++){
    if(m_actions.at(i) == action){
      m_actions.removeAt(i);
      break;
    }
  }
}
