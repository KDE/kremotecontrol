/*************************************************************************
 * Copyright: (C) 2009 Michael Zanetti <michael_zanetti@gmx.net          *
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
/*
 * dbusinterface.cpp
 *
 *  Created on: 14.02.2009
 *      Author: Michael Zanetti
 */



#include "dbusinterface.h"

#include <QStringList>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDomDocument>

#include <QtDBus/qdbusconnection.h>

#include <kdebug.h>
#include <kwindowsystem.h>
#include <knotification.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>

  static DBusInterface *theInstance = NULL;


DBusInterface::DBusInterface() {
}

DBusInterface *DBusInterface::getInstance() {
    if (!theInstance) {
        theInstance = new DBusInterface();
    }
    return theInstance;
}


DBusInterface::~DBusInterface() {
    //delete(DBusInterface::theInstance);
    //DBusInterface::theInstance = 0;
}

QStringList DBusInterface::getAllRegisteredPrograms() {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    return dBusIface->registeredServiceNames();
}


QStringList DBusInterface::getRegisteredPrograms() {
    QStringList returnList;

    QStringList allServices = getAllRegisteredPrograms();

    //Throw out invalid entries
    for (int i = 0; i < allServices.size(); ++i) {

        QString tmp = allServices.at(i);

        QRegExp r1("[a-zA-Z]{1,3}\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9_-]+");
        if (!r1.exactMatch(tmp)) {
            continue;
        }
        if (getNodes(tmp).isEmpty()) {
            continue;
        }
        QRegExp r2("[a-zA-Z0-9_\\.-]+-[0-9]+");
        if (r2.exactMatch(tmp)) {
            tmp.truncate(tmp.lastIndexOf('-'));
        }
        if (!returnList.contains(tmp)) {
            returnList << tmp;
        }

    }

    return returnList;
}

QStringList DBusInterface::getNodes(const QString &program) {
    kDebug() << "getting Nodes of" << program;
    QDBusInterface dBusIface(program, "/", "org.freedesktop.DBus.Introspectable");
    QDBusMessage msg = QDBusMessage::createMethodCall(program, "/", "org.freedesktop.DBus.Introspectable", "Introspect");
    QDBusReply<QString> response = dBusIface.connection().call(msg, QDBus::Block, 1);

    QDomDocument domDoc;
    domDoc.setContent(response);
    if (domDoc.toString().isEmpty()) { // No reply... perhaps a multi-instance...
        kDebug() << "no reply from" << program;
        QStringList instances = getAllRegisteredPrograms().filter(program);
        if (!instances.isEmpty()) {
            QDBusInterface iFace(instances.first(), "/", "org.freedesktop.DBus.Introspectable");
            QDBusMessage msg = QDBusMessage::createMethodCall(instances.first(), "/", "org.freedesktop.DBus.Introspectable", "Introspect");
            QDBusReply<QString> response = iFace.connection().call(msg, QDBus::Block, 1);
//            response = iFace.call("Introspect");
            domDoc.setContent(response);
        }
    }
    kDebug() << "got Nodes of" << program;

    QDomElement node = domDoc.documentElement();

    QDomElement child = node.firstChildElement();
    QStringList returnList;
    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("node")) {
            QString name = child.attribute(QLatin1String("name"));
            if (name != "org" && name != "modules" && !getFunctions(program, name).isEmpty()) {
                returnList << name;
            }
        }
        child = child.nextSiblingElement();
    }
    return returnList;
}

