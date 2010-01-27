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


#ifndef KCMLIRC_H
#define KCMLIRC_H

#include "ui_kcmlircbase.h"
#include "model.h"
#include "action.h"
#include <QMap>
#include<QVariant>
#include <QStandardItemModel>
#include <kcmodule.h>

class Profile;
class Remote;


Q_DECLARE_METATYPE(Remote*)
Q_DECLARE_METATYPE(Mode*)


class KCMLirc: public KCModule
{
    Q_OBJECT
    Q_CLASSINFO("KCMLirc", "org.kde.kcmlirc")

private:
    Ui::KCMLircBase *theKCMLircBase;
    QList<Action*> m_actionList;
    QList<Mode*> m_modeList;
    void autoPopulate(const Profile& profile, const Remote& remote);

    inline Action *currentAction() {
        Action *tAction = theKCMLircBase->theActions->currentItem()->data(0, Qt::UserRole).value<Action*>();
        return tAction;

    }
    void connectSignalsAndSlots();

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
    void updateModes();
    void updateProfileInfo();
    void updateProfileDetails(QModelIndex);
    void updateRemoteInfo();
    void updateRemoteDetails(QModelIndex);
    void slotAddMode();
    void slotRemoveMode();
    void slotSetDefaultMode();
    void slotAddAction();
    void slotAddActions();
    void slotEditAction();
    void slotRemoveAction();
    void slotEditMode();

signals:
    void haveButton(const QString &remote, const QString &button);

public Q_SLOTS:
    void gotButton(QString remote, QString button);
};

#endif
