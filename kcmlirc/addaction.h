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
#include <ui_addactionbase.h>


#include "arguments.h"
#include "mode.h"

class IRAction;


class AddAction : public QWizard, public Ui::AddActionBase
{
private:
    Q_OBJECT

    int curPage;
    Mode theMode;

public slots:
    virtual void slotCorrectPage();
    virtual void slotModeSelected();
    virtual void slotNextParam();
    virtual void slotParameterChanged();
    virtual void updateForPageChange();
    virtual void updateButton(const QString &remote, const QString &button);
    virtual void updateButtons();
    virtual void updateFunctions();
    virtual void updateObjects();
    virtual void updateButtonStates();
    virtual void updateParameters();
    virtual void updateParameter();
    virtual void updateProfiles();
    virtual void updateProfileFunctions();
    virtual void updateOptions();

public:
    Arguments theArguments;
    QString program;
    bool isUnique;

    static const QStringList getFunctions(const QString app, const QString obj);
    void updateArgument(QTreeWidgetItem *theItem);
    void requestNextPress();
    void cancelRequest();
    IRAction * getAction();

    QMap<QListWidgetItem *, QString> profileMap;
    QMap<QTreeWidgetItem *, QString> profileFunctionMap;
    QMap<QListWidgetItem *, QString> buttonMap;
    QMap<QTreeWidgetItem *, bool> uniqueProgramMap;
    QMap<QTreeWidgetItem *, QString> nameProgramMap;

    AddAction(QWidget *parent, const char *name, const Mode &mode);
    ~AddAction();
};

#endif
