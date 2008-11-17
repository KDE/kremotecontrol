/***************************************************************************
 *   Copyright (C) 2003 by Gav Wood                                        *
 *   gav@kde.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "addaction.h"
#include "newmodedialog.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "kcmlirc.h"
#include "editaction.h"
#include "editmode.h"
#include "modeslist.h"
#include "ui_selectprofile.h"
#include "kcmlircadaptor.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <qradiobutton.h>
#include <QComboBox>
#include <qevent.h>
//#include <q3listview.h>
//Added by qt3to4:
#include <QHBoxLayout>
#include <QDropEvent>
#include <QWidget>
#include <qdbusmessage.h>
#include <qdbusconnection.h>

#include <kpushbutton.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kgenericfactory.h>
#include <k3listview.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kaboutdata.h>
//#include <dcopclient.h>
//#include <dcopref.h>

//#include <irkick_stub.h>


#define VERSION "version name goes here"

typedef KGenericFactory<KCMLirc, QWidget> theFactory;
K_EXPORT_COMPONENT_FACTORY(kcm_lirc, theFactory("kcmlirc"))

KCMLirc::KCMLirc(QWidget *parent, const QStringList &/*args*/)
	: KCModule(theFactory::componentData(),parent)
{
	new KcmlircAdaptor(this);
	QDBusConnection dBusConnection = QDBusConnection::sessionBus();
	dBusConnection.registerObject("/KCMLirc", this, QDBusConnection::ExportAllSlots);

	KGlobal::locale()->insertCatalog( "kcmlirc" );
	setAboutData(new KAboutData("kcmlirc", 0, ki18n("KDE Lirc"), VERSION, ki18n("The KDE IR Remote Control System"), KAboutData::License_GPL_V2, ki18n("Copyright (c)2003 Gav Wood"), ki18n("Use this to configure KDE's infrared remote control system in order to control any KDE application with your infrared remote control."), "http://www.kde.org"));
	setButtons(KCModule::Help);
	setQuickHelp(i18n("<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognised applications and remote controls, simply select the <em>Loaded Extensions</em> tab.</p>"));
	bool ok;

#warning Port DCOP to DBUS
	QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "remotes");
	QDBusMessage response = QDBusConnection::sessionBus().call(m);
	if( response.type() == QDBusMessage::ErrorMessage ){
		kDebug() << response.errorMessage();
		if(KMessageBox::questionYesNo(this, i18n("The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"), i18n("Software Not Running"), KGuiItem("Start"), KGuiItem("Do Not Start")) == KMessageBox::Yes)
		{	kDebug() << "S" << KToolInvocation::startServiceByDesktopName("irkick") ;
			KConfig theConfig("irkickrc");
			KConfigGroup generalGroup = theConfig.group("General");
			if(generalGroup.readEntry("AutoStart", true) == false)
				if(KMessageBox::questionYesNo(this, i18n("Would you like the infrared remote control software to start automatically when you begin KDE?"), i18n("Automatically Start?"), KGuiItem("Start Automatically"), KGuiItem("Do Not Start")) == KMessageBox::Yes)
					generalGroup.writeEntry("AutoStart", true);
		}
	}


