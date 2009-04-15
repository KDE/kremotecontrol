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

#ifndef ADDACTION_H
#define ADDACTION_H

#include <qpair.h>

#include <QStandardItemModel>

#include "ui_addactionbase.h"
#include "arguments.h"
#include "mode.h"
#include "model.h"
#include "iraction.h"


class AddAction : public QWizard, public Ui::AddActionBase
{
private:
    Q_OBJECT

    Mode theMode;
    QStandardItemModel *dbusAppsModel;
    QStandardItemModel *argumentsModel;
    enum  {
        START =0,
        SELECT_BUTTON =1,
        SELECT_FUNCTION_DBUS =2,
        SELECT_FUNCTION_PROFILE =3,
        ACTION_ARGUMENTS =4,
        ACTION_OPTIONS=5,
        SELECT_MODE=6

    } page;

    inline Arguments getCurrentArgs() {
        Arguments retList;
        foreach(QStandardItem *item, argumentsModel->takeColumn(1)) {
            retList.append(item->data(Qt::EditRole));
        }
        return retList;
    }


public slots:
    virtual void slotModeSelected();
    virtual void updateForPageChange();
    virtual void updateButtonStates();
    virtual void updateButton(const QString &remote, const QString &button);
    virtual void updateButtons();
    virtual void updateDBusApplications();
    virtual void updateDBusFunctions(QModelIndex);
    virtual void updateProfiles();
    virtual void updateProfileFunctions();
    virtual void updateArguments();
    virtual void updateOptions();

public:
//    Arguments theArguments;
    bool isUnique;

    static const QStringList getFunctions(const QString app, const QString obj);
    void updateArgument(QTreeWidgetItem *theItem);
    IRAction * getAction();

//    QMap<QListWidgetItem *, QString> profileMap;
//    QMap<QTreeWidgetItem *, QString> profileFunctionMap;

    AddAction(QWidget *parent, const char *name, const Mode &mode);
    ~AddAction();


    virtual int nextId () const;
};

#endif
