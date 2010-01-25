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

#include "remotelist.h"


#include <kglobal.h>

class RemotesPrivate
{
  public:
   RemotesPrivate();
    QList<Remote> m_remotes;
//     ~NewProfileServerPrivate() {
//       while (!m_allProfiles.isEmpty()){
// 	delete m_allProfiles.takeFirst();
//       }
//     }
};



K_GLOBAL_STATIC(RemotesPrivate, instance)



QList< Remote > Remotes::RemoteList::remotes()
{
  return instance.m_remotes;
}
