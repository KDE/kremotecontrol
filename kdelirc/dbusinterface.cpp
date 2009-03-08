/*
 * dbusqueryinterface.cpp
 *
 *  Created on: 14.02.2009
 *      Author: frank
 */



#include "dbusinterface.h"

#include <QStringList>
#include <QDBusConnectionInterface>

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

QStringList DBusInterface::getRegisteredPrograms() const
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
