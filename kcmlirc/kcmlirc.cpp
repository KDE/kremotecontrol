/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@indigoarchive.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kgenericfactory.h>
#include <klistview.h>
#include <ksqueezedtextlabel.h>
#include <kmessagebox.h>

#include <dcopclient.h>

#include <irkick_stub.h>

#include "addaction.h"
#include "newmode.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "kcmlirc.h"

typedef KGenericFactory<KCMLirc, QWidget> theFactory;
K_EXPORT_COMPONENT_FACTORY(kcmlirc, theFactory("kcmlirc"));

KCMLirc::KCMLirc(QWidget *parent, const char *name, QStringList /*args*/) : KCModule(parent, name), DCOPObject("KCMLirc")
{
	(new QHBoxLayout(this))->setAutoAdd(true);
	theKCMLircBase = new KCMLircBase(this);
	connect(theKCMLircBase->theModes, SIGNAL( currentChanged(QListViewItem *) ), this, SLOT( updateActions() ));
	connect(theKCMLircBase->theExtensions, SIGNAL( currentChanged(QListViewItem *) ), this, SLOT( updateInformation() ));
	connect((QObject *)(theKCMLircBase->theAddAction), SIGNAL( clicked() ), this, SLOT( slotAddAction() ));
	connect((QObject *)(theKCMLircBase->theRemoveAction), SIGNAL( clicked() ), this, SLOT( slotRemoveAction() ));
	connect((QObject *)(theKCMLircBase->theAddMode), SIGNAL( clicked() ), this, SLOT( slotAddMode() ));
	connect((QObject *)(theKCMLircBase->theRemoveMode), SIGNAL( clicked() ), this, SLOT( slotRemoveMode() ));
	load();
}

KCMLirc::~KCMLirc()
{
}

void KCMLirc::slotAddAction()
{
	if(!theKCMLircBase->theModes->currentItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->currentItem()];

	AddAction theDialog(this, 0, m);
	connect(this, SIGNAL(haveButton(const QString &, const QString &)), &theDialog, SLOT(updateButton(const QString &, const QString &)));

	// populate the modes list box
	QListViewItem *item = theKCMLircBase->theModes->currentItem();
	if(item->parent()) item = item->parent();
	for(item = item->firstChild(); item; item = item->nextSibling())
		new KListViewItem(theDialog.theModes, item->text(0));

	if(theDialog.exec() == QDialog::Accepted && theDialog.theButtons->currentItem())
	{	IRAction a;
		a.setRemote(m.remote());
		a.setMode(m.name());
		a.setButton(theDialog.buttonMap[theDialog.theButtons->currentItem()]);
		a.setRepeat(theDialog.theRepeat->isChecked());
		a.setAutoStart(theDialog.theAutoStart->isChecked());
		Arguments args;
		// change mode?
		if(theDialog.theChangeMode->isChecked())
		{
			if(theDialog.theSwitchMode->isChecked() && theDialog.theModes->currentItem())
			{
				a.setProgram("");
				a.setObject(theDialog.theModes->currentItem()->text(0));
			}
			else if(theDialog.theExitMode->isChecked())
			{
				a.setProgram("");
				a.setObject("");
			}
			a.setRepeat(false);
		}
		// DCOP?
		else if(theDialog.theUseDCOP->isChecked() && theDialog.theObjects->currentItem() && theDialog.theObjects->currentItem()->parent() && theDialog.theFunctions->currentItem())
		{
			a.setProgram(theDialog.theObjects->currentItem()->parent()->text(0));
			a.setObject(theDialog.theObjects->currentItem()->text(0));
			a.setMethod(theDialog.theFunctions->currentItem()->text(2));
			theDialog.theParameters->setSorting(3);
			for(QListViewItem *i = theDialog.theParameters->firstChild(); i; i = i->nextSibling())
			{	QVariant v(i->text(1));
				v.cast(QVariant::nameToType(i->text(2)));
				args += v;
			}
		}
		// profile?
		else if(theDialog.theUseProfile->isChecked() && theDialog.theProfiles->currentItem() && theDialog.theProfileFunctions->currentItem())
		{
			ProfileServer *theServer = ProfileServer::profileServer();
			const ProfileAction *theAction = theServer->getAction(theDialog.profileMap[theDialog.theProfiles->currentItem()], theDialog.profileFunctionMap[theDialog.theProfileFunctions->currentItem()]);
			a.setProgram(theAction->profile()->id());
			a.setObject(theAction->objId());
			a.setMethod(theAction->prototype());
			theDialog.theParameters->setSorting(3);
			for(QListViewItem *i = theDialog.theParameters->firstChild(); i; i = i->nextSibling())
			{	QVariant v(i->text(1));
				v.cast(QVariant::nameToType(i->text(2)));
				args += v;
			}
		}

		// save our new action
		a.setArguments(args);
		allActions.addAction(a);
		updateActions();
		emit changed(true);
	}
}

