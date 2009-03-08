/*
 * dbusqueryinterface.h
 *
 *  Created on: 14.02.2009
 *      Author: frank
 */

#include <QStringList>

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


public:


  ~DBusInterface();

  static DBusInterface *getInstance();

  QStringList getRegisteredPrograms() const;

};
