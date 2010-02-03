/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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
    Mode masterMode("Master");
    addMode(masterMode);
    setDefaultMode(masterMode);
}

Remote::Remote(const QString &remote, const QList<Mode> &modes) {
    // Always create the Master Mode and set it default
    Mode masterMode("Master");
    addMode(masterMode);
    setDefaultMode(masterMode);
    
    m_modeList = modes;
    m_remoteName = remote;
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(remote).buttons()){
        m_buttonNameSet.insert(button.name());
    }
}

/*
void Remote::remote(const Solid::Control::RemoteControl& remote) const {
    m_remoteName = remote.name();
    m_availableInSolid = true;
}




void Remote::modeList(QList< Mode > modeList)
{
  m_modeList = modeList;
  m_availableInSolid = true;
}



QSet< QString > Remote::buttonNames() const
{
  return m_buttonNameSet;
}*/

QString Remote::name() const {
  return m_remoteName;
}

QList< Mode> Remote::allModes() const
{
  return m_modeList;
}

void Remote::addMode(const Mode& mode)
{
  m_modeList.append(mode);
}

void Remote::removeMode(const Mode& mode)
{
    m_modeList.removeAll(mode);
}


Mode Remote::defaultMode() const{
    foreach(const Mode &mode, m_modeList){
        if (mode.name() == m_defaultModeName){
            return mode;
        }
    }
    
    // Default Mode not found... returning Master Mode
    return Mode(i18n("Master"));
}


void Remote::setDefaultMode(const Mode &mode )
{
 if(m_modeList.contains(mode)){
    m_defaultModeName = mode.name();
 }
}

// QStringList Remote::modesToStringList()
// {
//   QStringList list;
// 
// foreach(const Mode &mode, m_modeList){
//     list <<  mode.name();
//   }
// }


// bool Remote::isAvailableInSolid()
// {
//   return m_availableInSolid;
// }
// 

// Solid::Control::RemoteControl Remote::remote()
// {
//   return m_remote;
// }

