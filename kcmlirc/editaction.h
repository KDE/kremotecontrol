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
#include "arguments.h"

/**
@author Gav Wood
*/

class EditAction : public KDialog, public Ui::EditActionBase
{
    Q_OBJECT
    IRAction *theAction;
    QMap<QString, QString> applicationMap, functionMap;
    QMap<QString, QString> nameProgramMap;
    QMap<QString, bool> uniqueProgramMap;
    Arguments arguments;
    QString program;
    bool isUnique;
    QButtonGroup mainGroup;

public:
    explicit EditAction(IRAction *action, QWidget *parent = 0, const char *name = 0);
    ~EditAction();


    void writeBack();
    void readFrom();

public slots:
    virtual void slotParameterChanged();
    virtual void updateArgument(int index);
    virtual void updateArguments();
    virtual void updateApplications();
    virtual void updateFunctions();
    virtual void updateOptions();
    virtual void updateDBusApplications();
    virtual void updateDBusObjects();
    virtual void updateDBusFunctions();
    virtual void addItem(QString item);

};

#endif
