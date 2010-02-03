/*************************************************************************
 * Copyright : (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>     *
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

#ifndef KCMLIRC_H
#define KCMLIRC_H

#include "ui_configurationwidget.h"
#include "model.h"
#include "action.h"
#include <remotelist.h>

#include <QMap>
#include<QVariant>
#include <QStandardItemModel>
#include <kcmodule.h>

class Profile;
class Remote;

class KCMLirc: public KCModule
{
    Q_OBJECT
    Q_CLASSINFO("KCMLirc", "org.kde.kcmlirc")

private:
    Ui::ConfigurationWidget ui;
    void autoPopulate(const Profile& profile, const Remote& remote);
    
    RemoteList m_remoteList;
    RemoteModel *m_remoteModel;
    
public:
    virtual void load();
    virtual void save();
    virtual void defaults();
    virtual void configChanged();
    const QString notes(Action *action) const;
    explicit KCMLirc(QWidget *parent = 0, const QVariantList &args = QVariantList());
    ~KCMLirc();

public slots:
    void updateActions();
    void updateModesStatus();
    void updateActionsStatus();
    void updateProfileInfo();
    void updateProfileDetails(QModelIndex);
    void updateRemoteInfo();
    void updateRemoteDetails(QModelIndex);
    void slotSetDefaultMode();
    void slotAddActions();
    void slotEditAction();
    void slotRemoveAction();
    void slotEditMode();

private slots:
    void addAction();  
    void addMode();
    void removeMode();
    void updateModes();
    void updateModeButtons(const QModelIndex &index);
    
signals:
    void haveButton(const QString &remote, const QString &button);

public Q_SLOTS:
    void gotButton(QString remote, QString button);
};

#endif
