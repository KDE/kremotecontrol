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
//
#include <QFile>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>


KDE_EXPORT ProfileServer *ProfileServer::theInstance = 0;

KDE_EXPORT ProfileServer::ProfileServer()
{
// theProfiles.setAutoDelete(true);
    loadProfiles();
}

KDE_EXPORT ProfileServer::~ProfileServer()
{
}

KDE_EXPORT void ProfileServer::loadProfiles()
{
    QStringList theFiles = KGlobal::dirs()->findAllResources("data", "profiles/*.profile.xml");
    for (QStringList::iterator i = theFiles.begin(); i != theFiles.end(); ++i) {
        Profile *p = new Profile();
        p->loadFromFile(*i);
        theProfiles.append(p);
    }
}

KDE_EXPORT const Profile* ProfileServer::getProfileById(const QString& profileId) const {
    foreach(Profile *prof, theProfiles) {
        if (profileId == prof->id()) {
            return prof;
        }
    }
    return NULL;
}


KDE_EXPORT Profile::Profile()
{
    // set up defaults
    theUnique = true;
    theIfMulti = IM_DONTSEND;

// theActions.setAutoDelete(true);
}

KDE_EXPORT Profile::~Profile()
{
}

KDE_EXPORT const ProfileAction *Profile::searchClass(const QString &buttonName) const
{
    for (QHash<QString, ProfileAction*>::const_iterator i = theActions.constBegin(); i != theActions.constEnd(); ++i) {
        if (i.value()->getClass()  == buttonName) {
            return i.value();
        }
    }
    return 0;
}


KDE_EXPORT const ProfileAction *Profile::getProfileActionByButton(const QString &c) const
{
    for (QHash<QString, ProfileAction*>::const_iterator i = theActions.constBegin(); i != theActions.constEnd(); ++i) {
        if (i.value()->buttonName() == c) {
            return i.value();
        }
    }
    return 0;
}


KDE_EXPORT void Profile::loadFromFile(const QString &fileName)
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

KDE_EXPORT const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &actionId) const
{
    const Profile *profile = getProfileById(appId);
    if (profile) {
        if (profile->actions()[actionId]) {
            return profile->actions()[actionId];
        }
    }
    return 0;
}

KDE_EXPORT const QString ProfileServer::getServiceName(const QString &appId) const
{
    const Profile *profile = getProfileById(appId);
    if (profile) {
        return profile->serviceName();
    }
    return QString();
}

KDE_EXPORT const ProfileAction *ProfileServer::getAction(const QString &appId, const QString &objId, const QString &prototype) const
{
    return getAction(appId, objId + "::" + prototype);
}

KDE_EXPORT bool Profile::characters(const QString &data)
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
        kDebug() << "loading function:" << attributes.value("prototype");
        curPA->setButtonName(attributes.value("button"));
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
        QVariant tmpArg(QVariant::nameToType(attributes.value("type").toLocal8Bit()));
//        tmpArg.convert();
        kDebug() << "***********************************";
        kDebug() << "type:" << attributes.value("type");
        kDebug() << "tmpArg:" << tmpArg;
        curPAA->setDefault(tmpArg);
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
    else if (name == "default" && curPA && curPAA) {
        if (curPAA->theDefault.type() == QVariant::Int) {
            curPAA->theDefault.setValue(charBuffer.toInt());
        } else if (curPAA->theDefault.type() == QVariant::UInt) {
            curPAA->theDefault.setValue(charBuffer.toUInt());
        } else if (curPAA->theDefault.type() == QVariant::Bool) {
            curPAA->theDefault.setValue(charBuffer == "true" ? true : false);
        } else if (curPAA->theDefault.type() == QVariant::Double) {
            curPAA->theDefault.setValue(charBuffer.toDouble());
        } else if (curPAA->theDefault.type() == QVariant::StringList) {
            curPAA->theDefault.setValue(charBuffer.split(','));
        } else if (curPAA->theDefault.type() == QVariant::ByteArray) {
            curPAA->theDefault.setValue(charBuffer.toLocal8Bit());
        } else {
            curPAA->theDefault.setValue(charBuffer);
        }

    } else if (name == "comment" && curPA && curPAA)
        curPAA->setComment(charBuffer);
    else if (name == "action") {
        curPA->setProfile(this);
        theActions.insert(curPA->objId() + "::" + curPA->prototype(), curPA);
        curPA = 0;
    } else if (name == "argument")
        curPAA = 0;

    charBuffer = "";
    return true;
}


KDE_EXPORT const QStringList ProfileServer::getAllButtonNamesById(const QString& profileId) const
{
    QStringList tReturn;
    const Profile *profile = getProfileById(profileId);
    if (profile) {
        foreach(const ProfileAction *profileAction, profile->actions().values()) {
            if (! profileAction->buttonName().isEmpty()) {
                tReturn << profileAction->buttonName();
            }
        }
    }
    return tReturn;
}


KDE_EXPORT ProfileServer::ProfileSupportedByRemote ProfileServer::isProfileAvailableForRemote(const Profile *profile, const QStringList &solidButtonNames)
{
    QStringList tProfilActionNames;

    foreach(const ProfileAction *profileAction, profile->actions().values()) {
        if (! profileAction->buttonName().isEmpty()) {
            tProfilActionNames << profileAction->buttonName();
        }
    }
    if (tProfilActionNames.size() == 0) {
        return ProfileServer::NO_ACTIONS_DEFINED;
    }
    int found=0;

    foreach(const QString tProfilActionName, tProfilActionNames) {
        if ( solidButtonNames.contains(tProfilActionName)) {
            found++;
        }
    }
    if (found == 0) {
        return ProfileServer::NOT_SUPPORTED;
    } else if (found != tProfilActionNames.size()) {
        return ProfileServer::PARTIAL_SUPPORTED;
    }
    return ProfileServer::FULL_SUPPORTED;
}
