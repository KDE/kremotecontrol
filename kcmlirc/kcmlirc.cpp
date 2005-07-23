/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlistview.h>

#include <kpushbutton.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kgenericfactory.h>
#include <klistview.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

#include <dcopclient.h>
#include <dcopref.h>

#include <irkick_stub.h>

#include "addaction.h"
#include "newmode.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "kcmlirc.h"
#include "editaction.h"
#include "editmode.h"
#include "modeslist.h"
#include "selectprofile.h"

typedef KGenericFactory<KCMLirc, QWidget> theFactory;
K_EXPORT_COMPONENT_FACTORY(kcmlirc, theFactory("kcmlirc"))

KCMLirc::KCMLirc(QWidget *parent, const char *name, QStringList /*args*/) : DCOPObject("KCMLirc"), KCModule(parent, name)
{
	KGlobal::locale()->insertCatalogue( "kcmlirc" );
	setAboutData(new KAboutData("kcmlirc", I18N_NOOP("KDE Lirc"), VERSION, I18N_NOOP("The KDE IR Remote Control System"), KAboutData::License_GPL_V2, "Copyright (c)2003 Gav Wood", I18N_NOOP("Use this to configure KDE's infrared remote control system in order to control any KDE application with your infrared remote control."), "http://www.kde.org"));
	setButtons(KCModule::Help);
	setQuickHelp(i18n("<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognised applications and remote controls, simply select the <em>Loaded Extensions</em> tab.</p>"));
	bool ok;
	KApplication::kApplication()->dcopClient()->remoteInterfaces("irkick", "IRKick", &ok);
	if(!ok)
		if(KMessageBox::questionYesNo(this, i18n("The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"), i18n("Software Not Running"), i18n("Start"), i18n("Do Not Start")) == KMessageBox::Yes)
		{	kdDebug() << "S" << KApplication::startServiceByDesktopName("irkick") << endl;
			KSimpleConfig theConfig("irkickrc");
			theConfig.setGroup("General");
			if(theConfig.readBoolEntry("AutoStart", true) == false)
				if(KMessageBox::questionYesNo(this, i18n("Would you like the infrared remote control software to start automatically when you begin KDE?"), i18n("Automatically Start?"), i18n("Start Automatically"), i18n("Do Not Start")) == KMessageBox::Yes)
					theConfig.writeEntry("AutoStart", true);
		}

	KApplication::kApplication()->dcopClient()->remoteInterfaces("irkick", "IRKick", &ok);
	kdDebug() << "OK" << ok << endl;


	(new QHBoxLayout(this))->setAutoAdd(true);
	theKCMLircBase = new KCMLircBase(this);
	connect(theKCMLircBase->theModes, SIGNAL( selectionChanged(QListViewItem *) ), this, SLOT( updateActions() ));
	connect(theKCMLircBase->theModes, SIGNAL( selectionChanged(QListViewItem *) ), this, SLOT( updateModesStatus(QListViewItem *) ));
	connect(theKCMLircBase->theActions, SIGNAL( currentChanged(QListViewItem *) ), this, SLOT( updateActionsStatus(QListViewItem *) ));
	connect(theKCMLircBase->theExtensions, SIGNAL( selectionChanged(QListViewItem *) ), this, SLOT( updateInformation() ));
	connect(theKCMLircBase->theModes, SIGNAL( itemRenamed(QListViewItem *) ), this, SLOT( slotRenamed(QListViewItem *) ));
	connect(theKCMLircBase->theModes, SIGNAL(dropped(KListView*, QDropEvent*, QListViewItem*, QListViewItem*)), this, SLOT(slotDrop(KListView*, QDropEvent*, QListViewItem*, QListViewItem*)));
	connect((QObject *)(theKCMLircBase->theAddActions), SIGNAL( clicked() ), this, SLOT( slotAddActions() ));
	connect((QObject *)(theKCMLircBase->theAddAction), SIGNAL( clicked() ), this, SLOT( slotAddAction() ));
	connect((QObject *)(theKCMLircBase->theEditAction), SIGNAL( clicked() ), this, SLOT( slotEditAction() ));
	connect((QObject *)(theKCMLircBase->theActions), SIGNAL( doubleClicked(QListViewItem *) ), this, SLOT( slotEditAction() ));
	connect((QObject *)(theKCMLircBase->theRemoveAction), SIGNAL( clicked() ), this, SLOT( slotRemoveAction() ));
	connect((QObject *)(theKCMLircBase->theAddMode), SIGNAL( clicked() ), this, SLOT( slotAddMode() ));
	connect((QObject *)(theKCMLircBase->theEditMode), SIGNAL( clicked() ), this, SLOT( slotEditMode() ));
	connect((QObject *)(theKCMLircBase->theRemoveMode), SIGNAL( clicked() ), this, SLOT( slotRemoveMode() ));
	load();
}