void KCMLirc::slotRemoveAction()
{
	if(!theKCMLircBase->theActions->currentItem()) return;

	IRAIt i = actionMap[theKCMLircBase->theActions->currentItem()];
	allActions.erase(i);
	updateActions();
	emit changed(true);
}

void KCMLirc::slotAddMode()
{
	if(!theKCMLircBase->theModes->currentItem()) return;

	NewMode theDialog(this, 0);
	for(QListViewItem *i = theKCMLircBase->theModes->firstChild(); i; i = i->nextSibling())
		new KListViewItem(theDialog.theRemotes, i->text(0));
	if(theDialog.exec() == QDialog::Accepted && theDialog.theRemotes->currentItem() && theDialog.theName->text() != "")
	{
		allModes.add(Mode(theDialog.theRemotes->currentItem()->text(0), theDialog.theName->text()));
		updateModes();
		emit changed(true);
	}
}

void KCMLirc::slotRemoveMode()
{
	if(!theKCMLircBase->theModes->currentItem()) return;
	if(!theKCMLircBase->theModes->currentItem()->parent()) return;

	if(KMessageBox::warningContinueCancel(this, "Are you sure you want to remove " + theKCMLircBase->theModes->currentItem()->text(0) + " and all its actions?", "Erase actions?") == KMessageBox::Continue)
	{
		allModes.erase(modeMap[theKCMLircBase->theModes->currentItem()]);
		updateModes();
		emit changed(true);
	}
}

void KCMLirc::updateActions()
{
	theKCMLircBase->theActions->clear();
	actionMap.clear();
	if(!theKCMLircBase->theModes->currentItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->currentItem()];

	theKCMLircBase->theModeLabel->setText(m.remoteName() + ": " + (m.name() == "" ? "<i>Always</i>" : ("<b>" + m.name() + "</b>")));
	IRAItList l = allActions.findByMode(m);
	for(IRAItList::iterator i = l.begin(); i != l.end(); i++)
		actionMap[new KListViewItem(theKCMLircBase->theActions, (**i).buttonName(), (**i).application(), (**i).function(), (**i).arguments().toString(), (**i).repeatable(), (**i).autoStartable())] = *i;
}

void KCMLirc::gotButton(QString remote, QString button)
{
	emit haveButton(remote, button);
}

void KCMLirc::updateModes()
{
	theKCMLircBase->theModes->clear();
	modeMap.clear();

	IRKick_stub IRKick("kded", "irkick");
	QStringList remotes = IRKick.remotes();
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); i++)
	{	QListViewItem *a = new QListViewItem(theKCMLircBase->theModes, RemoteServer::remoteServer()->getRemoteName(*i));
		modeMap[a] = Mode(*i, "");	// the null mode
		a->setOpen(true);
		ModeList l = allModes.getModes(*i);
		for(ModeList::iterator j = l.begin(); j != l.end(); j++)
			modeMap[new QListViewItem(a, (*j).name())] = *j;
	}
}

