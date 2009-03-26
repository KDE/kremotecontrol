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

#include <kdebug.h>
#include <kwindowsystem.h>
#include <knotification.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>

static DBusInterface *theInstance = NULL;


DBusInterface::DBusInterface()
{
}

DBusInterface *DBusInterface::getInstance() {
  if (!theInstance) {
    theInstance = new DBusInterface();
  }
  return theInstance;
};


DBusInterface::~DBusInterface()
{
  //delete(DBusInterface::theInstance);
  //DBusInterface::theInstance = 0;
}

QStringList DBusInterface::getAllRegisteredPrograms() {
  QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
  return dBusIface->registeredServiceNames();
}


QStringList DBusInterface::getRegisteredPrograms()
{
  QStringList returnList;

  QStringList allServices = getAllRegisteredPrograms();
  
  //Throw out invalid entries
  for(int i = 0; i < allServices.size(); ++i){
    kDebug() << "Service: " << allServices.at(i);

    QString tmp = allServices.at(i);

    QRegExp r1("[a-zA-Z]{1,3}\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9_-]+");
    if(!r1.exactMatch(tmp)){
      continue;
    }
    QRegExp r2("[a-zA-Z0-9_\\.-]+-[0-9]+");
    if(r2.exactMatch(tmp)){
      tmp.truncate(tmp.lastIndexOf('-'));
    }
    if(!returnList.contains(tmp)){
      returnList << tmp;
    }

  }

  return returnList;
}

QStringList DBusInterface::getObjects(const QString &program){
    QDBusInterface dBusIface(program, "/", "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface.call("Introspect");

    QDomDocument domDoc;
    domDoc.setContent(response);
    if(domDoc.toString().isEmpty()){ // No reply... perhaps a multi-instance...
      QStringList instances = getAllRegisteredPrograms().filter(program);
      kDebug() << "instances of " + program << instances;
      if(!instances.isEmpty()){
	QDBusInterface iFace(instances.first(), "/", "org.freedesktop.DBus.Introspectable");
	response = iFace.call("Introspect");
	domDoc.setContent(response);
	kDebug() << "new DBus response:" << response;
      }
    }

    QDomElement node = domDoc.documentElement();

    QDomElement child = node.firstChildElement();
    QStringList returnList;
    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("node")) {
	    QString name = child.attribute(QLatin1String("name"));
	    if(name != "org" && name != "modules"){
	      returnList << name;
	    }
        }
        child = child.nextSiblingElement();
    }
    kDebug() << "returning Object list: " << returnList;
    return returnList;
}

QList<Prototype> DBusInterface::getFunctions(const QString &program, const QString &object){
    QDBusInterface dBusIface(program, '/' + object, "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface.call("Introspect");

 //   kDebug() << response;
    QDomDocument domDoc;
    domDoc.setContent(response);

    if(domDoc.toString().isEmpty()){ // No reply... perhaps a multi-instance...
      QStringList instances = getAllRegisteredPrograms().filter(program);
      kDebug() << "instances of " + program << instances;
      if(!instances.isEmpty()){
	QDBusInterface iFace(instances.first(), "/" + object, "org.freedesktop.DBus.Introspectable");
	response = iFace.call("Introspect");
	domDoc.setContent(response);
	kDebug() << "new DBus response:" << response;
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
			
			if(!argStr.isEmpty()){
			  argStr += ", ";
			}
			argStr += tmpArg;
			argStr += " " + arg.attribute(QLatin1String("name"));

		      } else if(arg.attribute(QLatin1String("direction")) == "out"){
			retArg = tmpArg;

		      }

		    }
		    arg = arg.nextSiblingElement();
		  }
		  function = retArg + " " + function;
		  function += "(" + argStr + ")";
                }
                subChild = subChild.nextSiblingElement();
		if(!funcList.contains(function) && !function.isEmpty()){
		  funcList.append(function);
		}
            }
        }
        child = child.nextSiblingElement();
    }
    QList<Prototype> ret;
    foreach(QString tmp, funcList){
      ret.append(Prototype(tmp));
    }
    kDebug() << "returning function list: " << funcList;
    return ret;
}