KCMLirc::~KCMLirc()
{
}

void KCMLirc::updateModesStatus(QListViewItem *item)
{
	theKCMLircBase->theModes->setItemsRenameable(item && item->parent());
	theKCMLircBase->theAddActions->setEnabled(ProfileServer::profileServer()->profiles().count() && theKCMLircBase->theModes->selectedItem() && RemoteServer::remoteServer()->remotes()[modeMap[theKCMLircBase->theModes->selectedItem()].remote()]);
	theKCMLircBase->theAddAction->setEnabled(item);
	theKCMLircBase->theAddMode->setEnabled(item);
	theKCMLircBase->theRemoveMode->setEnabled(item && item->parent());
	theKCMLircBase->theEditMode->setEnabled(item);
}

void KCMLirc::updateActionsStatus(QListViewItem *item)
{
	theKCMLircBase->theRemoveAction->setEnabled(item);
	theKCMLircBase->theEditAction->setEnabled(item);
}

void KCMLirc::slotRenamed(QListViewItem *item)
{
	if(!item) return;

	if(item->parent() && item->text(0) != modeMap[item].name())
	{	allActions.renameMode(modeMap[item], item->text(0));
		allModes.rename(modeMap[item], item->text(0));
		emit changed(true);
		updateModes();
	}
}

void KCMLirc::slotEditAction()
{
	if(!theKCMLircBase->theActions->currentItem()) return;

	EditAction theDialog(actionMap[theKCMLircBase->theActions->currentItem()], this);
	QListViewItem *item = theKCMLircBase->theModes->currentItem();
	if(item->parent()) item = item->parent();
	theDialog.theModes->insertItem(i18n("[Exit current mode]"));
	for(item = item->firstChild(); item; item = item->nextSibling())
		theDialog.theModes->insertItem(item->text(0));
	theDialog.readFrom();
	if(theDialog.exec() == QDialog::Accepted) { theDialog.writeBack(); emit changed(true); updateActions(); }
}

void KCMLirc::slotAddActions()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->selectedItem()];
	if(!RemoteServer::remoteServer()->remotes()[m.remote()]) return;

	SelectProfile theDialog(this, 0);

	QMap<QListViewItem *, Profile *> profileMap;
	QDict<Profile> dict = ProfileServer::profileServer()->profiles();
	for(QDictIterator<Profile> i(dict); i.current(); ++i) profileMap[new QListViewItem(theDialog.theProfiles, i.current()->name())] = i.current();

	if(theDialog.exec() == QDialog::Accepted && theDialog.theProfiles->currentItem())
	{	autoPopulate(*(profileMap[theDialog.theProfiles->currentItem()]), *(RemoteServer::remoteServer()->remotes()[m.remote()]), m.name());
		updateActions();
		emit changed(true);
	}
}