//	KApplication::kApplication()->dcopClient()->remoteInterfaces("irkick", "IRKick", &ok);
//	ok = false;
//	kDebug() << "OK" << ok ;


	QHBoxLayout *layout = new QHBoxLayout(this);

	QWidget *widget = new QWidget(this);
	theKCMLircBase = new Ui::KCMLircBase();
	theKCMLircBase->setupUi(widget);
	layout->addWidget(widget);




	connect(theKCMLircBase->theModes, SIGNAL( selectionChanged(Q3ListViewItem *) ), this, SLOT( updateActions() ));
	connect(theKCMLircBase->theModes, SIGNAL( selectionChanged(Q3ListViewItem *) ), this, SLOT( updateModesStatus(Q3ListViewItem *) ));
	connect(theKCMLircBase->theActions, SIGNAL( currentChanged(Q3ListViewItem *) ), this, SLOT( updateActionsStatus(Q3ListViewItem *) ));
	connect(theKCMLircBase->theExtensions, SIGNAL( selectionChanged(Q3ListViewItem *) ), this, SLOT( updateInformation() ));
	connect(theKCMLircBase->theModes, SIGNAL( itemRenamed(Q3ListViewItem *) ), this, SLOT( slotRenamed(Q3ListViewItem *) ));
	connect(theKCMLircBase->theModes, SIGNAL(dropped(K3ListView*, QDropEvent*, Q3ListViewItem*, Q3ListViewItem*)), this, SLOT(slotDrop(K3ListView*, QDropEvent*, Q3ListViewItem*, Q3ListViewItem*)));
	connect((QObject *)(theKCMLircBase->theAddActions), SIGNAL( clicked() ), this, SLOT( slotAddActions() ));
	connect((QObject *)(theKCMLircBase->theAddAction), SIGNAL( clicked() ), this, SLOT( slotAddAction() ));
	connect((QObject *)(theKCMLircBase->theEditAction), SIGNAL( clicked() ), this, SLOT( slotEditAction() ));
	connect((QObject *)(theKCMLircBase->theActions), SIGNAL( doubleClicked(Q3ListViewItem *) ), this, SLOT( slotEditAction() ));
	connect((QObject *)(theKCMLircBase->theRemoveAction), SIGNAL( clicked() ), this, SLOT( slotRemoveAction() ));
	connect((QObject *)(theKCMLircBase->theAddMode), SIGNAL( clicked() ), this, SLOT( slotAddMode() ));
	connect((QObject *)(theKCMLircBase->theEditMode), SIGNAL( clicked() ), this, SLOT( slotEditMode() ));
	connect((QObject *)(theKCMLircBase->theRemoveMode), SIGNAL( clicked() ), this, SLOT( slotRemoveMode() ));
	load();
}

KCMLirc::~KCMLirc()
{
}

void KCMLirc::updateModesStatus(Q3ListViewItem *item)
{
	theKCMLircBase->theModes->setItemsRenameable(item && item->parent());
	theKCMLircBase->theAddActions->setEnabled(ProfileServer::profileServer()->profiles().count() && theKCMLircBase->theModes->selectedItem() && RemoteServer::remoteServer()->remotes()[modeMap[theKCMLircBase->theModes->selectedItem()].remote()]);
	theKCMLircBase->theAddAction->setEnabled(item);
	theKCMLircBase->theAddMode->setEnabled(item);
	theKCMLircBase->theRemoveMode->setEnabled(item && item->parent());
	theKCMLircBase->theEditMode->setEnabled(item);
}

void KCMLirc::updateActionsStatus(Q3ListViewItem *item)
{
	theKCMLircBase->theRemoveAction->setEnabled(item);
	theKCMLircBase->theEditAction->setEnabled(item);
}

void KCMLirc::slotRenamed(Q3ListViewItem *item)
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
	Q3ListViewItem *item = theKCMLircBase->theModes->currentItem();
	if(item->parent()) item = item->parent();
	theDialog.addItem(i18n("[Exit current mode]"));
	for(item = item->firstChild(); item; item = item->nextSibling())
		theDialog.addItem(item->text(0));
	theDialog.readFrom();

	if(theDialog.exec() == QDialog::Accepted) { theDialog.writeBack(); emit changed(true); updateActions(); }
}

void KCMLirc::slotAddActions()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->selectedItem()];
	if(!RemoteServer::remoteServer()->remotes()[m.remote()]) return;

	QDialog *theDialog = new QDialog(this);
	Ui::SelectProfile *ui = new Ui::SelectProfile();
	ui->setupUi(theDialog);

	QMap<Q3ListViewItem *, Profile *> profileMap;
	QHash<QString, Profile*> dict = ProfileServer::profileServer()->profiles();

	QHash<QString, Profile*>::const_iterator i;
	for (i = dict.constBegin(); i != dict.constEnd(); ++i)
		 profileMap[new Q3ListViewItem(ui->theProfiles, i.value()->name())] = i.value();

	if(theDialog->exec() == QDialog::Accepted && ui->theProfiles->currentItem())
	{	autoPopulate(*(profileMap[ui->theProfiles->currentItem()]), *(RemoteServer::remoteServer()->remotes()[m.remote()]), m.name());
		updateActions();
		emit changed(true);
	}
}