QList<Prototype> DBusInterface::getFunctions(const QString &program, const QString &object) {
    QDBusInterface dBusIface(program, '/' + object, "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface.call("Introspect");

    QDomDocument domDoc;
    domDoc.setContent(response);

    if (domDoc.toString().isEmpty()) { // No reply... perhaps a multi-instance...
        QStringList instances = getAllRegisteredPrograms().filter(program);
        if (!instances.isEmpty()) {
            QDBusInterface iFace(instances.first(), '/' + object, "org.freedesktop.DBus.Introspectable");
            response = iFace.call("Introspect");
            domDoc.setContent(response);
        }
    }

    QDomElement node = domDoc.documentElement();
    QDomElement child = node.firstChildElement();

    QList<Prototype> funcList;

    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("interface")) {
            if (child.attribute("name") == "org.freedesktop.DBus.Properties" ||
                    child.attribute("name") == "org.freedesktop.DBus.Introspectable") {
                child = child.nextSiblingElement();
                continue;
            }
            QDomElement subChild = child.firstChildElement();
            while (!subChild.isNull()) {
                if (subChild.tagName() == QLatin1String("method")) {
                    QString functionName = subChild.attribute(QLatin1String("name"));
                    QDomElement argDom = subChild.firstChildElement();
                    QList<Argument> argList;
                    while (!argDom.isNull()) {
                        Argument argument;
                        if (argDom.tagName() == QLatin1String("arg")) {
                            QString tmpArg = argDom.attribute(QLatin1String("type"));
                            if (tmpArg == "i") {
                                argument.setValue(QVariant::Int);
                            } else if (tmpArg == "u") {
                                argument.setValue(QVariant::UInt);
                            } else if (tmpArg == "x") {
                                argument.setValue(QVariant::LongLong);
                            } else if (tmpArg == "s") {
                                argument.setValue(QVariant::String);
                            } else if (tmpArg == "b") {
                                argument.setValue(QVariant::Bool);
                            } else if (tmpArg == "d") {
                                argument.setValue(QVariant::Double);
                            } else if (tmpArg == "as") {
                                argument.setValue(QVariant::StringList);
                            } else if (tmpArg == "ay") {
                                argument.setValue(QVariant::ByteArray);
                            } else {
                                argDom = argDom.nextSiblingElement();
                                continue;
                            }

                            if (argDom.attribute(QLatin1String("direction")) == "in") {
                                if (!argDom.attribute(QLatin1String("name")).isEmpty()) {
                                    argument.setDescription(argDom.attribute(QLatin1String("name")));
                                } else {
                                    argument.setDescription(i18nc("The name of a parameter", "unknown"));
                                }
                                argList.append(argument);
                            }
                        }
                        argDom = argDom.nextSiblingElement();
                    }
                    Prototype function(functionName, argList);
                    if(!funcList.contains(function)){
                        funcList.append(function);
                    }
                }
                subChild = subChild.nextSiblingElement();
            }
        }
        child = child.nextSiblingElement();
    }
    return funcList;
}

QStringList DBusInterface::getConfiguredRemotes() {
    QStringList remotes;
      QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "getConfiguredRemotes");    
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    } else {
        remotes = response.arguments().at(0).toStringList();
    }
    return remotes;
}

void DBusInterface::considerButtonEvents(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "considerButtonEvents");
    m.arguments().append(remoteName);
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void DBusInterface::ignoreButtonEvents(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "ignoreButtonEvents");  
    m.arguments().append(remoteName);
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void DBusInterface::reloadRemoteControlDaemon() {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "reloadConfiguration");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }

}

bool DBusInterface::isProgramRunning(const QString &program) {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    if (dBusIface->isServiceRegistered(program)) {
        return true;
    }
    return false;
}

bool DBusInterface::isUnique(const QString &program){
    QStringList instances = getAllRegisteredPrograms().filter(program);
    kDebug() << "instances of " + program << instances;

    // If there are more than 1 it is oviously not unique
    // If there are 0 we cannot know... Return false so the user can at least try to specify what he likes.
    if(instances.count() != 1){
	return false;
    }

    // So... we have exactly one instance...
    // check if there are any trailing numbers. If yes, it is a multi-instance-app
    QRegExp r2("[a-zA-Z0-9_\\.-]+-[0-9]+");
    if(r2.exactMatch(instances.first())){
	return false;
    }

    return true;
}

