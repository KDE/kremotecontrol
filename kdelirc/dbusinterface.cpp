/*
 * dbusqueryinterface.cpp
 *
 *  Created on: 14.02.2009
 *      Author: frank
 */



#include "dbusinterface.h"

#include <QStringList>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDomDocument>

#include <kdebug.h>

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

QStringList DBusInterface::getRegisteredPrograms()
{
  QStringList returnList;

  QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
  QStringList allServices = dBusIface->registeredServiceNames();
  
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
    returnList << tmp;

  }

  return returnList;
}

QStringList DBusInterface::getObjects(const QString &program){
    QDBusInterface *dBusIface = new QDBusInterface(program, "/", "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface->call("Introspect");

    QDomDocument domDoc;
    domDoc.setContent(response);

    QDomElement node = domDoc.documentElement();

    QDomElement child = node.firstChildElement();
    QStringList returnList;
    while (!child.isNull()) {
        if (child.tagName() == QLatin1String("node")) {
            returnList << child.attribute(QLatin1String("name"));
        }
        child = child.nextSiblingElement();
    }
    return returnList;
}

QList<Prototype> DBusInterface::getFunctions(const QString &program, const QString &object){
    QDBusInterface *dBusIface = new QDBusInterface(program, '/' + object, "org.freedesktop.DBus.Introspectable");
    QDBusReply<QString> response = dBusIface->call("Introspect");

 //   kDebug() << response;
    QDomDocument domDoc;
    domDoc.setContent(response);

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
		  function = "QString " + method + " ";
		  QDomElement arg = subChild.firstChildElement();
		  QString argStr;
		  while (!arg.isNull()) {
		    if (arg.tagName() == QLatin1String("arg")) {
		      if (arg.attribute(QLatin1String("direction")) == "in") {
			if(!argStr.isEmpty()){
			  argStr += ", ";
			}
			if (arg.attribute(QLatin1String("type")) == "i") {
			  argStr += "int";
			} else if (arg.attribute(QLatin1String("type")) == "u") {
			  argStr += "uint";
			} else if (arg.attribute(QLatin1String("type")) == "s") {
			  argStr += "QString";
			} else if (arg.attribute(QLatin1String("type")) == "b") {
			  argStr += "bool";
			} else if (arg.attribute(QLatin1String("type")) == "d") {
			  argStr += "double";
			} else if (arg.attribute(QLatin1String("type")) == "as") {
			  argStr += "QStringList";
			} else if (arg.attribute(QLatin1String("type")) == "ay") {
			  argStr += "QByteArray";
			} else if (arg.attribute(QLatin1String("type")) == "(iii)") {
			  QString helper = arg.attribute("name");
			  arg = arg.nextSiblingElement();
			  argStr += arg.attribute(QLatin1String("value")) + ' ' + helper;
			  arg = arg.nextSiblingElement();
			  continue;
			} else {
			  argStr += arg.attribute(QLatin1String("type"));
			}
			argStr += " " + arg.attribute(QLatin1String("name"));
		      }
		    }
		    arg = arg.nextSiblingElement();
		  }

		  function += "(" + argStr + ")";
                }
                subChild = subChild.nextSiblingElement();
		funcList.append(function);
            }
        }
        child = child.nextSiblingElement();
    }
    QList<Prototype> ret;
    foreach(QString tmp, funcList){
      ret.append(Prototype(tmp));
    }
    return ret;
}

