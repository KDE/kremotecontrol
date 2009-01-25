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


/**
 * @author Gav Wood
 */


#ifndef EDITACTION_H
#define EDITACTION_H



#include "iractions.h"
#include "ui_editactionbase.h"


//#include <QStringList>
//#include <QtAlgorithms>
//#include <QStandardItemModel>
//#include <kdebug.h>
//
//
//
//
//class DBusListModel : public QStringListModel
//{
//Q_OBJECT
//
//private:
//  QStringList theAppList;
//
//public:
//  DBusListModel(QObject *parent) : QStringListModel(parent)
//  {
//
//  }
//
//  bool dbussAppLessThan(const QString &s1, const QString &s2)
//  {
//    kDebug() << trimAppname(s1) << trimAppname(s2);
//    return trimAppname(s1) < trimAppname(s2);
//  }
//
//  int rowCount ( const QModelIndex & ) const
//  {
//    kDebug() << " size " << stringList().size();
//    return stringList().size();
//
//  }
//
//  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
//  {
//    kDebug() <<  "blub + Index "<< index.row() <<  stringList().at(index.row());
//
////    /kDebug() <<  "blub + " + children.at(index.row());
//    return "blub + " + stringList().at(index.row());
//  }
//
//  QString trimAppname(const QString &appName)
//  {
//   QString s = appName;
//   kDebug() << s;
//   s.remove(0, appName.lastIndexOf(".",0,Qt::CaseSensitive));
//   kDebug() << s;
//   return s;
//  }
//
//};

class Arguments;

class EditActionBaseWidget : public QWidget, public Ui::EditActionBase
{
public:
  EditActionBaseWidget(QWidget *parent = 0) : QWidget(parent)
  {
    setupUi(this);
  }
};



class EditAction : public KDialog
{
private:
  Q_OBJECT
  IRAction *theAction;
  EditActionBaseWidget *editActionBaseWidget;
  QMap<QString, QString> applicationMap, functionMap, nameProgramMap;
  QMap<QString, bool> uniqueProgramMap;
  Arguments arguments;
  QString program;
  bool isUnique;
  QButtonGroup mainGroup;
  void connectSignalsAndSlots();

public:
  explicit EditAction(IRAction *action, QWidget *parent = 0, const bool &modal = false);
  ~EditAction();


  void writeBack();
  void readFrom();

public slots:
  virtual void slotParameterChanged();
  virtual void updateArgument(int index);
  virtual void updateArguments();
  virtual void initApplications();
  virtual void updateFunctions();
  virtual void updateInstancesOptions();
  virtual void initDBusApplications();
  virtual void updateDBusObjects();
  virtual void updateDBusFunctions();
  virtual void addItem(QString item);

};
#endif
