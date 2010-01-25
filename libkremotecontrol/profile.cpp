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


#include <kdebug.h>

Profile::Profile(const QString &profileId, const QString &name, const QString &version, const QString &author, const QString &description) {
  m_profileId = profileId;
  m_name = name;
  m_version = ProfileVersion(version);
  m_author = author;
  m_description = description;
}

QString Profile::name() const
{
  return m_name;
}

QString Profile::author() const
{
  return m_author;
}

QString Profile::description() const
{
  return m_description;
}

QString Profile::profileId() const
{
  return m_profileId;
}

void Profile::addTemplate(const ProfileActionTemplate &applicationTemplate) {
  m_actionTemplates.append(applicationTemplate);
}

QList< ProfileActionTemplate > Profile::actionTemplates() const {
  return m_actionTemplates;
}

ProfileActionTemplate Profile::actionTemplate(const QString& actionTemplateID) const
{
  foreach(const ProfileActionTemplate &actionTemplate, m_actionTemplates){
    if(actionTemplate.actionTemplateID() == actionTemplateID){
      return actionTemplate;
    }
  }
  return ProfileActionTemplate();
}


QString Profile::version() const
{
  return m_version.toString();
}


int Profile::compareVersion(const Profile &other) const
{
  return m_version == other.version();
}

const int Profile::ProfileVersion::operator==(const Profile::ProfileVersion& other) const
{
  if(m_major == other.m_major){
    if(m_minor > other.m_minor){
      return 1;
    }
    else if(m_minor < other.m_minor){
      return -1;
    }
  }else{
    if(m_major > other.m_major){
      return 1;
    }
    else if(m_major < other.m_major){
      return -1;
    }
  }
  return 0;
}

Profile::ProfileVersion::ProfileVersion(const QString& version)
{
  QStringList split=  version.split(".");
   m_major = split.at(0).toInt();
  m_minor = split.at(1).toInt();
}

QString Profile::ProfileVersion::toString() const
{  
  return  QString().setNum(m_major) + "." + QString().setNum(m_minor);
  
}
