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
#include <qmap.h>

#include <kcmodule.h>

#include <dcopobject.h>

#include "kcmlircbase.h"
#include "iractions.h"
#include "modes.h"

class QListViewItem;
class KListView;
class QDropEvent;
class Profile;
class Remote;

class KCMLirc: public KCModule, virtual public DCOPObject
{
	Q_OBJECT
	K_DCOP

private:
	KCMLircBase *theKCMLircBase;
	IRActions allActions;
	Modes allModes;
	QMap<QListViewItem *, IRAIt > actionMap;
	QMap<QListViewItem *, Mode> modeMap;
	QMap<QListViewItem *, QString> profileMap, remoteMap;

	void autoPopulate(const Profile &profile, const Remote &remote, const QString &mode);

public slots:
	void updateActions();
	void updateModesStatus(QListViewItem *);
	void updateActionsStatus(QListViewItem *);
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
	void slotDrop(KListView *, QDropEvent *, QListViewItem *, QListViewItem *after);
	void slotRenamed(QListViewItem *item);
	void slotEditMode();


// MOC_SKIP_BEGIN
k_dcop:
// MOC_SKIP_END
	// now just used as a proxy to AddAction class
	virtual void gotButton(QString remote, QString button);
signals:
	void haveButton(const QString &remote, const QString &button);

public:
	virtual void load();
	virtual void save();
	virtual void defaults();
	virtual void configChanged();

	KCMLirc(QWidget *parent = 0, const char *name = 0, QStringList args = QStringList());
	~KCMLirc();
};

#endif
