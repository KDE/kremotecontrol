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

#include "profile.h"

NewProfile::NewProfile(const QString &name) {
  m_name = name;
}

QString NewProfile::name() const
{
  return m_name;
}

void NewProfile::addTemplate(const ProfileActionTemplate &applicationTemplate) {
  m_actionTemplates.append(applicationTemplate);
}

QList< ProfileActionTemplate > NewProfile::actionTemplates() const {
  return m_actionTemplates;
}

ProfileActionTemplate NewProfile::actionTemplate(const QString& templateID) const
{
  foreach(const ProfileActionTemplate &actionTemplate, m_actionTemplates){
    if(actionTemplate.templateID() == templateID){
      return actionTemplate;
    }
  }
  return ProfileActionTemplate();
}
