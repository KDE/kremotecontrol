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

#include "profileserver.h"
#include "profileactiontemplate.h"
#include "executionengine.h"
#include "dbusaction.h"
#include "remote.h"

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <QtCore/QFileInfo>
#include <QtGui/QTextDocument>
#include <QtXml/QXmlSimpleReader>
#include <QtXmlPatterns/QXmlSchemaValidator>
#include <QtXmlPatterns/QXmlSchema>
#include <klocalizedstring.h>

class ProfileServerPrivate
{
    private:
        QList<Profile*> m_allProfiles;

    public:
        ProfileServerPrivate();
        void addProfile(Profile* profile);
        QList<Profile*> allProfiles();

        ~ProfileServerPrivate() {
            while (!m_allProfiles.isEmpty()){
                delete m_allProfiles.takeFirst();
            }
        }
};

K_GLOBAL_STATIC(ProfileServerPrivate, instance)

QList<Profile*> ProfileServerPrivate::allProfiles() {
    return m_allProfiles;
}

ProfileServerPrivate::ProfileServerPrivate() {
    ProfileServer::ProfileXmlContentHandler *handler = new ProfileServer::ProfileXmlContentHandler(KUrl::fromLocalFile(KGlobal::dirs()->findResource("data",QLatin1String( "kremotecontrol/profiles/profile.xsd" ))));
    foreach( Profile *profile, handler->loadProfilesFromFiles(KGlobal::dirs()->findAllResources("data", QLatin1String( "kremotecontrol/profiles/*.profile.xml" )))){
        addProfile(profile);
    }
}

void ProfileServerPrivate::addProfile( Profile* profile) {
    for(int i =0; i< m_allProfiles.size(); i++){
        Profile *tProfile = m_allProfiles.at(i);
        if(profile->profileId() == tProfile->profileId()){
            if( 1 ==  profile->compareVersion(tProfile)){
                // new profileversion is greater as current -> replace
                m_allProfiles.replace(i, profile);
                return;
            } else {
                // in this case keep profile (first come first served...)
                return;
            }
        }
    }
    // Profile is no in list. Append
    m_allProfiles.append(profile);
}

KREMOTECONTROL_EXPORT void ProfileServer::addProfile(Profile* profile) {
    instance->addProfile(profile);
}

KREMOTECONTROL_EXPORT QList< Profile*> ProfileServer::allProfiles() {
    return instance->allProfiles();
}

KREMOTECONTROL_EXPORT Profile* ProfileServer::profile(const QString& profileId) {
    foreach(Profile *profile, instance->allProfiles()){
        if(profile->profileId() == profileId){
            return profile;
        }
    }
    kDebug() << "Profile" << profileId<< "not found.";
    return 0;
}

KREMOTECONTROL_EXPORT QList<ProfileActionTemplate> ProfileServer::actionTemplateList(const QString& remote, Profile* profile) {
    QList<ProfileActionTemplate> retList;
    foreach(const ProfileActionTemplate &actionTemplate, profile->actionTemplates()){
        kDebug() << "got template" << actionTemplate.actionTemplateId() << "with button" << actionTemplate.buttonName();
        foreach(const RemoteControlButton &button, RemoteControl(remote).buttons()){
            kDebug() << "got button" << button.name();
            if(button.name() == actionTemplate.buttonName()){
                retList.append(actionTemplate);
            }
        }
    }
    return retList;
}

ProfileServer::ProfileXmlContentHandler::ProfileXmlContentHandler(const QUrl &schemaFile) {
    m_schema = new QXmlSchema();
    m_schema->setMessageHandler(this);
    m_schema->load(schemaFile);
}

ProfileServer::ProfileXmlContentHandler::~ProfileXmlContentHandler() {
    delete m_schema;
}

bool ProfileServer::ProfileXmlContentHandler::validateFile(const QString& fileName) {
    if ( m_schema->isValid() ) {
        const QStringList theFiles = KGlobal::dirs()->findAllResources("data", QLatin1String( "kremotecontrol/profiles/*.profile.xml" ));
        QXmlSchemaValidator validator(*m_schema);
        return validator.validate( QUrl::fromLocalFile(fileName));
    }
    return false;
}

