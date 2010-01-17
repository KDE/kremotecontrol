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

#include <kdebug.h>

ActionList::ActionList()
{

}

void ActionList::loadFromConfig(const KConfig &config)
{
  clear();
  // Load actions from config into m_actions
  
}

void ActionList::saveToConfig(const KConfig &config)
{
  // Save m_actions into config
}

ActionList ActionList::findActions(const QString& remote) {
  ActionList retList;
  for(ActionList::iterator i = begin(); i != end(); ++i){
    if((*i)->remote() == remote){
      retList.append(*i);
    }
  }
  return retList;
}

ActionList ActionList::findActions(const Mode& mode) {
  ActionList retList;
  for(ActionList::iterator i = begin(); i != end(); ++i){
    if((*i)->mode() == mode){
      retList.append(*i);
    }
  }
  return retList;
}

ActionList ActionList::findActions(const Mode& mode, const Solid::Control::RemoteControlButton& button) {
  ActionList retList;
  kDebug() << "searching for action with mode" << mode.name() << "remote" << mode.remote() << "and button" << button.name();
  for(ActionList::iterator i = begin(); i != end(); ++i){
    kDebug() << "searching in action with mode" << (*i)->mode().name() << "remote" << (*i)->remote() << " and button:" << (*i)->button().name();
    if((*i)->mode() == mode && (*i)->button().id() == button.id() && (*i)->button().remoteName() == button.remoteName()){
      retList.append(*i);
    }
  }
  return retList;
}
