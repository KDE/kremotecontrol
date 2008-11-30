/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ADDACTION_H
#define ADDACTION_H

#include <qpair.h>

#include <ui_addactionbase.h>

#include "mode.h"
#include "arguments.h"

/**
@author Gav Wood
*/

class AddAction : public QWizard, public Ui::AddActionBase
{
	Q_OBJECT

	int curPage;
	Mode theMode;

public slots:
	virtual void slotCorrectPage();
	virtual void slotModeSelected();
	virtual void slotNextParam();
	virtual void slotParameterChanged();

	// connected to KCMLirc class to receive DCOP calls to tell it what button has been pressed
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

	QMap<QListWidgetItem *, QString> profileMap;
	QMap<QTreeWidgetItem *, QString> profileFunctionMap;
	QMap<QListWidgetItem *, QString> buttonMap;
	QMap<QTreeWidgetItem *, bool> uniqueProgramMap;
	QMap<QTreeWidgetItem *, QString> nameProgramMap;

	AddAction(QWidget *parent, const char *name, const Mode &mode);
	~AddAction();
};

#endif
