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
#include "kcmlirc.h"

typedef KGenericFactory<KCMLirc, QWidget> theFactory;
K_EXPORT_COMPONENT_FACTORY(kcmlirc, theFactory("kcmlirc"));

KCMLirc::KCMLirc(QWidget *parent, const char *name, QStringList /*args*/) : KCModule(parent, name), DCOPObject("KCMLirc")
{
	(new QHBoxLayout(this))->setAutoAdd(true);
	theKCMLircBase = new KCMLircBase(this);
	connect(theKCMLircBase->theModes, SIGNAL( currentChanged(QListViewItem *) ), this, SLOT( updateActions() ));
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
	if(!theKCMLircBase->theModes->currentItem() || !theKCMLircBase->theModes->currentItem()->parent()) return;
	Mode m = modeMap[theKCMLircBase->theModes->currentItem()];

	AddAction theDialog(this, 0, m);
	if(theDialog.exec() == QDialog::Accepted)
		if(theDialog.theObjects->currentItem())
		if(theDialog.theObjects->currentItem()->parent())
		if(theDialog.theButtons->currentItem())
		if(theDialog.theFunctions->currentItem())
		{
			IRAction a;
			a.setRemote(m.remote());
			a.setMode(m.name());
			a.setButton(theDialog.theButtons->currentItem()->text(0));
			a.setProgram(theDialog.theObjects->currentItem()->parent()->text(0));
			a.setObject(theDialog.theObjects->currentItem()->text(0));
			a.setMethod(theDialog.theFunctions->currentItem()->text(2));
			a.setRepeat(theDialog.theRepeat->isChecked());
			Arguments args;
			theDialog.theParameters->setSorting(0);
			for(QListViewItem *i = theDialog.theParameters->firstChild(); i; i = i->nextSibling())
			{	kdDebug() << "Got arg" << i->text(2) << ":: " << i->text(3) << endl;
				QVariant v(i->text(3));
				v.cast(QVariant::nameToType(i->text(1)));
				args += v;
			}
			a.setArguments(args);
			allActions.addAction(a);
			updateActions();
			// NEED TO IMPLEMENT set...() methods!!!!
			emit changed(true);
		}
}

void KCMLirc::slotRemoveAction()
{
	if(!theKCMLircBase->theActions->currentItem()) return;

	IRAIt i = actionMap[theKCMLircBase->theActions->currentItem()];
	kdDebug() << (*i).button() << endl;
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

}

void KCMLirc::slotStartListen()
{
	IRKick_stub IRKick("irkick", "IRKick");
	IRKick.stealNextPress(DCOPClient::mainClient()->appId(), "KCMLirc", "gotButton");
}

void KCMLirc::updateActions()
{
	theKCMLircBase->theActions->clear();
	actionMap.clear();
	if(!theKCMLircBase->theModes->currentItem()) return;
	Mode m = modeMap[theKCMLircBase->theModes->currentItem()];

	theKCMLircBase->theModeLabel->setText(m.remote() + ": " + (m.name() == "" ? "<i>Always</i>" : ("<b>" + m.name() + "</b>")));
	IRAItList l = allActions.findByMode(m);
	for(IRAItList::iterator i = l.begin(); i != l.end(); i++)
		actionMap[new KListViewItem(theKCMLircBase->theActions, (**i).button(), (**i).program(), (**i).function(), (**i).arguments().toString(), (**i).repeat() ? "Yes" : "No")] = *i;
}

void KCMLirc::gotButton(QString remote, QString button)
{
	emit haveButton(remote, button);
}

void KCMLirc::updateModes()
{
	theKCMLircBase->theModes->clear();
	modeMap.clear();

	IRKick_stub IRKick("irkick", "IRKick");
	QStringList remotes = IRKick.remotes();
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); i++)
	{	QListViewItem *a = new QListViewItem(theKCMLircBase->theModes, *i);		// TODO: make *i into nice name with wise singleton
		modeMap[a] = Mode(*i, "");	// the null mode
		ModeList l = allModes.getModes(*i);
		for(ModeList::iterator j = l.begin(); j != l.end(); j++)
			modeMap[new QListViewItem(a, (*j).name())] = *j;
	}
}

void KCMLirc::load()
{
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);
	allModes.loadFromConfig(theConfig);

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
