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

#ifndef PROFILE_H
#define PROFILE_H

#include "profileactiontemplate.h"
#include "kremotecontrol_export.h"



class KREMOTECONTROL_EXPORT Profile
{


  private:
    class ProfileVersion {

      private:
	int m_major;
	int m_minor;

      public:
	ProfileVersion(){
	  m_major=0;
	  m_minor=0;
	};
	ProfileVersion(const QString &version);
	const int operator==(const ProfileVersion &other) const;
	QString toString() const;
    };
  
  public:
    Profile(const QString &profileId, const QString &name, const QString &version, const QString &author, const QString &description = QString());
    Profile(){};
    QString name() const;
    QString author() const;
    QString version() const;
    QString description() const;
    QString profileId() const;
    QList<ProfileActionTemplate> actionTemplates() const;
    ProfileActionTemplate actionTemplate(const QString& actionTemplateID) const;
    void addTemplate(const ProfileActionTemplate &actionTemplate);
    int compareVersion(const Profile& other) const;
    
  protected:
    QString m_profileId;
    QString m_name;
    ProfileVersion m_version;
    QString m_author;
    QString m_description;
    QList<ProfileActionTemplate> m_actionTemplates;

};

#endif // PROFILE_H
