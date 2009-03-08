/*
 * dbusqueryinterface.h
 *
 *  Created on: 14.02.2009
 *      Author: frank
 */

#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include "prototype.h"
#include "iraction.h"

#include <QStringList>

#include <ktoolinvocation.h>


class DBusProgramm
{
private:
  QString theName;
  QString theFormattedName;
  bool theMultiInstance;

public:

  DBusProgramm(){};
  DBusProgramm(QString name, QString formattedName, bool multiIntance)
  {
    theName = name;
    theFormattedName = formattedName;
    theMultiInstance = multiIntance;
  }
  const QString name()
  {
    return theName;
  };
  const QString formattedName()
  {
    return theFormattedName;
  };
   bool multiInstance()
  {
    return theMultiInstance;
  };

};

class DBusInterface
{

private:
  DBusInterface();

  bool searchForProgram(const IRAction &action, QStringList &programs);

public:


  ~DBusInterface();

  static DBusInterface *getInstance();

  QStringList getRegisteredPrograms();
  QStringList getObjects(const QString &program);
  QList<Prototype> getFunctions(const QString &program, const QString &object);
  
  void executeAction(const IRAction &action);

};

#endif