/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@indigoarchive.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ADDACTION_H
#define ADDACTION_H

#include <qvaluelist.h>
#include <qpair.h>

#include <addactionbase.h>

#include "mode.h"

/**
@author Gav Wood
*/

class AddAction : public AddActionBase
{
	Q_OBJECT

	int curPage;
	Mode theMode;

public slots:
	virtual void slotCorrectPage();
	virtual void slotModeSelected();
	virtual void slotNextParam();

	// connected to KCMLirc class to receive DCOP calls to tell it what button has been pressed
	virtual void updateForPageChange();
	virtual void updateButton(const QString &remote, const QString &button);
	virtual void updateButtons();
	virtual void updateFunctions();
	virtual void updateObjects();
	virtual void updateButtonStates();
	virtual void updateParameters();
	virtual void updateParameter();
	virtual void updateCurrentParam(const QString &newValue);
	virtual void updateProfiles();
	virtual void updateProfileFunctions();

public:
	static const QStringList getFunctions(const QString app, const QString obj);

	void requestNextPress();
	void cancelRequest();

	QMap<QListViewItem *, QString> profileMap;
	QMap<QListViewItem *, QString> profileFunctionMap;
	QMap<QListViewItem *, QString> buttonMap;

	AddAction(QWidget *parent, const char *name, const Mode &mode);
	~AddAction();
};

#endif
