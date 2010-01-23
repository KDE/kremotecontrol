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

#ifndef NEWPROFILESERVER_H
#define NEWPROFILESERVER_H

#include "profile.h"
#include "kremotecontrol_export.h"
#include <QAbstractMessageHandler>
#include  <QXmlDefaultHandler>
#include <QDomDocument>
#include <QXmlSchema>

namespace NewProfileServer
{
    void addProfile(const NewProfile &profile);
    
    QList<NewProfile> allProfiles();
    NewProfile profile(const QString &profileName);
    
    QList<ProfileActionTemplate> actionTemplateList(const QString &remote, const NewProfile &profile);

    ProfileActionTemplate actionTemplate(const NewProfileAction* action);


    class  ProfileXmlContentHandler    : public QAbstractMessageHandler {

      private:
        ProfileActionTemplate parseAction(QDomNode actionNode, const QString& profileId);
	QXmlSchema *m_schema;
	 
	 NewProfile m_currentProfile;
      public:
	 ProfileXmlContentHandler(const QUrl& schema);
	 ~ProfileXmlContentHandler();
	 bool validateFile(const QString& fileName);
	 bool  parseFile(const QString& fileName);
	 QList<NewProfile> loadProfilesFromFiles(const QStringList& files);

	 NewProfile getParsedProfile(){
	    return m_currentProfile;
	 }


      protected:
      virtual void handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation);

    };
};

#endif // NEWPROFILESERVER_H