void KCMLirc::slotAddAction()
{
	kdDebug() << k_funcinfo << endl;
	if(!theKCMLircBase->theModes->selectedItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->selectedItem()];

	AddAction theDialog(this, 0, m);
	connect(this, SIGNAL(haveButton(const QString &, const QString &)), &theDialog, SLOT(updateButton(const QString &, const QString &)));

	// populate the modes list box
	QListViewItem *item = theKCMLircBase->theModes->selectedItem();
	if(item->parent()) item = item->parent();
	theDialog.theModes->setEnabled(item->firstChild());
	theDialog.theSwitchMode->setEnabled(item->firstChild());
	for(item = item->firstChild(); item; item = item->nextSibling())
	{	KListViewItem *a = new KListViewItem(theDialog.theModes, item->text(0));
		if(item->isSelected()) { a->setSelected(true); theDialog.theModes->setCurrentItem(a); }
	}

	if(theDialog.exec() == QDialog::Accepted && theDialog.theButtons->selectedItem())
	{	IRAction a;
		a.setRemote(m.remote());
		a.setMode(m.name());
		a.setButton(theDialog.buttonMap[theDialog.theButtons->selectedItem()]);
		a.setRepeat(theDialog.theRepeat->isChecked());
		a.setAutoStart(theDialog.theAutoStart->isChecked());
		a.setDoBefore(theDialog.theDoBefore->isChecked());
		a.setDoAfter(theDialog.theDoAfter->isChecked());
		a.setUnique(theDialog.isUnique);
		a.setIfMulti(theDialog.theDontSend->isChecked() ? IM_DONTSEND : theDialog.theSendToTop->isChecked() ? IM_SENDTOTOP : theDialog.theSendToBottom->isChecked() ? IM_SENDTOBOTTOM : IM_SENDTOALL);
		// change mode?
		if(theDialog.theChangeMode->isChecked())
		{
			if(theDialog.theSwitchMode->isChecked() && theDialog.theModes->selectedItem())
			{
				a.setProgram("");
				a.setObject(theDialog.theModes->selectedItem()->text(0));
			}
			else if(theDialog.theExitMode->isChecked())
			{
				a.setProgram("");
				a.setObject("");
			}
			a.setAutoStart(false);
			a.setRepeat(false);
		}
		// DCOP?
		else if(theDialog.theUseDCOP->isChecked() && theDialog.theObjects->selectedItem() && theDialog.theObjects->selectedItem()->parent() && theDialog.theFunctions->selectedItem())
		{
			a.setProgram(theDialog.program);
			a.setObject(theDialog.theObjects->selectedItem()->text(0));
			a.setMethod(theDialog.theFunctions->selectedItem()->text(2));
			theDialog.theParameters->setSorting(3);
			a.setArguments(theDialog.theArguments);
		}
		// profile?
		else if(theDialog.theUseProfile->isChecked() && theDialog.theProfiles->selectedItem() && (theDialog.theProfileFunctions->selectedItem() || theDialog.theJustStart->isChecked()))
		{
			ProfileServer *theServer = ProfileServer::profileServer();

			if(theDialog.theNotJustStart->isChecked())
			{	const ProfileAction *theAction = theServer->getAction(theDialog.profileMap[theDialog.theProfiles->selectedItem()], theDialog.profileFunctionMap[theDialog.theProfileFunctions->selectedItem()]);
				a.setProgram(theAction->profile()->id());
				a.setObject(theAction->objId());
				a.setMethod(theAction->prototype());
				theDialog.theParameters->setSorting(3);
				a.setArguments(theDialog.theArguments);
			}
			else
			{	a.setProgram(theServer->profiles()[theDialog.profileMap[theDialog.theProfiles->selectedItem()]]->id());
				a.setObject("");
			}
		}

		// save our new action
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

void KCMLirc::autoPopulate(const Profile &profile, const Remote &remote, const QString &mode)
{
	QDict<RemoteButton> d = remote.buttons();
	for(QDictIterator<RemoteButton> i(d); i.current(); ++i)
	{	const ProfileAction *pa = profile.searchClass(i.current()->getClass());
		if(pa)
		{
			IRAction a;
			a.setRemote(remote.id());
			a.setMode(mode);
			a.setButton(i.current()->id());
			a.setRepeat(pa->repeat());
			a.setAutoStart(pa->autoStart());
			a.setProgram(pa->profile()->id());
			a.setObject(pa->objId());
			a.setMethod(pa->prototype());
			a.setUnique(pa->profile()->unique());
			a.setIfMulti(pa->profile()->ifMulti());
			Arguments l;
			// argument count should be either 0 or 1. undefined if > 1.
			if(Prototype(pa->prototype()).argumentCount() == 1)
			{	l.append(QString().setNum(i.current()->parameter().toFloat() * pa->multiplier()));
				l.back().cast(QVariant::nameToType(Prototype(pa->prototype()).type(0).utf8()));
			}
			a.setArguments(l);
			allActions.addAction(a);
		}
	}
}

void KCMLirc::slotAddMode()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;

	NewMode theDialog(this, 0);
	QMap<QListViewItem *, QString> remoteMap;
	QListViewItem *tr = theKCMLircBase->theModes->selectedItem();
	if(tr) if(tr->parent()) tr = tr->parent();
	for(QListViewItem *i = theKCMLircBase->theModes->firstChild(); i; i = i->nextSibling())
	{	KListViewItem *a = new KListViewItem(theDialog.theRemotes, i->text(0));
		remoteMap[a] = modeMap[i].remote();
		if(i == tr) { a->setSelected(true); theDialog.theRemotes->setCurrentItem(a); }
	}
	if(theDialog.exec() == QDialog::Accepted && theDialog.theRemotes->selectedItem() && !theDialog.theName->text().isEmpty())
	{
		allModes.add(Mode(remoteMap[theDialog.theRemotes->selectedItem()], theDialog.theName->text()));
		updateModes();
		emit changed(true);
	}
}

void KCMLirc::slotEditMode()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;

	EditMode theDialog(this, 0);

	Mode &mode = modeMap[theKCMLircBase->theModes->selectedItem()];
	theDialog.theName->setEnabled(theKCMLircBase->theModes->selectedItem()->parent());
	theDialog.theName->setText(mode.name().isEmpty() ? mode.remoteName() : mode.name());
	if(!mode.iconFile().isNull())
		theDialog.theIcon->setIcon(mode.iconFile());
	else
		theDialog.theIcon->resetIcon();
	theDialog.theDefault->setChecked(allModes.isDefault(mode));
	theDialog.theDefault->setEnabled(!allModes.isDefault(mode));

	if(theDialog.exec() == QDialog::Accepted)
	{	kdDebug() << "Setting icon : " << theDialog.theIcon->icon() << endl;
		mode.setIconFile(theDialog.theIcon->icon().isEmpty() ? QString::null : theDialog.theIcon->icon());
		allModes.updateMode(mode);
		if(!mode.name().isEmpty())
		{	allActions.renameMode(mode, theDialog.theName->text());
			allModes.rename(mode, theDialog.theName->text());
		}
		if(theDialog.theDefault->isChecked()) allModes.setDefault(mode);
		emit changed(true);
		updateModes();
	}
}

