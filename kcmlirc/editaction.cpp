//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <qregexp.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>

#include <kdebug.h>
#include <klineedit.h>
#include <klistview.h>
#include <kapplication.h>
#include <kmessagebox.h>

#include <dcopclient.h>
#include <irkick_stub.h>

#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "editaction.h"
#include "addaction.h"

EditAction::EditAction(IRAIt action, QWidget *parent, const char *name) : EditActionBase(parent, name)
{
	theAction = action;

	updateApplications();
	updateDCOPApplications();
}

EditAction::~EditAction()
{
}

void EditAction::readFrom()
{
	theRepeat->setChecked((*theAction).repeat());
	theAutoStart->setChecked((*theAction).autoStart());
	arguments = (*theAction).arguments();
	if((*theAction).program() == "")
	{	// change mode
		theChangeMode->setChecked(true);
		if((*theAction).object() == "")
			theModes->setCurrentText("[Exit current mode]");
		else
			theModes->setCurrentText((*theAction).object());
	}
	else if(ProfileServer::profileServer()->getAction((*theAction).program(), (*theAction).object(), (*theAction).method().prototype()))
	{	// profile action
		theUseProfile->setChecked(true);
		const ProfileAction *a = ProfileServer::profileServer()->getAction((*theAction).program(), (*theAction).object(), (*theAction).method().prototype());
		theApplications->setCurrentText(a->profile()->name());
		updateFunctions();
		theFunctions->setCurrentText(a->name());
		updateArguments();
	}
	else
	{	// dcop action
		theUseDCOP->setChecked(true);
		theDCOPApplications->setCurrentText((*theAction).program());
		updateDCOPObjects();
		theDCOPObjects->setCurrentText((*theAction).object());
		updateDCOPFunctions();
		theDCOPFunctions->setCurrentText((*theAction).method().prototype());
		updateArguments();
	}

}

void EditAction::writeBack()
{
	if(theChangeMode->isChecked())
	{
		(*theAction).setProgram("");
		if(theModes->currentText() == "[Exit current mode]")
			(*theAction).setObject("");
		else
			(*theAction).setObject(theModes->currentText());
	}
	else if(theUseProfile->isChecked() && ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]))
	{
		const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]);
		(*theAction).setProgram(a->profile()->id());
		(*theAction).setObject(a->objId());
		(*theAction).setMethod(a->prototype());
		(*theAction).setArguments(arguments);
	}
	else
	{
		(*theAction).setProgram(theDCOPApplications->currentText());
		(*theAction).setObject(theDCOPObjects->currentText());
		(*theAction).setMethod(theDCOPFunctions->currentText());
		(*theAction).setArguments(arguments);
	}
	(*theAction).setRepeat(theRepeat->isChecked());
	(*theAction).setAutoStart(theAutoStart->isChecked());
}

void EditAction::slotInputArgument(const QString &value)
{
	arguments[theArguments->currentItem()] = value;
}

void EditAction::updateArguments()
{
	if(theUseProfile->isChecked())
	{
		theArguments->clear();
		// TODO: arguments.resize(count);
		const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]);
		if(!a) { arguments.clear(); return; }
		const QValueList<ProfileActionArgument> &p = a->arguments();
		if(p.count() != arguments.count())
		{	arguments.clear();
			for(unsigned i = 0; i < p.count(); i++)
				arguments.append(QVariant(""));
		}
		theArguments->setEnabled(p.count());
		theArgument->setEnabled(p.count());
		for(unsigned i = 0; i < p.count(); i++)
			theArguments->insertItem(p[i].comment() + " (" + p[i].type() + ")");
		if(p.count())
			updateArgument(0);
	}
	else if(theUseDCOP->isChecked())
	{
		theArguments->clear();
		Prototype p(theDCOPFunctions->currentText());
		if(p.count() != arguments.count())
		{	arguments.clear();
			for(unsigned i = 0; i < p.count(); i++)
				arguments.append(QVariant(""));
		}
		theArguments->setEnabled(p.count());
		theArgument->setEnabled(p.count());
		for(unsigned i = 0; i < p.count(); i++)
			theArguments->insertItem(QString().setNum(i + 1) + ": " + (p.name(i) == "" ? p.type(i) : p.name(i) + " (" + p.type(i) + ")"));
		if(p.count())
			updateArgument(0);
	}
}

void EditAction::updateArgument(int index)
{
	theArgument->setText(arguments[index].toString());
}

void EditAction::updateApplications()
{
	ProfileServer *theServer = ProfileServer::profileServer();
	theApplications->clear();
	applicationMap.clear();

	QDict<Profile> dict = theServer->profiles();
	QDictIterator<Profile> i(dict);
	for(; i.current(); ++i)
	{	theApplications->insertItem(i.current()->name());
		applicationMap[i.current()->name()] = i.currentKey();
	}
	updateFunctions();
}

void EditAction::updateFunctions()
{
	ProfileServer *theServer = ProfileServer::profileServer();
	theFunctions->clear();
	functionMap.clear();
	if(theApplications->currentText() == QString::null || theApplications->currentText() == "") return;

	const Profile *p = theServer->profiles()[applicationMap[theApplications->currentText()]];

	QDict<ProfileAction> dict = p->actions();
	for(QDictIterator<ProfileAction> i(dict); i.current(); ++i)
	{	theFunctions->insertItem(i.current()->name());
		functionMap[i.current()->name()] = i.currentKey();
	}

}

void EditAction::updateDCOPApplications()
{
	theDCOPApplications->clear();
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	QCStringList theApps = theClient->registeredApplications();
	for(QCStringList::iterator i = theApps.begin(); i != theApps.end(); i++)
		if(QString(*i).find("anonymous"))
			theDCOPApplications->insertItem(QString(*i));
	updateDCOPObjects();
}

void EditAction::updateDCOPObjects()
{
	theDCOPObjects->clear();
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	if(theDCOPApplications->currentText() == QString::null || theDCOPApplications->currentText() == "") return;
	QCStringList theObjects = theClient->remoteObjects(QCString(theDCOPApplications->currentText()));
	for(QCStringList::iterator j = theObjects.begin(); j != theObjects.end(); j++)
		if(*j != "ksycoca" && *j != "qt" && AddAction::getFunctions(theDCOPApplications->currentText(), *j).count())
			theDCOPObjects->insertItem(QString(*j));
	updateDCOPFunctions();
}

void EditAction::updateDCOPFunctions()
{
	theDCOPFunctions->clear();
	if(theDCOPObjects->currentText() == QString::null || theDCOPObjects->currentText() == "") return;
	QStringList functions = AddAction::getFunctions(theDCOPApplications->currentText(), theDCOPObjects->currentText());
	for(QStringList::iterator i = functions.begin(); i != functions.end(); i++)
		theDCOPFunctions->insertItem(*i);
}

