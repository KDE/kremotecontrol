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
#include "kcmlirc.h"

typedef KGenericFactory<KCMLirc, QWidget> theFactory;
K_EXPORT_COMPONENT_FACTORY(kcmlirc, theFactory("kcmlirc"));

KCMLirc::KCMLirc(QWidget *parent, const char *name, QStringList /*args*/) : KCModule(parent, name), DCOPObject("KCMLirc")
{
	// place widgets here
	(new QHBoxLayout(this))->setAutoAdd(true);
	theKCMLircBase = new KCMLircBase(this);
	connect(theKCMLircBase->theButtons, SIGNAL( currentChanged(QListViewItem *) ), this, SLOT( updateActions() ));
	connect((QObject *)(theKCMLircBase->theListen), SIGNAL( clicked() ), this, SLOT( slotStartListen() ));
	connect((QObject *)(theKCMLircBase->theAddAction), SIGNAL( clicked() ), this, SLOT( slotAddAction() ));
	connect((QObject *)(theKCMLircBase->theRemoveAction), SIGNAL( clicked() ), this, SLOT( slotRemoveAction() ));
	load();
}

KCMLirc::~KCMLirc()
{
}

void KCMLirc::slotAddAction()
{
	AddAction theDialog(this, 0);
	if(theDialog.exec() == QDialog::Accepted)
		if(theKCMLircBase->theButtons->currentItem())
		if(theKCMLircBase->theButtons->currentItem()->parent())
		if(theDialog.theObjects->currentItem())
		if(theDialog.theObjects->currentItem()->parent())
		if(theDialog.theFunctions->currentItem())
		{
			IRAction a;
			a.setRemote(theKCMLircBase->theButtons->currentItem()->parent()->text(0));
			a.setButton(theKCMLircBase->theButtons->currentItem()->text(0));
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
	// TODO: need guards
	allActions[qMakePair(theKCMLircBase->theButtons->currentItem()->parent()->text(0),
				theKCMLircBase->theButtons->currentItem()->text(0))].erase
				(actionMap[theKCMLircBase->theActions->currentItem()]);
	emit changed(true);
	updateActions();
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
	QListViewItem *current = theKCMLircBase->theButtons->currentItem();
	if(current) if(current->parent())
	{	QString button = current->text(0);
		QString remote = current->parent()->text(0);
		theKCMLircBase->theButtonLabel->setText(remote + ": <b>" + button + "</b>");
		for(QValueList<IRAction>::iterator i = allActions[qMakePair(remote, button)].begin(); i != allActions[qMakePair(remote, button)].end(); i++)
			actionMap[new KListViewItem(theKCMLircBase->theActions, (*i).program(), (*i).function(), (*i).arguments().toString(), (*i).repeat() ? "Yes" : "No")] = i;
	}
}

void KCMLirc::gotButton(QString remote, QString button)
{
	for(QListViewItem *i = theKCMLircBase->theButtons->firstChild(); i; i = i->nextSibling())
		if(i->text(0) == remote)
		{
			for(i = i->firstChild(); i; i = i->nextSibling())
				if(i->text(0) == button)
				{
					theKCMLircBase->theButtons->setCurrentItem(i);
					theKCMLircBase->theButtons->ensureItemVisible(i);
					break;
				}
			break;
		}
}

void KCMLirc::updateRemotes()
{
	theKCMLircBase->theButtons->clear();
	QListViewItem *a;

	IRKick_stub IRKick("irkick", "IRKick");
	QStringList remotes = IRKick.remotes();
	for(QStringList::iterator i = remotes.begin(); i != remotes.end(); i++)
	{	a = new QListViewItem(theKCMLircBase->theButtons, *i);
		QStringList buttons = IRKick.buttons(*i);
		for(QStringList::iterator j = buttons.begin(); j != buttons.end(); j++)
			new QListViewItem(a, *j);
	}
}

void KCMLirc::load()
{
	KSimpleConfig theConfig("irkickrc");
	allActions.loadFromConfig(theConfig);

	updateRemotes();
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