void KCMLirc::slotRemoveMode()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;
	if(!theKCMLircBase->theModes->selectedItem()->parent()) return;

	if(KMessageBox::warningContinueCancel(this, i18n("Are you sure you want to remove %1 and all its actions?").arg(theKCMLircBase->theModes->selectedItem()->text(0)), i18n("Erase Actions?")) == KMessageBox::Continue)
	{
		allModes.erase(modeMap[theKCMLircBase->theModes->selectedItem()]);
		updateModes();
		emit changed(true);
	}
}

void KCMLirc::slotSetDefaultMode()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;
	allModes.setDefault(modeMap[theKCMLircBase->theModes->selectedItem()]);
	updateModes();
	emit changed(true);
}

void KCMLirc::slotDrop(KListView *, QDropEvent *, QListViewItem *, QListViewItem *after)
{
	Mode m = modeMap[after];

	if(modeMap[theKCMLircBase->theModes->selectedItem()].remote() != m.remote())
	{
		KMessageBox::error(this, i18n("You may only drag the selected items onto a mode of the same remote control"), i18n("You May Not Drag Here"));
		return;
	}
	for(QListViewItem *i = theKCMLircBase->theActions->firstChild(); i; i = i->nextSibling())
		if(i->isSelected())
			(*(actionMap[i])).setMode(m.name());

	updateActions();
	emit changed(true);
}

