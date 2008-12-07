/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef _KCMLIRC_H_
#define _KCMLIRC_H_

#include <qstringlist.h>
#include <QMap>
//Added by qt3to4:
#include <QDropEvent>

#include <kcmodule.h>

#include "ui_kcmlircbase.h"
#include "iractions.h"
#include "modes.h"

class QDropEvent;
class Profile;
class Remote;

class KCMLirc: public KCModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kcmlirc")

private:
    Ui::KCMLircBase *theKCMLircBase;
    IRActions allActions;
    Modes allModes;
    QMap<QTreeWidgetItem *, IRAction *> actionMap;
    QMap<QTreeWidgetItem *, Mode> modeMap;
    QMap<QTreeWidgetItem *, QString> profileMap, remoteMap;

    void autoPopulate(const Profile &profile, const Remote &remote, const QString &mode);

public slots:
    void updateActions();
    void updateModesStatus();
    void updateActionsStatus();
    void updateModes();
    void updateExtensions();
    void updateInformation();
    void slotAddMode();
    void slotRemoveMode();
    void slotSetDefaultMode();
    void slotAddAction();
    void slotAddActions();
    void slotEditAction();
    void slotRemoveAction();
    void slotDrop(QTreeWidget *, QDropEvent *, QTreeWidgetItem *, QTreeWidgetItem *after);
    void slotEditMode();


signals:
    void haveButton(const QString &remote, const QString &button);

public:
    virtual void load();
    virtual void save();
    virtual void defaults();
    virtual void configChanged();

    explicit KCMLirc(QWidget *parent = 0, const QStringList &args = QStringList());
    ~KCMLirc();

public Q_SLOTS:
    void gotButton(QString remote, QString button);

};

#endif
