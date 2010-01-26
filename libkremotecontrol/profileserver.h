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

#ifndef PROFILESERVER_H
#define PROFILESERVER_H

#include "profile.h"
#include "remote.h"
#include "kremotecontrol_export.h"

#include <QAbstractMessageHandler>
#include <QXmlDefaultHandler>
#include <QDomDocument>
#include <QXmlSchema>

namespace ProfileServer
{

    enum ProfileSupportedByRemote {

	  FULL_SUPPORTED,
	  PARTIAL_SUPPORTED,
	  NOT_SUPPORTED,
	  NO_ACTIONS_DEFINED
    };

    void addProfile(const Profile &profile);
    
    QList<Profile> allProfiles();
    Profile profile(const QString &profileId);
    
    QList<ProfileActionTemplate> actionTemplateList(const QString &remote, const Profile &profile);
    ProfileServer::ProfileSupportedByRemote isProfileAvailableForRemote(const  Profile &profile, const Remote &remote);
    ProfileActionTemplate actionTemplate(const ProfileAction* action);


    

    class  ProfileXmlContentHandler    : public QAbstractMessageHandler {

      private:
        ProfileActionTemplate parseAction(QDomNode actionNode, const QString& profileId);
	QXmlSchema *m_schema;
	 
	 Profile m_currentProfile;
      public:
	 ProfileXmlContentHandler(const QUrl& schema);
	 ~ProfileXmlContentHandler();
	 bool validateFile(const QString& fileName);
	 bool  parseFile(const QString& fileName);
	 QList<Profile> loadProfilesFromFiles(const QStringList& files);

	 Profile getParsedProfile(){
	    return m_currentProfile;
	 }


      protected:
      virtual void handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation);

    };
};

#endif // PROFILESERVER_H