void KCMLirc::updateActions()
{
	IRAIt oldCurrent;
	if(theKCMLircBase->theActions->selectedItem()) oldCurrent = actionMap[theKCMLircBase->theActions->selectedItem()];

	theKCMLircBase->theActions->clear();
	actionMap.clear();

	if(!theKCMLircBase->theModes->selectedItem()) { updateActionsStatus(0); return; }

	Mode m = modeMap[theKCMLircBase->theModes->selectedItem()];
	theKCMLircBase->theModeLabel->setText(m.remoteName() + ": " + (m.name().isEmpty() ? i18n("Actions <i>always</i> available") : i18n("Actions available only in mode <b>%1</b>").arg(m.name())));
	IRAItList l = allActions.findByMode(m);
	for(IRAItList::iterator i = l.begin(); i != l.end(); ++i)
	{	QListViewItem *b = new KListViewItem(theKCMLircBase->theActions, (**i).buttonName(), (**i).application(), (**i).function(), (**i).arguments().toString(), (**i).notes());
		actionMap[b] = *i;
		if(*i == oldCurrent) { b->setSelected(true); theKCMLircBase->theActions->setCurrentItem(b); }
	}

	if(theKCMLircBase->theActions->currentItem())
		theKCMLircBase->theActions->currentItem()->setSelected(true);
	updateActionsStatus(theKCMLircBase->theActions->currentItem());
}

void KCMLirc::gotButton(QString remote, QString button)
{
	emit haveButton(remote, button);
}

void KCMLirc::updateModes()
{
	Mode oldCurrent;
	if(theKCMLircBase->theModes->selectedItem()) oldCurrent = modeMap[theKCMLircBase->theModes->selectedItem()];

	theKCMLircBase->theModes->clear();
	modeMap.clear();

	IRKick_stub IRKick("irkick", "IRKick");
	QStringList remotes = IRKick.remotes();
	if(remotes.begin() == remotes.end())
		theKCMLircBase->theMainLabel->setMaximumSize(32767, 32767);
	else
		theKCMLircBase->theMainLabel->setMaximumSize(0, 0);
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); ++i)
	{	Mode mode = allModes.getMode(*i, "");
		QListViewItem *a = new KListViewItem(theKCMLircBase->theModes, RemoteServer::remoteServer()->getRemoteName(*i), allModes.isDefault(mode) ? "Default" : "", mode.iconFile().isNull() ? "" : "");
		if(!mode.iconFile().isNull())
			a->setPixmap(2, KIconLoader().loadIcon(mode.iconFile(), KIcon::Panel));
		modeMap[a] = mode;	// the null mode
		if(modeMap[a] == oldCurrent) { a->setSelected(true); theKCMLircBase->theModes->setCurrentItem(a); }
		a->setOpen(true);
		ModeList l = allModes.getModes(*i);
		for(ModeList::iterator j = l.begin(); j != l.end(); ++j)
			if(!(*j).name().isEmpty())
			{	QListViewItem *b = new KListViewItem(a, (*j).name(), allModes.isDefault(*j) ? i18n("Default") : "", (*j).iconFile().isNull() ? "" : "");
				if(!(*j).iconFile().isNull())
					b->setPixmap(2, KIconLoader().loadIcon((*j).iconFile(), KIcon::Panel));
				modeMap[b] = *j;
				if(*j == oldCurrent) { b->setSelected(true); theKCMLircBase->theModes->setCurrentItem(b); }
			}
	}
	if(theKCMLircBase->theModes->currentItem())
		theKCMLircBase->theModes->currentItem()->setSelected(true);
	updateModesStatus(theKCMLircBase->theModes->currentItem());
	updateActions();
}