void ProfileServer::ProfileXmlContentHandler::handleMessage(QtMsgType type, const QString& description, const QUrl& identifier, const QSourceLocation& sourceLocation) {
    Q_UNUSED(type);
    Q_UNUSED(identifier);
    Q_UNUSED(sourceLocation);
    QTextDocument document;
    document.setHtml(description);
    kDebug() << "Error validating xml file " << sourceLocation.uri().toString() << " Message " << document.toPlainText();
}

ProfileServer::ProfileSupportedByRemote KREMOTECONTROL_EXPORT ProfileServer::isProfileAvailableForRemote(Profile* profile, const Remote& remote) {
    QStringList tProfilActionNames;
    foreach(const ProfileActionTemplate profileAction, profile->actionTemplates()) {
        if (! profileAction.buttonName().isEmpty()) {
            tProfilActionNames << profileAction.buttonName();
        }
    }
    if (tProfilActionNames.size() == 0) {
        return ProfileServer::NO_ACTIONS_DEFINED;
    }
    int found=0;
    foreach(const QString & tProfilActionName, tProfilActionNames) {
        foreach(const RemoteControlButton &button, RemoteControl(remote.name()).buttons()) {
            if(button.name() == tProfilActionName){
                found++;
            }
        }
    }
    if (found == 0) {
        return ProfileServer::NOT_SUPPORTED;
    } else if (found != tProfilActionNames.size()) {
        return ProfileServer::PARTIAL_SUPPORTED;
    }
    return ProfileServer::FULL_SUPPORTED;
}

/*
********************************************************************
* ProfileServer::ProfileXmlContentHandler
*
********************************************************************
*/

QList<Profile*> ProfileServer::ProfileXmlContentHandler::loadProfilesFromFiles(const QStringList& files) {
    QList<Profile*> profileList;
    foreach(const QString &file, files) {
        if(validateFile(file)) {
            Profile *profile = parseFile(file);
            if(profile){
                profileList.append(profile);
            }
        }
    }
    return profileList;
}

Profile * ProfileServer::ProfileXmlContentHandler::parseFile(const QString& fileName) {
    //QString id = fileName.left(filename.indexOf(".profile.xml"));
    QFile file( fileName );

    QString profileId = QFileInfo(fileName).fileName();
    profileId = profileId.left(profileId.indexOf(QLatin1String( ".profile.xml" )));
    QDomDocument doc;
    QString errorMsg;
    int errorLine, errorColumn;
    if(doc.setContent( &file, &errorMsg, &errorLine, &errorColumn)) {

        QDomElement rootElement = doc.namedItem(QLatin1String( "profile" )).toElement();
        QString name = rootElement.namedItem(QLatin1String( "name" )).toElement().text().trimmed();
        QString description;
        if(!rootElement.namedItem(QLatin1String( "description" )).isNull()) {
            description = rootElement.namedItem(QLatin1String( "description" )).toElement().text().trimmed();
        }
        QString author = rootElement.namedItem(QLatin1String( "author" )).toElement().text().trimmed();
        QString version = rootElement.namedItem(QLatin1String( "version" )).toElement().text().trimmed();

        Profile *profile =  new Profile(profileId, i18n(name.toUtf8().data()) ,version, author, i18n(description.toUtf8().data()));

        QDomNodeList actionNodeList = rootElement.elementsByTagName(QLatin1String( "action" ));
        for(int count = 0; count < actionNodeList.size(); ++count){
            profile->addTemplate(parseAction(actionNodeList.at(count), profileId));
        }
        return profile;
    } else {
        kDebug() << "Could not parse xml file " << fileName;
        kDebug() << " Error on line " << errorLine << "Column " << errorColumn << " Message"  << errorMsg;
        return 0;
    }
}

