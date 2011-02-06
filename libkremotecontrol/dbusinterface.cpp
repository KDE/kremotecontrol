/*************************************************************************
 * Copyright (C) 2009 Michael Zanetti <michael_zanetti@gmx.net           *
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

#include <kdebug.h>
#include <kwindowsystem.h>
#include <knotification.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>

#include <QtCore/QStringList>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QtDBus/qdbusconnection.h>
#include <QtXml/QDomDocument>
#include <QScriptEngine>

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

QStringList DBusInterface::allRegisteredPrograms() {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    return dBusIface->registeredServiceNames();
}

QStringList DBusInterface::registeredPrograms() {
    QStringList returnList;

    QStringList allServices = allRegisteredPrograms();

    //Throw out invalid entries
    for (int i = 0; i < allServices.size(); ++i) {
        QString tmp = allServices.at(i);

        QRegExp r1( QLatin1String( "[a-zA-Z]{1,3}\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9_-]+" ));
        if (!r1.exactMatch(tmp)) {
            continue;
        }
        if (nodes(tmp).isEmpty()) {
            continue;
        }
        QRegExp r2( QLatin1String( "[a-zA-Z0-9_\\.-]+-[0-9]+" ));
        if (r2.exactMatch(tmp)) {
            tmp.truncate(tmp.lastIndexOf( QLatin1Char( '-' )));
        }
        if (!returnList.contains(tmp)) {
            returnList << tmp;
        }

    }

    return returnList;
}

QStringList DBusInterface::nodes(const QString &program) {
    kDebug() << "getting Nodes of" << program;
    QDBusInterface dBusIface(program, QLatin1String( "/" ), QLatin1String( "org.freedesktop.DBus.Introspectable" ));
    QDBusMessage msg = QDBusMessage::createMethodCall(program, QLatin1String( "/" ), QLatin1String( "org.freedesktop.DBus.Introspectable" ), QLatin1String( "Introspect" ));
    QDBusReply<QString> response = dBusIface.connection().call(msg, QDBus::Block, 1);

    QDomDocument domDoc;
    domDoc.setContent(response);
    if (domDoc.toString().isEmpty()) { // No reply... perhaps a multi-instance...
        kDebug() << "no reply from" << program;
        QStringList instances = allRegisteredPrograms().filter(program);
        if (!instances.isEmpty()) {
            QDBusInterface iFace(instances.first(), QLatin1String( "/" ), QLatin1String( "org.freedesktop.DBus.Introspectable" ));
            QDBusMessage msg = QDBusMessage::createMethodCall(instances.first(), QLatin1String( "/" ), QLatin1String( "org.freedesktop.DBus.Introspectable" ), QLatin1String( "Introspect" ));
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
            if (name != QLatin1String( "org" ) && name != QLatin1String( "modules" ) && !functions(program, name).isEmpty()) {
                returnList << name;
            }
        }
        child = child.nextSiblingElement();
    }
    return returnList;
}

QList<Prototype> DBusInterface::functions(const QString &program, const QString &object) {
    QDBusInterface dBusIface(program, QLatin1Char( '/' ) + object, QLatin1String( "org.freedesktop.DBus.Introspectable" ));
    QDBusReply<QString> response = dBusIface.call(QLatin1String( "Introspect" ));

    QDomDocument domDoc;
    domDoc.setContent(response);

    if (domDoc.toString().isEmpty()) { // No reply... perhaps a multi-instance...
        QStringList instances = allRegisteredPrograms().filter(program);
        if (!instances.isEmpty()) {
            QDBusInterface iFace(instances.first(), QLatin1Char( '/' ) + object, QLatin1String( "org.freedesktop.DBus.Introspectable" ));
            response = iFace.call(QLatin1String( "Introspect" ));
            domDoc.setContent(response);
        }
    }

    QDomElement node = domDoc.documentElement();
    QDomElement child = node.firstChildElement();

    QList<Prototype> funcList;

    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("interface")) {
            if (child.attribute(QLatin1String( "name" )) == QLatin1String( "org.freedesktop.DBus.Properties" ) ||
                    child.attribute(QLatin1String( "name" )) == QLatin1String( "org.freedesktop.DBus.Introspectable" )) {
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
                            if (tmpArg == QLatin1String( "i" )) {
                                argument.setValue(QVariant::Int);
                            } else if (tmpArg == QLatin1String( "u" )) {
                                argument.setValue(QVariant::UInt);
                            } else if (tmpArg == QLatin1String( "x" )) {
                                argument.setValue(QVariant::LongLong);
                            } else if (tmpArg == QLatin1String( "s" )) {
                                argument.setValue(QVariant::String);
                            } else if (tmpArg == QLatin1String( "b" )) {
                                argument.setValue(QVariant::Bool);
                            } else if (tmpArg == QLatin1String( "d" )) {
                                argument.setValue(QVariant::Double);
                            } else if (tmpArg == QLatin1String( "as" )) {
                                argument.setValue(QVariant::StringList);
                            } else if (tmpArg == QLatin1String( "ay" )) {
                                argument.setValue(QVariant::ByteArray);
                            } else {
                                argDom = argDom.nextSiblingElement();
                                continue;
                            }

                            if (argDom.attribute(QLatin1String("direction")) == QLatin1String( "in" )) {
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

QStringList DBusInterface::configuredRemotes() {
    QStringList remotes;
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "configuredRemotes" ));
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    } else {
        remotes = response.arguments().at(0).toStringList();
    }
    return remotes;
}

void DBusInterface::considerButtonEvents(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "considerButtonEvents" ));
    m << remoteName;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void DBusInterface::ignoreButtonEvents(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "ignoreButtonEvents" ));
    m << remoteName;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void DBusInterface::reloadRemoteControlDaemon() {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "reloadConfiguration" ));
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
    QStringList instances = allRegisteredPrograms().filter(program);
    kDebug() << "instances of " << program << instances;

    // If there are more than 1 it is oviously not unique
    // If there are 0 we cannot know... Return false so the user can at least try to specify what he likes.
    if(instances.count() != 1){
	return false;
    }

    // So... we have exactly one instance...
    // check if there are any trailing numbers. If yes, it is a multi-instance-app
    QRegExp r2( QLatin1String( "[a-zA-Z0-9_\\.-]+-[0-9]+" ));
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
            kDebug() << "nope... " << service << " not here.";
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
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); i++) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action->application() + QLatin1Char( '-' ) + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeLast();
        } else if (programs.size() > 1 && action->destination() == DBusAction::Bottom) {
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); ++i) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action->application() + QLatin1Char( '-' ) + QString().setNum(p);
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
        runCommand.remove(QRegExp( QLatin1String( "org.[a-zA-Z0-9]*." )));
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
            kDebug() << "Sending data (" << program << ", " << QLatin1Char( '/' ) + action->node() << ", " << action->function().name();

            if(action->function().name().startsWith("script:")) {
                QString scriptText = action->function().name().remove(0, 7);
                
                QScriptEngine scriptEngine;
                QDBusIfaceWrapper *appIface = new QDBusIfaceWrapper(program, QLatin1Char( '/' ) + action->node());
                QScriptValue objectValue = scriptEngine.newQObject(appIface);
                scriptEngine.globalObject().setProperty("dbus", objectValue);
                int argCount = 1;
                foreach(const Argument &arg, action->function().args()) {
                    QScriptValue argValue;
                    switch(arg.value().type()) {
                    case QVariant::Int:
                    case QVariant::LongLong:
                        argValue = QScriptValue(arg.value().toInt());
                        break;
                    case QVariant::UInt:
                        argValue = QScriptValue(arg.value().toUInt());
                        break;
                    case QVariant::Double:
                        argValue = QScriptValue(arg.value().toDouble());
                        break;
                    case QVariant::Bool:
                        argValue = QScriptValue(arg.value().toBool());
                        break;
                    case QVariant::StringList:
                        argValue = QScriptValue(arg.value().toStringList().join(","));
                        break;
                    case QVariant::ByteArray:
                    case QVariant::String:
                        argValue = QScriptValue(arg.value().toString());
                    default:
                        argValue = QScriptValue(arg.value().toInt());
                    }

                    scriptEngine.globalObject().setProperty("arg" + QString::number(argCount++), argValue);
                }
                qDebug() << "its a script:" << scriptText;
                scriptEngine.evaluate(scriptText);
            } else {
            
                QDBusMessage m = QDBusMessage::createMethodCall(program, QLatin1Char( '/' )
                             + action->node(), QLatin1String( "" ), action->function().name());

                foreach(const Argument &arg, action->function().args()){
                    kDebug() << "Got argument:" << arg.value().type() << "value" << arg.value();
                    m << arg.value();
                }
                QDBusMessage response = QDBusConnection::sessionBus().call(m);
                if (response.type() == QDBusMessage::ErrorMessage) {
                    kDebug() << response.errorMessage();
                }
            }
        }
    }
}

void DBusInterface::changeMode(const QString& remoteName, const QString& modeName) {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "changeMode" ));
    m << remoteName;
    m << modeName;
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if(!reply.isValid()){
        kDebug() << "Couldn't change to mode " << modeName << " on remote " << remoteName;
    }
}

QString DBusInterface::currentMode(const QString& remoteName) {
  QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "currentMode" ));
     m << remoteName;
    QDBusReply<QString> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
      return reply;
    } else {
      kDebug() << reply.error().message();
      return QString();
    }
}

QStringList DBusInterface::modesForRemote(const QString& remoteName) {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "modesForRemote" ));
    m << remoteName;
    QDBusReply<QStringList> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
        return reply;
    } else {
        kDebug() << reply.error().message();
        return QStringList();
    }
}

QString DBusInterface::modeIcon(const QString& remoteName, const QString& modeName) {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "modeIcon" ));
    m << remoteName;
    m << modeName;
    QDBusReply<QString> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
        return reply;
    } else {
        kDebug() << reply.error().message();
        return QLatin1String( "" );
    }

}

bool DBusInterface::eventsIgnored(const QString& remoteName) {
   QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ),
                     QLatin1String( "org.kde.krcd" ), QLatin1String( "eventsIgnored" ));
     m << remoteName;
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if (reply.isValid()) {
      return reply;
    } else {
      kDebug() << reply.error().message();
      return false;
    }
}

bool DBusInterface::isKdedModuleRunning() {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/kded" ), QLatin1String( "org.kde.kded" ), QLatin1String( "loadedModules" ));
    QDBusReply<QStringList> reply = QDBusConnection::sessionBus().call(m);
    if(reply.isValid()){
        return reply.value().contains(QLatin1String( "kremotecontroldaemon" ));
    }
    kDebug() << reply.error().message();
    return false;
}

bool DBusInterface::loadKdedModule() {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/kded" ), QLatin1String( "org.kde.kded" ), QLatin1String( "loadModule" ));
    m << QLatin1String( "kremotecontroldaemon" );
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if(!reply.isValid() || !reply.value()){
        return false;
    }

    m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/kded" ), QLatin1String( "org.kde.kded" ), QLatin1String( "setModuleAutoloading" ));
    m << QLatin1String( "kremotecontroldaemon" ) << true;
    QDBusConnection::sessionBus().call(m);
    return true;
}

bool DBusInterface::unloadKdedModule() {
    QDBusMessage m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/kded" ), QLatin1String( "org.kde.kded" ), QLatin1String( "unloadModule" ));
    m << QLatin1String( "kremotecontroldaemon" );
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(m);
    if(!reply.isValid() || !reply.value()){
        return false;
    }

    m = QDBusMessage::createMethodCall(QLatin1String( "org.kde.kded" ), QLatin1String( "/kded" ), QLatin1String( "org.kde.kded" ), QLatin1String( "setModuleAutoloading" ));
    m << QLatin1String( "kremotecontroldaemon" ) << false;
    QDBusConnection::sessionBus().call(m);
    return true;
}

QDBusIfaceWrapper::QDBusIfaceWrapper(const QString &program, const QString &path):
    m_program(program),
    m_path(path)
{

}

int QDBusIfaceWrapper::call(const QString& method)
{
    QDBusInterface iface(m_program, m_path);
    qDebug() << "calling" << method;
    QDBusReply<int> ret = iface.call(method);
    qDebug() << "return value is" << ret.value();
    return ret;
}

int QDBusIfaceWrapper::call(const QString& method, int value)
{
    QDBusInterface iface(m_program, m_path);
    qDebug() << "calling" << method << "with arg" << value;
    QDBusReply<int> ret = iface.call(method, value);
    qDebug() << "return value is" << ret.value();
    return ret;
}

int QDBusIfaceWrapper::call(const QString& method, const QString &value)
{
    QDBusInterface iface(m_program, m_path);
    qDebug() << "calling" << method << "with arg" << value;
    QDBusReply<int> ret = iface.call(method, value);
    qDebug() << "return value is" << ret.value();
    return ret;
}
