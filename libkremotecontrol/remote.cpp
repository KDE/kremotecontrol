/*
    Copyright (C) <2010>  Michael Zanetti <michael_zanetti@gmx.net>

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

#include "remote.h"

#include <KLocale>
#include <kdebug.h>

class ModeChangeHandler{
  public:
      virtual  bool isButtonModechange(const Remote *remote, const QString &button ) =0;
      virtual  void nextMode(Remote *remote, const QString &button)=0;
};    


class GroupModechangehandler : public ModeChangeHandler{

  bool isButtonModechange(const Remote* remote, const QString& button) {
    foreach(Mode *mode, remote->m_modeList){
      if(mode->button() == button && ! (mode == remote->currentMode())){
	return true;
      }
    }
    return false;
  }


  void nextMode(Remote* remote, const QString& button) {
    int index=remote->m_modeList.indexOf(remote->currentMode());
    Mode *mode =0;
    int size = remote->m_modeList.size();
    if(index < size){
      for(int i = index +1; i < size ; ++i){
	if(remote->m_modeList.at(index)->button() == button){
	  mode = remote->m_modeList.at(index);
	}
      }
    }  
    if(mode== 0){
      for(int i = 0; i < index; ++i){
	if(remote->m_modeList.at(index)->button() == button){
	  mode = remote->m_modeList.at(index);
	}
      }
    }
    if(mode){
      remote->setCurrentMode(mode);
    }
  }
};


Remote::Remote() {
    // Always create the Master Mode and set it default
    Mode *masterMode = new Mode("Master");
    addMode(masterMode);
    setDefaultMode(masterMode);
    m_modechangeHandler = new GroupModechangehandler();
}

Remote::Remote(const QString &remote, const QList<Mode*> &modes) {
    m_modeList = modes;
    m_remoteName = remote;

    // Always create the Master Mode and set it default
    bool hasMaster = false;
    foreach(Mode *mode, m_modeList){
        if(mode->name() == "Master"){
            hasMaster = true;
            break;
        }
    }
    if(!hasMaster){
        Mode *masterMode = new Mode("Master");
        addMode(masterMode);
        setDefaultMode(masterMode);
    }

}

QString Remote::name() const {
  return m_remoteName;
}

QList<Mode*> Remote::allModes() const {
  return m_modeList;
}

void Remote::addMode(Mode *mode) {
  m_modeList.append(mode);
}

void Remote::removeMode(Mode *mode) {
    if(mode->name() == "Master"){
        kDebug() << "cannot delete the Master mode";
        return;
    }

    if(m_defaultMode == mode){
        // Deleting the Default Mode... Setting Master Mode to default
        foreach(Mode *tmp, m_modeList){
            if(tmp->name() == "Master"){
                m_defaultMode = tmp;
                break;
            }
        }
    }
    m_modeList.removeAll(mode);
}

Mode* Remote::masterMode() const {
    foreach(Mode *mode, m_modeList){
        kDebug() << "checking Mode" << mode->name();
        if(mode->name() == "Master"){
            return mode;
        }
    }
    kDebug() << "Master mode not found";
    // Huh??? No Master Mode? Should never happen as removeMode() doesn't delete the Master mode
    // and all ctors create a Master Mode...
    return 0;
}

Mode *Remote::defaultMode() const {
    return m_defaultMode;
}

void Remote::setDefaultMode(Mode *mode) {
    if(!m_modeList.contains(mode)){
        m_modeList.append(mode);
    }
    m_defaultMode = mode;
}

void Remote::setDefaultMode(const QString &modeName){
    foreach(Mode *mode, m_modeList){
        if(mode->name() == modeName){
            setDefaultMode(mode);
            return;
        }
    }
}

Mode* Remote::currentMode() const {
    if(m_currentMode != 0){
        return m_currentMode;
    }
    return m_defaultMode;
}

void Remote::setCurrentMode(Mode* mode) {
    m_currentMode = mode;
}

bool Remote::isAvailable() const {
    return Solid::Control::RemoteControl::allRemoteNames().contains(m_remoteName);
}

bool Remote::isButtonModechange(const QString& button) {
  return m_modechangeHandler->isButtonModechange(this,button);
}

void Remote::nextMode ( const QString& button ){
  m_modechangeHandler->nextMode(this, button);
}