void KCMLirc::updateExtensions()
{
	theKCMLircBase->theExtensions->clear();

	{	ProfileServer *theServer = ProfileServer::profileServer();
		QListViewItem *a = new QListViewItem(theKCMLircBase->theExtensions, i18n("Applications"));
		a->setOpen(true);
		profileMap.clear();
		QDict<Profile> dict = theServer->profiles();
		QDictIterator<Profile> i(dict);
		for(; i.current(); ++i)
			profileMap[new QListViewItem(a, i.current()->name())] = i.currentKey();
	}
	{	RemoteServer *theServer = RemoteServer::remoteServer();
		QListViewItem *a = new QListViewItem(theKCMLircBase->theExtensions, i18n("Remote Controls"));
		a->setOpen(true);
		remoteMap.clear();
		QDict<Remote> dict = theServer->remotes();
		QDictIterator<Remote> i(dict);
		for(; i.current(); ++i)
			remoteMap[new QListViewItem(a, i.current()->name())] = i.currentKey();
	}
	updateInformation();
}

void KCMLirc::updateInformation()
{
	theKCMLircBase->theInformation->clear();
	theKCMLircBase->theInformationLabel->setText("");

	if(!theKCMLircBase->theExtensions->selectedItem()) return;

	if(!theKCMLircBase->theExtensions->selectedItem()->parent())
	{
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:").arg(theKCMLircBase->theExtensions->selectedItem()->text(0)));
		if(theKCMLircBase->theExtensions->selectedItem()->text(0) == i18n("Applications"))
			new QListViewItem(theKCMLircBase->theInformation, i18n("Number of Applications"), QString().setNum(theKCMLircBase->theExtensions->selectedItem()->childCount()));
		else if(theKCMLircBase->theExtensions->selectedItem()->text(0) == i18n("Remote Controls"))
			new QListViewItem(theKCMLircBase->theInformation, i18n("Number of Remote Controls"), QString().setNum(theKCMLircBase->theExtensions->selectedItem()->childCount()));
	}
	else if(theKCMLircBase->theExtensions->selectedItem()->parent()->text(0) == i18n("Applications"))
	{
		ProfileServer *theServer = ProfileServer::profileServer();
		const Profile *p = theServer->profiles()[profileMap[theKCMLircBase->theExtensions->selectedItem()]];
		new QListViewItem(theKCMLircBase->theInformation, i18n("Extension Name"), p->name());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Extension Author"), p->author());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Application Identifier"), p->id());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Number of Actions"), QString().setNum(p->actions().count()));
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:").arg(p->name()));
	}
	else if(theKCMLircBase->theExtensions->selectedItem()->parent()->text(0) == i18n("Remote Controls"))
	{
		RemoteServer *theServer = RemoteServer::remoteServer();
		const Remote *p = theServer->remotes()[remoteMap[theKCMLircBase->theExtensions->selectedItem()]];
		new QListViewItem(theKCMLircBase->theInformation, i18n("Extension Name"), p->name());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Extension Author"), p->author());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Remote Control Identifier"), p->id());
		new QListViewItem(theKCMLircBase->theInformation, i18n("Number of Buttons"), QString().setNum(p->buttons().count()));
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:").arg(p->name()));
	}
}

void KCMLirc::load()
{
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);
	allModes.generateNulls(IRKick_stub("irkick", "IRKick").remotes());

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
	IRKick_stub("irkick", "IRKick").reloadConfiguration();

	emit changed(true);
}

void KCMLirc::configChanged()
{
 // insert your saving code here...
    emit changed(true);
}

// TODO: Take this out when I know how
extern "C"
{
	KDE_EXPORT KCModule *create_kcmlirc(QWidget *parent, const char *)
	{	KGlobal::locale()->insertCatalogue("kcmlirc");
		return new KCMLirc(parent, "KCMLirc");
	}
}

#include <irkick_stub.cpp>

#include "kcmlirc.moc"
