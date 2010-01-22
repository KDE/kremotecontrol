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

#ifndef REMOTE_H
#define REMOTE_H

#include "mode.h"

#include <solid/control/remotecontrol.h>

class Remote
{
  public:
    Remote();
  
    Solid::Control::RemoteControl remote();
    QList<Mode> allModes() const;
    Mode defaultMode() const;
    
  private:
    Solid::Control::RemoteControl m_remote();
    QList<Mode> m_modeList;
    QString m_defaultModeName;
    
};

#endif // REMOTE_H
