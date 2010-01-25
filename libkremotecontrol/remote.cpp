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

/*
Remote::Remote(const Solid::Control::RemoteControl &remote, const QList<Mode> &modes)
{
 m_remote = remote;
  m_modeList = modes;
}*/


QList< Mode> Remote::allModes() const
{
  return m_modeList;	
}

void Remote::addMode(const Mode& mode)
{
  m_modeList.append(mode);
}


Mode Remote::defaultMode() 
{
 foreach(Mode mode, m_modeList){
  if (mode.name() == m_defaultModeName){
    return mode;
  }
  Mode master(i18n("Default mode"), QString());
  m_modeList.append(master);
  m_defaultModeName = master.name();  
 } 
}


void Remote::setDefaultMode(const Mode &mode ) 
{
 if(m_modeList.contains(mode)){
    m_defaultModeName = mode.name();
 }
}

