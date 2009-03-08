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
#include <kwindowsystem.h>

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
//            KNotification::event("app_event", i18n("Starting <b>%1</b>...",
//                                                   action.application()), SmallIcon("irkick"), theTrayIcon->parentWidget());
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