ProfileActionTemplate ProfileServer::ProfileXmlContentHandler::parseAction(QDomNode actionNode, const QString& profileId) {
    QString buttonName;
    bool autostart = true;
    bool repeat = false;
    QString actionId = actionNode.attributes().namedItem(QLatin1String( "id" )).nodeValue().trimmed();
    if (!actionNode.namedItem(QLatin1String( "button" )).isNull()){
        buttonName = actionNode.namedItem(QLatin1String( "button" )).toElement().text().trimmed();
    }
    if (!actionNode.namedItem(QLatin1String( "autostart" )).isNull()){
        QString value = actionNode.namedItem(QLatin1String( "autostart" )).toElement().text().trimmed();
        autostart = value == QLatin1String( "true" )? true: false;
    }
    if (!actionNode.namedItem(QLatin1String( "repeat" )).isNull()){
        repeat = QVariant(actionNode.namedItem(QLatin1String( "repeat" )).toElement().text().trimmed()).toBool();
    }

    QString actionName = actionNode.attributes().namedItem(QLatin1String( "name" )).nodeValue().trimmed();

    QString description = actionNode.attributes().namedItem(QLatin1String( "description" )).nodeValue().trimmed();

    DBusAction::ActionDestination actionType;
    QString ifMultiTag = actionNode.namedItem(QLatin1String( "ifmulti" )).toElement().text().trimmed();
    if(ifMultiTag == QLatin1String( "sendtotop" )){
        actionType = DBusAction::Top;
    } else if(ifMultiTag == QLatin1String( "sendtobottom" )) {
        actionType = DBusAction::Bottom;
    } else if(ifMultiTag == QLatin1String( "sendtoall" )) {
        actionType = DBusAction::All;
    } else if(ifMultiTag == QLatin1String( "dontsend" )) {
        actionType = DBusAction::None;
    } else {
        actionType = DBusAction::Unique;
    }

    QDomElement execNode;
    QString serviceName;
    QString nodeName;
    QString interfaceName;
    QString functionName;
    if(!actionNode.namedItem(QLatin1String( "prototype" )).isNull()){
        execNode = actionNode.namedItem(QLatin1String( "prototype" )).toElement();
        interfaceName = execNode.namedItem(QLatin1String( "interface" )).toElement().text().trimmed();
        functionName = execNode.namedItem(QLatin1String( "function" )).toElement().text().trimmed();
    } else if(!actionNode.namedItem(QLatin1String( "script" )).isNull()){
        execNode = actionNode.namedItem(QLatin1String( "script" )).toElement();
        functionName = "script:" + execNode.namedItem(QLatin1String( "scripttext" )).toElement().text().trimmed();
    } else {
        kDebug() << "neither prototype nor script present... discarding action" << actionName;
    }
    serviceName = execNode.namedItem(QLatin1String( "serviceName" )).toElement().text().trimmed();
    nodeName = execNode.namedItem(QLatin1String( "node" )).toElement().text().trimmed();
    

    QList<Argument> arguments;
    if(!execNode.namedItem(QLatin1String( "arguments" )).isNull()){
        QDomNodeList attributeNodes = execNode.namedItem(QLatin1String( "arguments" )).toElement().elementsByTagName(QLatin1String( "argument" ));
        for(int attributeCount = 0; attributeCount < attributeNodes.size(); ++ attributeCount){
            QDomNode attributeNode = attributeNodes.at(attributeCount);
            QString typeString  = attributeNode.attributes().namedItem(QLatin1String( "type" )).nodeValue().trimmed();
            QVariant argValue(QVariant::nameToType(typeString.toLocal8Bit()));
            QString description = attributeNode.toElement().attributes().namedItem(QLatin1String( "comment" )).nodeValue().trimmed();
            if(!attributeNode.toElement().namedItem(QLatin1String( "default" )).isNull()){
                QVariant qVariant = QVariant::nameToType(typeString.toLocal8Bit());
                QString value = attributeNode.toElement().namedItem(QLatin1String( "default" )).toElement().text().trimmed();
                if(argValue.type() == QVariant::StringList){
                    QStringList stringList;
                    foreach(const QString &tListValue, value.split(QLatin1Char( ',' ), QString::SkipEmptyParts)){
                        stringList << tListValue.trimmed();
                    }
                    argValue = QVariant(stringList);
                } else {
                    argValue = QVariant(value);
                }
                argValue.convert(QVariant::nameToType(typeString.toLocal8Bit()));
            }
            arguments.append(Argument(argValue, i18n(description.toUtf8().data())));
        }
    }

    Prototype function(functionName, arguments);
    
    kDebug() << "creating action template" << actionName << serviceName << nodeName << interfaceName << functionName;

    return ProfileActionTemplate(profileId,
                                  actionId,
                                  i18n(actionName.toUtf8().data()),
                                  serviceName,
                                  nodeName,
                                  interfaceName,
                                  function,
                                  actionType,
                                  autostart,
                                  repeat,
                                  i18n(description.toUtf8().data()),
                                  buttonName);
}