bool DBusInterface::searchForProgram(const DBusAction *action, QStringList &programs) {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    programs.clear();

    if (action->destination() == DBusAction::Unique) {
        QString service = action->application();
        
        kDebug() << "searching for prog:" << service;
        if (dBusIface->isServiceRegistered(service)) {
            kDebug() << "adding Program: " << service;
            programs += service;
        } else {
            kDebug() << "nope... " + service + " not here.";
        }
    } else {

        // find all instances...
        const QStringList buf = dBusIface->registeredServiceNames();

        for (QStringList::const_iterator i = buf.constBegin(); i != buf.constEnd(); ++i) {
            QString program = *i;
            if (program.contains(action->application()))
                programs += program;
        }

        if (programs.size() == 1) {
            kDebug() << "Yeah! found it!";
        } else if (programs.size() == 0) {
            kDebug() << "Nope... not here...";
        } else {
            kDebug() << "found multiple instances...";
        }

        if (programs.size() > 1 && action->destination() == DBusAction::None) {
            kDebug() << "Multiple instances of" << action->application() << "found but destination is set to None";
            return false;
        } else if (programs.size() > 1 && action->destination() == DBusAction::Top) {
            ;
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); i++) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action->application() + '-' + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeFirst();
        } else if (programs.size() > 1 && action->destination() == DBusAction::Bottom) {
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); ++i) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action->application() + '-' + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeFirst();
        }
    }
    kDebug() << "returning true";
    return true;
}


void DBusInterface::executeAction(const DBusAction* action) {
    kDebug() << "executeAction called";
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();

    QStringList programs;

    if (!searchForProgram(action, programs)) {
        return;
    }

    // if programs.size()==0 here, then the app is definately not running.
    kDebug() << "Autostart: " << action->autostart();
    kDebug() << "programs.size: " << programs.size();
    if (action->autostart() && !programs.size()) {
        kDebug() << "Should start " << action->application();
        QString runCommand = action->application();
        runCommand.remove(QRegExp("org.[a-zA-Z0-9]*."));
        kDebug() << "runCommand" << runCommand;
        KToolInvocation::startServiceByDesktopName(runCommand);
    }
    if (action->function().name().isEmpty()) // Just start
        return;

    if (!searchForProgram(action, programs)) {
        kDebug() << "Failed to start the application" << action->application();
        return;
    }

    for (QStringList::iterator i = programs.begin(); i != programs.end(); ++i) {
        const QString &program = *i;
        kDebug() << "Searching DBus for program:" << program;
        if (dBusIface->isServiceRegistered(program)) {
            kDebug() << "Sending data (" << program << ", " << '/' + action->node() << ", " << action->function().name();

            QDBusMessage m = QDBusMessage::createMethodCall(program, '/'
                             + action->node(), "", action->function().name());

            foreach(const Argument &arg, action->function().args()){
                kDebug() << "Got argument:" << arg.value().type() << "value" << arg.value();
                m << arg.value();
            }
            //   theDC->send(program.utf8(), action.object().utf8(), action.method().prototypeNR().utf8(), data);
            QDBusMessage response = QDBusConnection::sessionBus().call(m);
            if (response.type() == QDBusMessage::ErrorMessage) {
                kDebug() << response.errorMessage();
            }
        }
    }  
}


void DBusInterface::changeMode(const QString& remoteName, const QString& modeName) {    
      QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "getConfiguredRemotes");    		      
    m << remoteName;
    m << modeName;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if(reply.isValid()){      
      if(!reply){
	kDebug() << "Couldn't change to mode " << modeName << " on remote " << remoteName;
      }
    }else{
        kDebug() << response.errorMessage();
    }
}

QString DBusInterface::getCurrentMode(const QString& remoteName) {
  QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "getCurrentMode");    
     m << remoteName;
    QDBusReply<QString> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
      return reply;
    } else {
      kDebug() << reply.error().message();  
      return QString();
    }
}

QStringList DBusInterface::getModesForRemote(const QString& remoteName) {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "getModesForRemote");    
    m << remoteName;
    QDBusReply<QStringList> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
        return reply;
    } else {
          kDebug() << reply.error().message();
	  return QStringList();
    }
}

bool DBusInterface::eventsIgnored(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall("org.kde.kded", "/modules/kremotecontrol",
                     "org.kde.krcd", "eventsIgnored");    
     m << remoteName;
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
      return reply;
    } else {
      kDebug() << reply.error().message();  
      return false;
    }
}
