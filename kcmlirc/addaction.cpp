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
#include <qradiobutton.h>

#include <kdebug.h>
#include <klineedit.h>
#include <klistview.h>
#include <kapplication.h>
#include <kmessagebox.h>

#include <dcopclient.h>
#include <irkick_stub.h>

#include "prototype.h"
#include "addaction.h"

AddAction::AddAction(QWidget *parent, const char *name, const Mode &mode): AddActionBase(parent, name), theMode(mode)
{
	connect(this, SIGNAL( selected(const QString &) ), SLOT( slotCorrectPage() ));
	connect(this, SIGNAL( selected(const QString &) ), SLOT( updateButtonStates() ));
	updateObjects();
	updateFunctions();
	curPage = 0;
}

AddAction::~AddAction()
{
}


void AddAction::slotModeSelected()
{
	theSwitchMode->setChecked(true);
}

void AddAction::slotCorrectPage()
{
	int lastPage = curPage;
	curPage = indexOf(currentPage());

	if(curPage == 2 && theUseProfile->isChecked())
		showPage(((QWizard *)this)->page(lastPage > 1 ? 1 : 3));
	if((curPage == 2 || curPage == 5) && theChangeMode->isChecked())
		showPage(((QWizard *)this)->page(lastPage > 1 ? 1 : 6));

	if(curPage == 3 && theUseDCOP->isChecked())
		showPage(((QWizard *)this)->page(lastPage == 4 ? 2 : 4));

	if(curPage == 4 && (
	(theUseDCOP->isChecked() && theFunctions->currentItem() && !Prototype(theFunctions->currentItem()->text(2)).count()) ||
	(theUseProfile->isChecked() && theProfileFunctions->currentItem() && !theFunctions->currentItem()->text(1).toInt())
	))
		showPage(((QWizard *)this)->page(lastPage == 5 ? (theUseDCOP->isChecked() ? 2 : 3) : 5));
}

void AddAction::requestNextPress()
{
	IRKick_stub("irkick", "IRKick").stealNextPress(DCOPClient::mainClient()->appId(), "KCMLirc", "gotButton");
}

void AddAction::cancelRequest()
{
	IRKick_stub("irkick", "IRKick").dontStealNextPress();
}

void AddAction::updateButton(const QString &remote, const QString &button)
{
	if(theMode.remote() == remote)
	{	theButtons->setCurrentItem(theButtons->findItem(button, 0));
		theButtons->ensureItemVisible(theButtons->findItem(button, 0));
	}
	else
		KMessageBox::error(this, "You did not select a mode of that remote control. Please use " + theMode.remote() + ", or revert back to select a different mode.", "Incorrect remote control detected");

	if(indexOf(currentPage()) == 1)
		requestNextPress();
}

void AddAction::updateButtons()
{
	theButtons->clear();
	IRKick_stub IRKick("irkick", "IRKick");
	QStringList buttons = IRKick.buttons(theMode.remote());
	for(QStringList::iterator j = buttons.begin(); j != buttons.end(); j++)
		new QListViewItem(theButtons, *j);
}

void AddAction::updateButtonStates()
{
	if(indexOf(currentPage()) == 1) requestNextPress(); else cancelRequest();
	switch(indexOf(currentPage()))
	{	case 0: setNextEnabled(currentPage(), theProfiles->currentItem() != 0 || !theUseProfile->isChecked()); break;
		case 1: updateButtons(); setNextEnabled(currentPage(), theButtons->currentItem() != 0); break;
		case 2: setNextEnabled(currentPage(), theFunctions->currentItem() != 0); break;
		case 3: setNextEnabled(currentPage(), theProfileFunctions->currentItem() != 0); break;
		case 4: updateParameters(); setNextEnabled(currentPage(), true); break;
		case 5: setNextEnabled(currentPage(), false); setFinishEnabled(currentPage(), true); break;
		case 6: setNextEnabled(currentPage(), false); setFinishEnabled(currentPage(), theModes->currentItem() || !theSwitchMode->isChecked()); break;
	}
}

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