void KCMLirc::slotAddAction()
{
	kDebug() ;
	if(!theKCMLircBase->theModes->selectedItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->selectedItem()];
	kDebug() << "Calling AddAction with Mode: " << m.name();
	AddAction theDialog(this, 0, m);
	connect(this, SIGNAL(haveButton(const QString &, const QString &)), &theDialog, SLOT(updateButton(const QString &, const QString &)));


      // populate the modes list box
        Q3ListViewItem *item = theKCMLircBase->theModes->selectedItem();
        if(item->parent()) item = item->parent();
        theDialog.theModes->setEnabled(item->firstChild());
        theDialog.theSwitchMode->setEnabled(item->firstChild());
        for(item = item->firstChild(); item; item = item->nextSibling())
        {       QListWidgetItem *a = new QListWidgetItem(item->text(0), theDialog.theModes);
                if(item->isSelected()) { a->setSelected(true); theDialog.theModes->setCurrentItem(a); }
        }

        if(theDialog.exec() == QDialog::Accepted && theDialog.theButtons->currentItem())
        {       IRAction a;
                a.setRemote(m.remote());
                a.setMode(m.name());
                kDebug() << "Saving action. Button is: " << theDialog.buttonMap[theDialog.theButtons->currentItem()];
                a.setButton(theDialog.buttonMap[theDialog.theButtons->currentItem()]);
                a.setRepeat(theDialog.theRepeat->isChecked());
                a.setAutoStart(theDialog.theAutoStart->isChecked());
                a.setDoBefore(theDialog.theDoBefore->isChecked());
                a.setDoAfter(theDialog.theDoAfter->isChecked());
                a.setUnique(theDialog.isUnique);
                a.setIfMulti(theDialog.theDontSend->isChecked() ? IM_DONTSEND : theDialog.theSendToTop->isChecked() ? IM_SENDTOTOP : theDialog.theSendToBottom->isChecked() ? IM_SENDTOBOTTOM : IM_SENDTOALL);
                // change mode?
                if(theDialog.theChangeMode->isChecked())
                {
                        if(theDialog.theSwitchMode->isChecked() && !theDialog.theModes->selectedItems().isEmpty())
                        {
                                a.setProgram("");
                                a.setObject(theDialog.theModes->selectedItems().first()->text());
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
                else if(theDialog.theUseDCOP->isChecked() && !theDialog.theObjects->selectedItems().isEmpty() && theDialog.theObjects->selectedItems().first()->parent() && !theDialog.theFunctions->selectedItems().isEmpty())
                {
                        a.setProgram(theDialog.program);
                        a.setObject(theDialog.theObjects->selectedItems().first()->text(0));
                        a.setMethod(theDialog.theFunctions->selectedItems().first()->text(2));
                        theDialog.theParameters->sortItems(3, Qt::AscendingOrder);
                        a.setArguments(theDialog.theArguments);
                }
                // profile?
                else if(theDialog.theUseProfile->isChecked() && !theDialog.theProfiles->selectedItems().isEmpty() && (!theDialog.theProfileFunctions->selectedItems().isEmpty() || theDialog.theJustStart->isChecked()))
                {
                        ProfileServer *theServer = ProfileServer::profileServer();

                        if(theDialog.theNotJustStart->isChecked())
                        {       const ProfileAction *theAction = theServer->getAction(theDialog.profileMap[theDialog.theProfiles->selectedItems().first()], theDialog.profileFunctionMap[theDialog.theProfileFunctions->selectedItems().first()]);
                                a.setProgram(theAction->profile()->id());
                                a.setObject(theAction->objId());
                                a.setMethod(theAction->prototype());
                                theDialog.theParameters->sortItems(3, Qt::AscendingOrder);
                                a.setArguments(theDialog.theArguments);
                        }
                        else
                        {       a.setProgram(theServer->profiles()[theDialog.profileMap[theDialog.theProfiles->selectedItems().first()]]->id());
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
	QHash<QString, RemoteButton*> d = remote.buttons();
	QHash<QString, RemoteButton*>::const_iterator i;
	for(i = d.constBegin(); i != d.constEnd(); ++i)	{
		const ProfileAction *pa = profile.searchClass(i.value()->getClass());
		if(pa)
		{
			IRAction a;
			a.setRemote(remote.id());
			a.setMode(mode);
			a.setButton(i.value()->id());
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
			{	l.append(QString().setNum(i.value()->parameter().toFloat() * pa->multiplier()));
				l.back().convert(QVariant::nameToType(Prototype(pa->prototype()).type(0).toLocal8Bit()));
			}
			a.setArguments(l);
			allActions.addAction(a);
		}
	}
}

void KCMLirc::slotAddMode()
{
	if(!theKCMLircBase->theModes->selectedItem()) return;

	NewModeDialog theDialog(this);
	QMap<Q3ListViewItem *, QString> remoteMap;
	Q3ListViewItem *tr = theKCMLircBase->theModes->selectedItem();
	if(tr) if(tr->parent()) tr = tr->parent();
	for(Q3ListViewItem *i = theKCMLircBase->theModes->firstChild(); i; i = i->nextSibling())
	{	K3ListViewItem *a = new K3ListViewItem(theDialog.theRemotes, i->text(0));
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
	{	kDebug() << "Setting icon : " << theDialog.theIcon->icon() ;
		mode.setIconFile(theDialog.theIcon->icon().isEmpty() ? QString::null : theDialog.theIcon->icon());	//krazy:exclude=nullstrassign for old broken gcc
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

	if(KMessageBox::warningContinueCancel(this, i18n("Are you sure you want to remove %1 and all its actions?", theKCMLircBase->theModes->selectedItem()->text(0)), i18n("Erase Actions?")) == KMessageBox::Continue)
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

void KCMLirc::slotDrop(K3ListView *, QDropEvent *, Q3ListViewItem *, Q3ListViewItem *after)
{
	Mode m = modeMap[after];

	if(modeMap[theKCMLircBase->theModes->selectedItem()].remote() != m.remote())
	{
		KMessageBox::error(this, i18n("You may only drag the selected items onto a mode of the same remote control"), i18n("You May Not Drag Here"));
		return;
	}
	for(Q3ListViewItem *i = theKCMLircBase->theActions->firstChild(); i; i = i->nextSibling())
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
	theKCMLircBase->theModeLabel->setText(m.remoteName() + ": " + (m.name().isEmpty() ? i18n("Actions <i>always</i> available") : i18n("Actions available only in mode <b>%1</b>", m.name())));
	IRAItList l = allActions.findByMode(m);
	for(IRAItList::iterator i = l.begin(); i != l.end(); ++i) {
		kDebug() << "Adding action: " << (**i).buttonName() << (**i).application(), (**i).function();
		Q3ListViewItem *b = new K3ListViewItem(theKCMLircBase->theActions, (**i).buttonName(), (**i).application(), (**i).function(), (**i).arguments().toString(), (**i).notes());
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

	kDebug() << "updating Modes";

	QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "remotes");
	QDBusMessage response = QDBusConnection::sessionBus().call(m);
	if( response.type() == QDBusMessage::ErrorMessage ){
		kDebug() << response.errorMessage();
	} 

	QStringList remotes;
	for( int i = 0; i < response.arguments().at(0).toStringList().size(); ++i){
		kDebug() << "Reveiced remote: " << response.arguments().at(0).toStringList().at(i);
	}
	remotes = response.arguments().at(0).toStringList();

	if(remotes.begin() == remotes.end())
		theKCMLircBase->theMainLabel->setMaximumSize(32767, 32767);
	else
		theKCMLircBase->theMainLabel->setMaximumSize(0, 0);
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); ++i)
	{	Mode mode = allModes.getMode(*i, "");
		Q3ListViewItem *a = new K3ListViewItem(theKCMLircBase->theModes, RemoteServer::remoteServer()->getRemoteName(*i), allModes.isDefault(mode) ? "Default" : "", mode.iconFile().isNull() ? "" : "");
		if(!mode.iconFile().isNull())
			a->setPixmap(2, KIconLoader().loadIcon(mode.iconFile(), KIconLoader::Panel));
		modeMap[a] = mode;	// the null mode
		if(modeMap[a] == oldCurrent) { a->setSelected(true); theKCMLircBase->theModes->setCurrentItem(a); }
		a->setOpen(true);
		ModeList l = allModes.getModes(*i);
		for(ModeList::iterator j = l.begin(); j != l.end(); ++j)
			if(!(*j).name().isEmpty())
			{	Q3ListViewItem *b = new K3ListViewItem(a, (*j).name(), allModes.isDefault(*j) ? i18n("Default") : "", (*j).iconFile().isNull() ? "" : "");
				if(!(*j).iconFile().isNull())
					b->setPixmap(2, KIconLoader().loadIcon((*j).iconFile(), KIconLoader::Panel));
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
		Q3ListViewItem *a = new Q3ListViewItem(theKCMLircBase->theExtensions, i18n("Applications"));
		a->setOpen(true);
		profileMap.clear();
		QHash<QString, Profile*> dict = theServer->profiles();
		QHash<QString, Profile*>::const_iterator i;
		for(i = dict.constBegin(); i != dict.constEnd(); ++i)
			profileMap[new Q3ListViewItem(a, i.value()->name())] = i.key();
	}
	{	RemoteServer *theServer = RemoteServer::remoteServer();
		Q3ListViewItem *a = new Q3ListViewItem(theKCMLircBase->theExtensions, i18n("Remote Controls"));
		a->setOpen(true);
		remoteMap.clear();
		QHash<QString, Remote*> dict = theServer->remotes();
		QHash<QString, Remote*>::const_iterator i;
		for(i = dict.constBegin(); i != dict.constEnd(); ++i)
			remoteMap[new Q3ListViewItem(a, i.value()->name())] = i.key();
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
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:", theKCMLircBase->theExtensions->selectedItem()->text(0)));
		if(theKCMLircBase->theExtensions->selectedItem()->text(0) == i18n("Applications"))
			new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Number of Applications"), QString().setNum(theKCMLircBase->theExtensions->selectedItem()->childCount()));
		else if(theKCMLircBase->theExtensions->selectedItem()->text(0) == i18n("Remote Controls"))
			new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Number of Remote Controls"), QString().setNum(theKCMLircBase->theExtensions->selectedItem()->childCount()));
	}
	else if(theKCMLircBase->theExtensions->selectedItem()->parent()->text(0) == i18n("Applications"))
	{
		ProfileServer *theServer = ProfileServer::profileServer();
		const Profile *p = theServer->profiles()[profileMap[theKCMLircBase->theExtensions->selectedItem()]];
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Extension Name"), p->name());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Extension Author"), p->author());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Application Identifier"), p->id());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Number of Actions"), QString().setNum(p->actions().count()));
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:", p->name()));
	}
	else if(theKCMLircBase->theExtensions->selectedItem()->parent()->text(0) == i18n("Remote Controls"))
	{
		RemoteServer *theServer = RemoteServer::remoteServer();
		const Remote *p = theServer->remotes()[remoteMap[theKCMLircBase->theExtensions->selectedItem()]];
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Extension Name"), p->name());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Extension Author"), p->author());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Remote Control Identifier"), p->id());
		new Q3ListViewItem(theKCMLircBase->theInformation, i18n("Number of Buttons"), QString().setNum(p->buttons().count()));
		theKCMLircBase->theInformationLabel->setText(i18n("Information on <b>%1</b>:", p->name()));
	}
}

void KCMLirc::load()
{
	KConfig theConfig("irkickrc");
//	KConfigGroup generalGroup = theConfig.group("General");

	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);
	QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "remotes");
	QDBusMessage response = QDBusConnection::sessionBus().call(m);
	if( response.type() == QDBusMessage::ErrorMessage ){
		kDebug() << response.errorMessage();
	} 
	
	QStringList remotes;
	for( int i = 0; i < response.arguments().size(); ++i){
		remotes << response.arguments().at(i).toString();
	}

	allModes.generateNulls(remotes);

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
	KConfig theConfig("irkickrc");
	allActions.saveToConfig(theConfig);
	allModes.saveToConfig(theConfig);

	theConfig.sync();

	QDBusMessage m = QDBusMessage::createMethodCall("org.kde.irkick", "/IRKick", "", "reloadConfiguration");
	QDBusMessage response = QDBusConnection::sessionBus().call(m);
	if( response.type() == QDBusMessage::ErrorMessage ){
		kDebug() << response.errorMessage();
	} 
	
	emit changed(true);
}

void KCMLirc::configChanged()
{
 // insert your saving code here...
    emit changed(true);
}

// TODO: Take this out when I know how
//extern "C"
//{
//	KDE_EXPORT KCModule *create_kcmlirc(QWidget *parent, const char *)
//	KGlobal::locale()->insertCatalog("kcmlirc");
//		return new KCMLirc(parent);
//	}
//}

#include <ktoolinvocation.h>

#include "kcmlirc.moc"
