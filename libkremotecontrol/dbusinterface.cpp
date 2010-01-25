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


KDE_EXPORT DBusInterface::DBusInterface()
{
}

KDE_EXPORT DBusInterface *DBusInterface::getInstance() {
    if (!theInstance) {
        theInstance = new DBusInterface();
    }
    return theInstance;
}


DBusInterface::~DBusInterface()
{
    //delete(DBusInterface::theInstance);
    //DBusInterface::theInstance = 0;
}

KDE_EXPORT QStringList DBusInterface::getAllRegisteredPrograms() {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    return dBusIface->registeredServiceNames();
}


KDE_EXPORT QStringList DBusInterface::getRegisteredPrograms()
{
    QStringList returnList;

    QStringList allServices = getAllRegisteredPrograms();

    //Throw out invalid entries
    for (int i = 0; i < allServices.size(); ++i) {

        QString tmp = allServices.at(i);

        QRegExp r1("[a-zA-Z]{1,3}\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9_-]+");
        if (!r1.exactMatch(tmp)) {
            continue;
        }
        if (getObjects(tmp).isEmpty()) {
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

KDE_EXPORT QStringList DBusInterface::getObjects(const QString &program) {
    QDBusInterface dBusIface(program, "/", "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface.call("Introspect");

    QDomDocument domDoc;
    domDoc.setContent(response);
    if (domDoc.toString().isEmpty()) { // No reply... perhaps a multi-instance...
        QStringList instances = getAllRegisteredPrograms().filter(program);
        if (!instances.isEmpty()) {
            QDBusInterface iFace(instances.first(), "/", "org.freedesktop.DBus.Introspectable");
            response = iFace.call("Introspect");
            domDoc.setContent(response);
        }
    }

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

KDE_EXPORT QStringList DBusInterface::getFunctions(const QString &program, const QString &object) {
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

    QStringList funcList;
    QString function;

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

                    QString method = subChild.attribute(QLatin1String("name"));
                    function = method;
                    QDomElement arg = subChild.firstChildElement();
                    QString argStr;
                    QString retArg = "void";
                    while (!arg.isNull()) {
                        if (arg.tagName() == QLatin1String("arg")) {
                            QString tmpArg = arg.attribute(QLatin1String("type"));
                            if (tmpArg == "i") {
                                tmpArg = "int";
                            } else if (tmpArg == "u") {
                                tmpArg = "uint";
                            } else if (tmpArg == "x") {
                                tmpArg = "qlonglong";
                            } else if (tmpArg == "s") {
                                tmpArg = "QString";
                            } else if (tmpArg == "b") {
                                tmpArg = "bool";
                            } else if (tmpArg == "d") {
                                tmpArg = "double";
                            } else if (tmpArg == "as") {
                                tmpArg = "QStringList";
                            } else if (tmpArg == "ay") {
                                tmpArg = "QByteArray";
                            } else {
                                arg = arg.nextSiblingElement();
                                continue;
                            }

                            if (arg.attribute(QLatin1String("direction")) == "in") {
                                if (!argStr.isEmpty()) {
                                    argStr += ", ";
                                }
                                argStr += tmpArg;
                                argStr.append(' ' );
                                if (!arg.attribute(QLatin1String("name")).isEmpty()) {
                                    argStr.append(arg.attribute(QLatin1String("name")));
                                } else {
                                    argStr.append( i18nc("The name of a parameter", "unknown"));
                                }
                            } else if (arg.attribute(QLatin1String("direction")) == "out") {
                                retArg = tmpArg;
                            }
                        }
                        arg = arg.nextSiblingElement();
                    }
                    function = retArg + ' ' + function;
                    function += '(' + argStr + ')';
                }
                subChild = subChild.nextSiblingElement();
                if (!funcList.contains(function) && !function.isEmpty()) {
                    funcList.append(function);
                }
            }
        }
        child = child.nextSiblingElement();
    }
    return funcList;
}

KDE_EXPORT QStringList DBusInterface::getRemotes() {
    QStringList remotes;
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick",
                     "", "remotes");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    } else {
        remotes = response.arguments().at(0).toStringList();
    }
    return remotes;
}


KDE_EXPORT void DBusInterface::requestNextKeyPress() {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "stealNextPress");
    m << "org.kde.kcmshell_kcm_lirc";
    m << "/KCMLirc";
    m << "gotButton";
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

KDE_EXPORT void DBusInterface::cancelKeyPressRequest() {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "dontStealNextPress");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

KDE_EXPORT QStringList DBusInterface::getButtons(const QString& remoteName) {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "buttons");
    m << remoteName;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);

    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
    return response.arguments().at(0).toStringList();
}

KDE_EXPORT void DBusInterface::reloadIRKick() {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick",
                     "", "reloadConfiguration");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }

}


KDE_EXPORT bool DBusInterface::isProgramRunning(const QString &program) {
    QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
    if (dBusIface->isServiceRegistered(program)) {
        return true;
    }
    return false;
}

KDE_EXPORT bool DBusInterface::isUnique(const QString &program){
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

bool DBusInterface::searchForProgram(const DBusAction *action, QStringList &programs)
{
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
            ;
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
    if (action->function().isEmpty()) // Just start
        return;

    if (!searchForProgram(action, programs)) {
      kDebug() << "Failed to start the application" << action->application();
      return;
    }

    for (QStringList::iterator i = programs.begin(); i != programs.end(); ++i) {
        const QString &program = *i;
        kDebug() << "Searching DBus for program:" << program;
        if (dBusIface->isServiceRegistered(program)) {
            kDebug() << "Sending data (" << program << ", " << '/' + action->node() << ", " << action->function();

            QDBusMessage m = QDBusMessage::createMethodCall(program, '/'
                             + action->node(), "", action->function());

            foreach(const Argument &arg, action->arguments()){
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
