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

Remote::Remote() {
    // Always create the Master Mode and set it default
    Mode *masterMode = new Mode("");
    addMode(masterMode);
    setDefaultMode(masterMode);
}

Remote::Remote(const QString &remote, const QList<Mode*> &modes) {
    // Always create the Master Mode and set it default
    Mode *masterMode = new Mode("");
    addMode(masterMode);
    setDefaultMode(masterMode);
    
    m_modeList = modes;
    m_remoteName = remote;
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
    if(mode->name().isEmpty()){
        // Cannot delete the Master Mode...
        return;
    }
    
    if(m_defaultMode == mode){
        // Deleting the Default Mode... Setting Master Mode to default
        foreach(Mode *tmp, m_modeList){
            if(tmp->name().isEmpty()){
                m_defaultMode = tmp;
                break;
            }
        }
    }
    m_modeList.removeAll(mode);
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

bool Remote::isAvailable() const {
    return Solid::Control::RemoteControl::allRemoteNames().contains(m_remoteName);
}
