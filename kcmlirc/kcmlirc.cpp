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

IRAction::IRAction(const QString &theProgram, const QString &theObject, const QString &theMethod, const QValueList<QVariant> &theArguments, const QString &theRemote, const QString &theButton, bool theRepeat)
{
	Program = theProgram;
	Object = theObject;
	Method = theMethod;
	Arguments = theArguments;
	Remote = theRemote;
	Button = theButton;
	Repeat = theRepeat;
}

const QString IRAction::ArgumentString() const
{
	QString ret = "";
	for(QValueList<QVariant>::const_iterator i = Arguments.begin(); i != Arguments.end(); i++)
	{	QString s = (*i).toString();
		if(s == QString::null) s = "...";
		if(i != Arguments.begin()) ret += ", ";
		ret += s;
	}
	return ret;
}

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
	{
		// TODO: need guards here
		IRAction a;
		a.Remote = theKCMLircBase->theButtons->currentItem()->parent()->text(0);
		a.Button = theKCMLircBase->theButtons->currentItem()->text(0);
		a.Program = theDialog.theObjects->currentItem()->parent()->text(0);
		a.Object = theDialog.theObjects->currentItem()->text(0);
		a.Method = theDialog.theFunctions->currentItem()->text(2);
		a.Repeat = theDialog.theRepeat->isChecked();
		a.Arguments.clear();
		theDialog.theParameters->setSorting(0);
		for(QListViewItem *i = theDialog.theParameters->firstChild(); i; i = i->nextSibling())
		{	kdDebug() << "Got arg" << i->text(2) << ":: " << i->text(3) << endl;
			QVariant v(i->text(3));
			v.cast(QVariant::nameToType(i->text(1)));
			a.Arguments += v;
		}
		allActions[qMakePair(a.Remote, a.Button)] += a;
		updateActions();
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
			actionMap[new KListViewItem(theKCMLircBase->theActions, (*i).Program, (*i).Object + "::" + (*i).Method, (*i).ArgumentString(), (*i).Repeat ? "Yes" : "No")] = i;
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

void KCMLirc::load()
{
	KSimpleConfig theConfig("irkickrc");
	int numBindings = theConfig.readNumEntry("Bindings");
	for(int i = 0; i < numBindings; i++)
	{	QString Binding = "Binding" + QString().setNum(i);

		int numArguments = theConfig.readNumEntry(Binding + "Arguments");
		QValueList<QVariant> Arguments;
		for(int j = 0; j < numArguments; j++)
		{	QVariant::Type theType = (QVariant::Type)theConfig.readNumEntry(Binding + "ArgumentType" + QString().setNum(j), QVariant::String);
			Arguments += theConfig.readPropertyEntry(Binding + "Argument" + QString().setNum(j), theType == QVariant::CString ? QVariant::String : theType);
			Arguments.last().cast(theType);
		}

		allActions[qMakePair(theConfig.readEntry(Binding + "Remote"), theConfig.readEntry(Binding + "Button"))]
			 += IRAction( theConfig.readEntry(Binding + "Program"), theConfig.readEntry(Binding + "Object"),
					theConfig.readEntry(Binding + "Method"), Arguments, theConfig.readEntry(Binding + "Remote"),
					theConfig.readEntry(Binding + "Button"), theConfig.readBoolEntry(Binding + "Repeat"));
	}

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

	// purge all bindings from entry
	int numBindings = theConfig.readNumEntry("Bindings");
	for(int i = 0; i < numBindings; i++)
	{	QString Binding = "Binding" + QString().setNum(i);
		int numArguments = theConfig.readNumEntry(Binding + "Arguments");
		for(int j = 0; j < numArguments; j++)
		{	theConfig.deleteEntry(Binding + "Argument" + QString().setNum(j));
			theConfig.deleteEntry(Binding + "ArgumentType" + QString().setNum(j));
		}
		theConfig.deleteEntry(Binding + "Arguments"); theConfig.deleteEntry(Binding + "Program");
		theConfig.deleteEntry(Binding + "Object"); theConfig.deleteEntry(Binding + "Method");
		theConfig.deleteEntry(Binding + "Remote"); theConfig.deleteEntry(Binding + "Button");
		theConfig.deleteEntry(Binding + "Repeat");
	}

	// save the new ones
	int index = 0;
	for(IRActions::iterator k = allActions.begin(); k != allActions.end(); k++)
	{	QValueList<IRAction> &theActions = k.data();
		for(QValueList<IRAction>::iterator i = theActions.begin(); i != theActions.end(); i++,index++)
		{	QString Binding = "Binding" + QString().setNum(index);
			theConfig.writeEntry(Binding + "Arguments", (*i).Arguments.count());
			for(unsigned j = 0; j < (*i).Arguments.count(); j++)
			{	QVariant::Type preType = (*i).Arguments[j].type();
				if(preType == QVariant::CString) (*i).Arguments[j].cast(QVariant::String);
				theConfig.writeEntry(Binding + "Argument" + QString().setNum(j), (*i).Arguments[j]);
				theConfig.writeEntry(Binding + "ArgumentType" + QString().setNum(j), preType);
			}
			theConfig.writeEntry(Binding + "Program", (*i).Program);
			theConfig.writeEntry(Binding + "Object", (*i).Object);
			theConfig.writeEntry(Binding + "Method", (*i).Method);
			theConfig.writeEntry(Binding + "Remote", (*i).Remote);
			theConfig.writeEntry(Binding + "Button", (*i).Button);
			theConfig.writeEntry(Binding + "Repeat", (*i).Repeat);
		}
	}
	theConfig.writeEntry("Bindings", index + 1);

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
