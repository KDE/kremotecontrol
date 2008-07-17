//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EDITACTION_H
#define EDITACTION_H



#include "iractions.h"
#include "ui_editactionbase.h"
#include "arguments.h"

/**
@author Gav Wood
*/

class EditAction : public QDialog, public Ui::EditActionBase
{
	Q_OBJECT
	IRAIt theAction;
	QMap<QString, QString> applicationMap, functionMap;
	QMap<QString, QString> nameProgramMap;
	QMap<QString, bool> uniqueProgramMap;
	Arguments arguments;
	QString program;
	bool isUnique;

public:
	void writeBack();
	void readFrom();

	virtual void slotParameterChanged();
	virtual void updateArgument(int index);
	virtual void updateArguments();
	virtual void updateApplications();
	virtual void updateFunctions();
	virtual void updateOptions();
	virtual void updateDCOPApplications();
	virtual void updateDCOPObjects();
	virtual void updateDCOPFunctions();
	virtual void addItem(QString item);

	EditAction(IRAIt action, QWidget *parent = 0, const char *name = 0);
	~EditAction();
};

#endif
