/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@indigoarchive.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qregexp.h>
#include <qlabel.h>

#include <klineedit.h>
#include <klistview.h>
#include <kapplication.h>

#include <dcopclient.h>

#include "prototype.h"
#include "addaction.h"

AddAction::AddAction(QWidget *parent, const char *name): AddActionBase(parent, name)
{
	connect(this, SIGNAL( selected(const QString &) ), SLOT( updateButtonStates() ));
	updateButtonStates();
	updateObjects();
	updateFunctions();
	curPage = 0;
}

AddAction::~AddAction()
{
}

void AddAction::updateButtonStates()
{
	int lastPage = curPage;
	curPage = indexOf(currentPage());
	if(curPage == 1 && theFunctions->currentItem() && !Prototype(theFunctions->currentItem()->text(2)).count())
		showPage(((QWizard *)this)->page(lastPage ? 0 : 2));
	switch(curPage)
	{	case 0: setNextEnabled(currentPage(), theFunctions->currentItem() != 0); break;
		case 1: if(lastPage == 0) updateParameters(); setNextEnabled(currentPage(), true); break;
		case 2: setFinishEnabled(currentPage(), true); break;
	}
}

// TODO: consolidate this (and stringising code from updateFunctions) into a class to desctribe function prototype.
const QStringList AddAction::getFunctions(const QString app, const QString obj)
{
	QStringList ret;
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	QCStringList theApps = theClient->remoteFunctions(QCString(app), QCString(obj));
	for(QCStringList::iterator i = theApps.begin(); i != theApps.end(); i++)
		if(	QString(*i) != "QCStringList interfaces()" &&
			QString(*i) != "QCStringList functions()" &&
			QString(*i) != "QCStringList objects()" &&
			QString(*i) != "QCStringList find(QCString)" )
			ret += *i;
	return ret;

}

void AddAction::updateParameters()
{
	theParameters->clear();
	if(theFunctions->currentItem())
	{	Prototype p(theFunctions->currentItem()->text(2));
		for(unsigned k = 0; k < p.count(); k++)
			new KListViewItem(theParameters, QString().setNum(k + 1), p.type(k), p.name(k) == "" ? "<anonymous>" : p.name(k), "");
	}
	updateParameter();
}

void AddAction::updateParameter()
{
	if(theParameters->currentItem())
	{	theCurParameter->setText(theParameters->currentItem()->text(2));
		theCurValue->setText(theParameters->currentItem()->text(3));
		theCurComment->setText("This field must be of type " + theParameters->currentItem()->text(1) + ".");
		theCurParameter->setEnabled(true);
		theCurValue->setEnabled(true);
		theCurComment->setEnabled(true);
	}
	else
	{	theCurParameter->setText("");
		theCurValue->setText("");
		theCurComment->setText("");
		theCurParameter->setEnabled(false);
		theCurValue->setEnabled(false);
		theCurComment->setEnabled(false);
	}
}

void AddAction::updateCurrentParam(const QString &newValue)
{
	if(theParameters->currentItem())
		theParameters->currentItem()->setText(3, newValue);
}

void AddAction::updateObjects()
{
	theObjects->clear();
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	QCStringList theApps = theClient->registeredApplications();
	for(QCStringList::iterator i = theApps.begin(); i != theApps.end(); i++)
	{	if(QString(*i).find("anonymous"))
		{	KListViewItem *a = new KListViewItem(theObjects, *i);
			QCStringList theObjects = theClient->remoteObjects(*i);
			for(QCStringList::iterator j = theObjects.begin(); j != theObjects.end(); j++)
				if(*j != "ksycoca" && *j != "qt")// && getFunctions(*i, *j).count())
					new KListViewItem(a, *j);
		}
	}
}

void AddAction::updateFunctions()
{
	theFunctions->clear();
	if(theObjects->currentItem() && theObjects->currentItem()->parent())
	{	QStringList functions = getFunctions(theObjects->currentItem()->parent()->text(0), theObjects->currentItem()->text(0));
		for(QStringList::iterator i = functions.begin(); i != functions.end(); i++)
		{	Prototype p((QString)(*i));
			new KListViewItem(theFunctions, p.name(), p.argumentList(), *i);
		}
	}
}

