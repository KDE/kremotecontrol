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

/**
@author Gav Wood
*/
class AddAction : public AddActionBase
{
	Q_OBJECT

	const QStringList getFunctions(const QString app, const QString obj);
	QValueList< QPair<QString, QString> > extractParameters(const QString &prototype);

public slots:
	virtual void updateFunctions();
	virtual void updateObjects();
	virtual void updateButtonStates();
	virtual void updateParameters();
	virtual void updateParameter();
	virtual void updateCurrentParam(const QString &newValue);
public:
	AddAction(QWidget *parent, const char *name);
	~AddAction();
};

#endif
