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

#include "model.h"

#include <QStandardItemModel>

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
    QMap<QString, QString> nameProgramMap;
    QMap<QString, bool> uniqueProgramMap;
    QString program;
    bool isUnique;
    QButtonGroup mainGroup;
    void connectSignalsAndSlots();
    QStandardItemModel *argumentsModel;
   QStandardItemModel *dbusServiceModel;
    inline QString getCurrentDbusApp() {
        return  editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex(),Qt::UserRole).toString();
    }
    inline QString getCurrentDBusFunction() {
        return  editActionBaseWidget->theDBusFunctions->itemData(editActionBaseWidget->theDBusFunctions->currentIndex(),Qt::UserRole).value<Prototype>().prototype();
    }
    Arguments getCurrentArgs();



public:
    explicit EditAction(IRAction *action, QWidget *parent = 0, const bool &modal = false);
    ~EditAction();


    void writeBack();
    void readFrom();
    void addItem(QString item);

private slots:
    void updateApplications();
    void updateFunctions();
    void updateArguments();
    void updateInstancesOptions();
    void updateDBusApplications();
    void updateDBusObjects();
    void updateDBusFunctions();


};
#endif