void DBusInterface::executeAction(const IRAction& action) {
    kDebug() << "executeAction called";
    QDBusConnectionInterface *dBusIface =
        QDBusConnection::sessionBus().interface();

    QStringList programs;

    if (!searchForProgram(action, programs)) {
        return;
    }

    // if programs.size()==0 here, then the app is definately not running.
    kDebug() << "Autostart: " << action.autoStart();
    kDebug() << "programs.size: " << programs.size();
    if (action.autoStart() && !programs.size()) {
        kDebug() << "Should start " << action.program();
        QString sname = ProfileServer::profileServer()->getServiceName(
                            action.program());
        if (!sname.isNull()) {
            KNotification::event("app_event", i18n("Starting <b>%1</b>...",
                                                   action.application()), SmallIcon("irkick"));
            kDebug() << "starting service:" << sname;
            QString error;
            if(KToolInvocation::startServiceByDesktopName(sname, QString(), &error)){
            	kDebug() << "starting " + sname + " failed: " << error;
            }
        } else if (action.program().contains(QRegExp("org.[a-zA-Z0-9]*."))) {
            QString runCommand = action.program();
            runCommand.remove(QRegExp("org.[a-zA-Z0-9]*."));
            kDebug() << "runCommand" << runCommand;
            KToolInvocation::startServiceByDesktopName(runCommand);
        }
    }
    if (action.isJustStart())
        return;

    if (!searchForProgram(action, programs))
        return;

    for (QStringList::iterator i = programs.begin(); i != programs.end(); ++i) {
        const QString &program = *i;
        kDebug() << "Searching DBus for program:" << program;
        if (dBusIface->isServiceRegistered(program)) {
            kDebug() << "Sending data (" << program << ", " << '/' + action.object() << ", " << action.method().prototypeNR();

            QDBusMessage m = QDBusMessage::createMethodCall(program, '/'
                             + action.object(), "", action.method().prototypeNR());

            for (Arguments::const_iterator j = action.arguments().begin(); j
                    != action.arguments().end(); ++j) {
                kDebug() << "Got argument:" << (*j).type();
                //    m << (*j).toString();
                m << (*j);
            }
            //   theDC->send(program.utf8(), action.object().utf8(), action.method().prototypeNR().utf8(), data);
            QDBusMessage response = QDBusConnection::sessionBus().call(m);
            if (response.type() == QDBusMessage::ErrorMessage) {
                kDebug() << response.errorMessage();
            }
        }
    }
}

bool DBusInterface::searchForProgram(const IRAction &action, QStringList &programs)
{
    QDBusConnectionInterface *dBusIface =
        QDBusConnection::sessionBus().interface();
    programs.clear();

    if (action.unique()) {
        kDebug() << "searching for prog:" << action.program();
        if (dBusIface->isServiceRegistered(action.program())) {
            kDebug() << "adding Program: " << action.program();
            programs += action.program();
        } else {
	    kDebug() << "nope... " + action.program() + " not here.";
	}
    } else {

        // find all instances...
        QStringList buf = dBusIface->registeredServiceNames();

        for (QStringList::iterator i = buf.begin(); i != buf.end(); ++i) {
            QString program = *i;
            if (program.contains(action.program()))
                programs += program;
        }

        if (programs.size() == 1) {
            kDebug() << "Yeah! found it!";
        } else if (programs.size() == 0) {
            kDebug() << "Nope... not here...";
        } else {
            kDebug() << "found multiple instances...";
        }

        if (programs.size() > 1 && action.ifMulti() == IM_DONTSEND) {
            kDebug() << "size:" << programs.size() << "ifmulti:" << action.ifMulti();
            return false;
        } else if (programs.size() > 1 && action.ifMulti() == IM_SENDTOTOP) {
            ;
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); i++) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action.program() + '-' + QString().setNum(p);
                if (programs.contains(id)) {
                    programs.clear();
                    programs += id;
                    break;
                }
            }
            while (programs.size() > 1) programs.removeFirst();
        } else if (programs.size() > 1 && action.ifMulti() == IM_SENDTOBOTTOM) {
            ;
            QList<WId> s = KWindowSystem::stackingOrder();
            // go through all the (ordered) window pids
            for (int i = 0; i < s.size(); ++i) {
                int p = KWindowSystem::windowInfo(s.at(i), NET::WMPid).win();
                QString id = action.program() + '-' + QString().setNum(p);
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

bool DBusInterface::isProgramRunning(const QString &program) {
  QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
  if(dBusIface->isServiceRegistered(program)){
    return true;
  }
  return false;
}

