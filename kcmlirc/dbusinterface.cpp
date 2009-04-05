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

    QDBusConnection dBusConnection = QDBusConnection::sessionBus();
    dBusConnection.registerObject("/KCMLirc", this,
                                  QDBusConnection::ExportAllSlots);

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
			if(!arg.attribute(QLatin1String("name")).isEmpty()){
			  argStr += " " + arg.attribute(QLatin1String("name"));
			} else {
			  argStr += " " + i18nc("The name of a parameter", "unknown");
			}

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

QStringList DBusInterface::getRemotes(){
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


void DBusInterface::requestNextKeyPress(){
    kDebug() << "Requesting next press from irkick";
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "stealNextPress");
    m << "org.kde.kcmshell_kcm_lirc";
    m << "/KCMLirc";
    m << "gotButton";
kDebug() << "arguments are:" << m;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

void DBusInterface::cancelKeyPressRequest() {
    kDebug() << "Cancelling keypress request";
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "dontStealNextPress");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
}

QStringList DBusInterface::getButtons(const QString& remoteName) {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "buttons");
    m << remoteName;
    QDBusMessage response = QDBusConnection::sessionBus().call(m);

    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }
    kDebug() << "Got response: " << response.arguments();
    return response.arguments().at(0).toStringList();
}

void DBusInterface::reloadIRKick() {
    QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick",
                     "", "reloadConfiguration");
    QDBusMessage response = QDBusConnection::sessionBus().call(m);
    if (response.type() == QDBusMessage::ErrorMessage) {
        kDebug() << response.errorMessage();
    }

}


bool DBusInterface::isProgramRunning(const QString &program) {
  QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
  if(dBusIface->isServiceRegistered(program)){
    return true;
  }
  return false;
}

void DBusInterface::gotButton(QString remote, QString button)
{
    emit haveButton(remote, button);
}

