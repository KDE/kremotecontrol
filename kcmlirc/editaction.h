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

inline QString getCurrentDbusApp(){
   return  editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex(),Qt::UserRole).toString();
}
inline QString getCurrentDbusFunction(){
   return  editActionBaseWidget->theDBusFunctions->itemData(editActionBaseWidget->theDBusFunctions->currentIndex(),Qt::UserRole).toString();
}



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
