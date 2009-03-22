/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood
  */

#include "profileserver.h"

#include <QFile>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>


ProfileServer *ProfileServer::theInstance = 0;

ProfileServer::ProfileServer()
{
// theProfiles.setAutoDelete(true);
    loadProfiles();
}

ProfileServer::~ProfileServer()
{
}

void ProfileServer::loadProfiles()
{
    QStringList theFiles = KGlobal::dirs()->findAllResources("data", "profiles/*.profile.xml");
    for (QStringList::iterator i = theFiles.begin(); i != theFiles.end(); ++i) {
        kDebug() << "Found data file: " << *i ;
        Profile *p = new Profile();
        p->loadFromFile(*i);
        theProfiles.append(p);
    }
}

const Profile* ProfileServer::getProfileById(const QString& profileId) const{
  foreach(Profile *prof, theProfiles){
    if(profileId == prof->id()){
      return prof;
    }
  }
  return NULL;
}


Profile::Profile()
{
    // set up defaults
    theUnique = true;
    theIfMulti = IM_DONTSEND;

// theActions.setAutoDelete(true);
}

const ProfileAction *Profile::searchClass(const QString &c) const
{
	kDebug() << "Actionscount: " << theActions.size();
    for (QHash<QString, ProfileAction*>::const_iterator i = theActions.constBegin(); i != theActions.constEnd(); ++i) {
        kDebug() << "value:" << i.value();
        kDebug() << "Class: " << i.value()->getClass();
        if (i.value()->getClass() == c) {
            return i.value();
        }
    }
    return 0;
}

void Profile::loadFromFile(const QString &fileName)
{
    charBuffer = "";
    curPA = 0;
    curPAA = 0;

    QFile xmlFile(fileName);
    QXmlInputSource source(&xmlFile);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.parse(source);
}

const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &actionId) const
{
    kDebug() << "Profile to search:" << appId << actionId;
    if (getProfileById(appId)){
        if (getProfileById(appId)->actions()[actionId]){
            return getProfileById(appId)->actions()[actionId];
	}
    }
    return 0;
}

const QString &ProfileServer::getServiceName(const QString &appId) const
{
    if (getProfileById(appId)){
        return getProfileById(appId)->serviceName();
    }
    //TODO: FIX this
    return QString();
}

const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &objId, const QString &prototype) const
{
    return getAction(appId, objId + "::" + prototype);
}

bool Profile::characters(const QString &data)
{
    charBuffer += data;
    return true;
}

bool Profile::startElement(const QString &, const QString &, const QString &name, const QXmlAttributes &attributes)
{
    if (name == "profile") {
        theId = attributes.value("id");
        theServiceName = attributes.value("servicename");
    } else if (name == "action") {
        curPA = new ProfileAction;
        curPA->setObjId(attributes.value("objid"));
        curPA->setPrototype(attributes.value("prototype"));
        curPA->setClass(attributes.value("class"));
        kDebug() << "adding action class: " << curPA->getClass();
        curPA->setMultiplier(attributes.value("multiplier").isEmpty() ? 1.0 : attributes.value("multiplier").toFloat());
        curPA->setRepeat(attributes.value("repeat") == "1");
        curPA->setAutoStart(attributes.value("autostart") == "1");
    } else if (name == "instances") {
        theUnique = attributes.value("unique") == "1";
        theIfMulti = attributes.value("ifmulti") == "sendtotop" ? IM_SENDTOTOP : attributes.value("ifmulti") == "sendtobottom" ? IM_SENDTOBOTTOM : attributes.value("ifmulti") == "sendtoall" ? IM_SENDTOALL : IM_DONTSEND;
    } else if (name == "argument") {
        curPA->theArguments.append(ProfileActionArgument());
        curPAA = &(curPA->theArguments.last());
        curPAA->setAction(curPA);
        curPAA->setType(attributes.value("type"));
        curPAA->setDefault(attributes.value("default"));
    } else if (name == "range" && curPAA)
        curPAA->setRange(qMakePair(attributes.value("min").toInt(), attributes.value("max").toInt()));

    charBuffer = "";
    return true;
}

bool Profile::endElement(const QString &, const QString &, const QString &name)
{
    if (name == "name")
        if (curPA)
            curPA->setName(charBuffer);
        else
            theName = charBuffer;
    else if (name == "author")
        theAuthor = charBuffer;
    else if (name == "comment" && curPA && !curPAA)
        curPA->setComment(charBuffer);
    else if (name == "default" && curPA && curPAA)
        curPAA->setDefault(charBuffer);
    else if (name == "comment" && curPA && curPAA)
        curPAA->setComment(charBuffer);
    else if (name == "action") {
        curPA->setProfile(this);
        theActions.insert(curPA->objId() + "::" + curPA->prototype(), curPA);
    	kDebug() << "theActions"  << theActions;
        curPA = 0;
    } else if (name == "argument")
        curPAA = 0;

    charBuffer = "";
    return true;
}
