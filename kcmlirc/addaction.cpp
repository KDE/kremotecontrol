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

#include "addaction.h"

AddAction::AddAction(QWidget *parent, const char *name): AddActionBase(parent, name)
{
	connect(this, SIGNAL( selected(const QString &) ), SLOT( updateButtonStates() ));
	updateButtonStates();
	updateObjects();
	updateFunctions();
}

AddAction::~AddAction()
{
}

QValueList< QPair<QString, QString> > AddAction::extractParameters(const QString &prototype)
{
	QRegExp main("^(.*) (\\w[\\d\\w]*)\\((.*)\\)");
	QValueList< QPair<QString, QString> > ret;
	if(main.search(prototype) == -1)
		return ret;
	ret += qMakePair(main.cap(1), main.cap(2));
	QRegExp parameters("^\\s*([^,\\s]+)(\\s+(\\w[\\d\\w]*))?(,(.*))?$");
	QString args = main.cap(3);
	while(parameters.search(args) != -1)
	{	ret += qMakePair(parameters.cap(1), parameters.cap(3));
		args = parameters.cap(5);
	}

	return ret;
}

void AddAction::updateButtonStates()
{
	if(indexOf(currentPage()) == 0)
		setNextEnabled(currentPage(), theFunctions->currentItem() != 0);
	else
		setFinishEnabled(currentPage(), true);
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
	{	QValueList<QPair<QString, QString> > l = extractParameters(theFunctions->currentItem()->text(2));
		for(unsigned k = 1; k < l.count(); k++)
			new KListViewItem(theParameters, QString().setNum(k), l[k].first, l[k].second == "" ? "<anonymous>" : l[k].second, "");
	}
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
		{	QValueList<QPair<QString, QString> > l = extractParameters(QString(*i));
			QString params;
			for(unsigned k = 1; k < l.count(); k++)
			{	if(k > 1) params += ", ";
				params += l[k].first + (l[k].second != "" ? (" " + l[k].second) : "");
			}
			new KListViewItem(theFunctions, l[0].second, params, *i);

		}
	}
}