void KCMLirc::updateExtensions()
{
	theKCMLircBase->theExtensions->clear();

	{	ProfileServer *theServer = ProfileServer::profileServer();
		QListViewItem *a = new QListViewItem(theKCMLircBase->theExtensions, "Applications");
		a->setOpen(true);
		profileMap.clear();
		QDict<Profile> dict = theServer->profiles();
		QDictIterator<Profile> i(dict);
		for(; i.current(); ++i)
			profileMap[new QListViewItem(a, i.current()->name())] = i.currentKey();
	}
	{	RemoteServer *theServer = RemoteServer::remoteServer();
		QListViewItem *a = new QListViewItem(theKCMLircBase->theExtensions, "Remote Controls");
		a->setOpen(true);
		remoteMap.clear();
		QDict<Remote> dict = theServer->remotes();
		QDictIterator<Remote> i(dict);
		for(; i.current(); ++i)
			remoteMap[new QListViewItem(a, i.current()->name())] = i.currentKey();
	}
}

void KCMLirc::updateInformation()
{
	theKCMLircBase->theInformation->clear();
	theKCMLircBase->theInformationLabel->setText("");

	if(!theKCMLircBase->theExtensions->currentItem()) return;

	if(!theKCMLircBase->theExtensions->currentItem()->parent())
	{
		theKCMLircBase->theInformationLabel->setText("Information on <b>" + theKCMLircBase->theExtensions->currentItem()->text(0) + "</b>:");
		if(theKCMLircBase->theExtensions->currentItem()->text(0) == "Applications")
			new QListViewItem(theKCMLircBase->theInformation, "Number of Applications", QString().setNum(theKCMLircBase->theExtensions->currentItem()->childCount()));
		else if(theKCMLircBase->theExtensions->currentItem()->text(0) == "Remote Controls")
			new QListViewItem(theKCMLircBase->theInformation, "Number of Remote Controls", QString().setNum(theKCMLircBase->theExtensions->currentItem()->childCount()));
	}
	else if(theKCMLircBase->theExtensions->currentItem()->parent()->text(0) == "Applications")
	{
		ProfileServer *theServer = ProfileServer::profileServer();
		const Profile *p = theServer->profiles()[profileMap[theKCMLircBase->theExtensions->currentItem()]];
		new QListViewItem(theKCMLircBase->theInformation, "Extension Name", p->name());
		new QListViewItem(theKCMLircBase->theInformation, "Extension Author", p->author());
		new QListViewItem(theKCMLircBase->theInformation, "Application Identifier", p->id());
		new QListViewItem(theKCMLircBase->theInformation, "Number of Actions", QString().setNum(p->actions().count()));
		theKCMLircBase->theInformationLabel->setText("Information on <b>" + p->name() + "</b>:");
	}
	else if(theKCMLircBase->theExtensions->currentItem()->parent()->text(0) == "Remote Controls")
	{
		RemoteServer *theServer = RemoteServer::remoteServer();
		const Remote *p = theServer->remotes()[remoteMap[theKCMLircBase->theExtensions->currentItem()]];
		new QListViewItem(theKCMLircBase->theInformation, "Extension Name", p->name());
		new QListViewItem(theKCMLircBase->theInformation, "Extension Author", p->author());
		new QListViewItem(theKCMLircBase->theInformation, "Remote Control Identifier", p->id());
		new QListViewItem(theKCMLircBase->theInformation, "Number of Buttons", QString().setNum(p->buttons().count()));
		theKCMLircBase->theInformationLabel->setText("Information on <b>" + p->name() + "</b>:");
	}
}

void KCMLirc::load()
{
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);

	updateExtensions();
	updateModes();
	updateActions();
}

void KCMLirc::defaults()
{
	// insert your default settings code here...
	emit changed(true);
}

void KCMLirc::save()
{
	KSimpleConfig theConfig("irkickrc");
	allActions.saveToConfig(theConfig);
	allModes.saveToConfig(theConfig);

	theConfig.sync();
	IRKick_stub("kded", "irkick").reloadConfiguration();

	emit changed(true);
}

int KCMLirc::buttons()
{
    return KCModule::Help;
}

void KCMLirc::configChanged()
{
 // insert your saving code here...
    emit changed(true);
}

QString KCMLirc::quickHelp() const
{
    return i18n("Helpful information about the kcmlirc module.");
}

// TODO: Take this out when I know how
extern "C"
{
	KCModule *create_kcmlirc(QWidget *parent, const char *)
	{	KGlobal::locale()->insertCatalogue("kcmlirc");
		return new KCMLirc(parent, "KCMLirc");
	}
}

#include <irkick_stub.cpp>

#include "kcmlirc.moc"
