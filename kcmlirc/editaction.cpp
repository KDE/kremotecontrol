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
#include <qwidgetstack.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <klineedit.h>
#include <klistview.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>

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
	theDoBefore->setChecked((*theAction).doBefore());
	theDoAfter->setChecked((*theAction).doAfter());
	theDontSend->setChecked((*theAction).ifMulti() == IM_DONTSEND);
	theSendToTop->setChecked((*theAction).ifMulti() == IM_SENDTOTOP);
	theSendToBottom->setChecked((*theAction).ifMulti() == IM_SENDTOBOTTOM);
	theSendToAll->setChecked((*theAction).ifMulti() == IM_SENDTOALL);

	if((*theAction).isModeChange())
	{	// change mode
		theChangeMode->setChecked(true);
		if((*theAction).object().isEmpty())
			theModes->setCurrentText(i18n("[Exit current mode]"));
		else
			theModes->setCurrentText((*theAction).object());
	}
	else if((*theAction).isJustStart())
	{	// profile action
		theUseProfile->setChecked(true);
		const Profile *p = ProfileServer::profileServer()->profiles()[(*theAction).program()];
		theApplications->setCurrentText(p->name());
		updateFunctions();
		updateArguments();
		theJustStart->setChecked(true);
	}
	else if(ProfileServer::profileServer()->getAction((*theAction).program(), (*theAction).object(), (*theAction).method().prototype()))
	{	// profile action
		theUseProfile->setChecked(true);
		const ProfileAction *a = ProfileServer::profileServer()->getAction((*theAction).program(), (*theAction).object(), (*theAction).method().prototype());
		theApplications->setCurrentText(a->profile()->name());
		updateFunctions();
		theFunctions->setCurrentText(a->name());
		arguments = (*theAction).arguments();
		updateArguments();
		theNotJustStart->setChecked(true);
	}
	else
	{	// dcop action
		theUseDCOP->setChecked(true);
		theDCOPApplications->setCurrentText((*theAction).program());
		updateDCOPObjects();
		theDCOPObjects->setCurrentText((*theAction).object());
		updateDCOPFunctions();
		theDCOPFunctions->setCurrentText((*theAction).method().prototype());
		arguments = (*theAction).arguments();
		updateArguments();
	}
	updateOptions();
}

void EditAction::writeBack()
{
	if(theChangeMode->isChecked())
	{
		(*theAction).setProgram("");
		if(theModes->currentText() == i18n("[Exit current mode]"))
			(*theAction).setObject("");
		else
			(*theAction).setObject(theModes->currentText());
		(*theAction).setDoBefore(theDoBefore->isChecked());
		(*theAction).setDoAfter(theDoAfter->isChecked());
	}
	else if(theUseProfile->isChecked() && (
						ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()])
						||
						theJustStart->isChecked() && ProfileServer::profileServer()->profiles()[theApplications->currentText()]
						)
		)
	{	if(theJustStart->isChecked())
		{	(*theAction).setProgram(ProfileServer::profileServer()->profiles()[applicationMap[theApplications->currentText()]]->id());
			(*theAction).setObject("");
		}
		else
		{	const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]);
			(*theAction).setProgram(ProfileServer::profileServer()->profiles()[applicationMap[theApplications->currentText()]]->id());
			(*theAction).setObject(a->objId());
			(*theAction).setMethod(a->prototype());
			(*theAction).setArguments(arguments);
		}
	}
	else
	{
		(*theAction).setProgram(program);//theDCOPApplications->currentText());
		(*theAction).setObject(theDCOPObjects->currentText());
		(*theAction).setMethod(theDCOPFunctions->currentText());
		(*theAction).setArguments(arguments);
	}
	(*theAction).setRepeat(theRepeat->isChecked());
	(*theAction).setAutoStart(theAutoStart->isChecked());
	(*theAction).setUnique(isUnique);
	(*theAction).setIfMulti(theDontSend->isChecked() ? IM_DONTSEND : theSendToTop->isChecked() ? IM_SENDTOTOP : theSendToBottom->isChecked() ? IM_SENDTOBOTTOM : IM_SENDTOALL);
}

void EditAction::updateArguments()
{
	kdDebug() << k_funcinfo << endl;
	if(theUseProfile->isChecked())
	{
		theArguments->clear();
		const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]);
		if(!a) { arguments.clear(); return; }
		const QValueList<ProfileActionArgument> &p = a->arguments();
		if(p.count() != arguments.count())
		{	arguments.clear();
			for(unsigned i = 0; i < p.count(); i++)
				arguments.append(QVariant(""));
		}
		theArguments->setEnabled(p.count());
		for(unsigned i = 0; i < p.count(); i++)
		{	theArguments->insertItem(p[i].comment() + " (" + p[i].type() + ")");
			arguments[i].cast(QVariant::nameToType(p[i].type().utf8()));
		}
		if(p.count()) updateArgument(0); else updateArgument(-1);
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
		for(unsigned i = 0; i < p.count(); i++)
		{	theArguments->insertItem(QString().setNum(i + 1) + ": " + (p.name(i).isEmpty() ? p.type(i) : p.name(i) + " (" + p.type(i) + ")"));
			arguments[i].cast(QVariant::nameToType(p.type(i).utf8()));
		}
		if(p.count()) updateArgument(0); else updateArgument(-1);
	}
}

void EditAction::updateOptions()
{
	if (theUseProfile->isChecked())
	{
		ProfileServer *theServer = ProfileServer::profileServer();
		if(!theApplications->currentItem()) return;
		const Profile *p = theServer->profiles()[applicationMap[theApplications->currentText()]];
		isUnique = p->unique();
	}
	else if (theUseDCOP->isChecked())
	{
		if(theDCOPApplications->currentText().isNull() || theDCOPApplications->currentText().isEmpty()) return;
		program = theDCOPApplications->currentText();
		isUnique = uniqueProgramMap[theDCOPApplications->currentText()];
	}
	else
		isUnique = true;

	theIMLabel->setEnabled(!isUnique);
	theIMGroup->setEnabled(!isUnique);
	theDontSend->setEnabled(!isUnique);
	theSendToTop->setEnabled(!isUnique);
	theSendToBottom->setEnabled(!isUnique);
	theSendToAll->setEnabled(!isUnique);
}

// called when the textbox/checkbox/whatever changes value
void EditAction::slotParameterChanged()
{
	kdDebug() << "in: " << arguments[theArguments->currentItem()].toString() << endl;
	int type = arguments[theArguments->currentItem()].type();
	kdDebug() << type << endl;
	switch(type)
	{
	case QVariant::Int: case QVariant::UInt:
		arguments[theArguments->currentItem()].asInt() = theValueIntNumInput->value();
		break;
	case QVariant::Double:
		arguments[theArguments->currentItem()].asDouble() = theValueDoubleNumInput->value();
		break;
	case QVariant::Bool:
		arguments[theArguments->currentItem()].asBool() = theValueCheckBox->isChecked();
		break;
	case QVariant::StringList:
		arguments[theArguments->currentItem()].asStringList() = theValueEditListBox->items();
		break;
	default:
		arguments[theArguments->currentItem()].asString() = theValueLineEdit->text();
	}
	arguments[theArguments->currentItem()].cast(QVariant::Type(type));
	kdDebug() << "out: " << arguments[theArguments->currentItem()].toString() << endl;

}

void EditAction::updateArgument(int index)
{
	kdDebug() << k_funcinfo << " i: " << index << endl;
	if(index >= 0)
	{	switch(arguments[index].type())
		{
		case QVariant::Int: case QVariant::UInt:
			theValue->raiseWidget(2);
			theValueIntNumInput->setValue(arguments[index].toInt());
			break;
		case QVariant::Double:
			theValue->raiseWidget(3);
			theValueDoubleNumInput->setValue(arguments[index].toDouble());
			break;
		case QVariant::Bool:
			theValue->raiseWidget(1);
			theValueCheckBox->setChecked(arguments[index].toBool());
			break;
		case QVariant::StringList:
		{	theValue->raiseWidget(4);
			QStringList backup = arguments[index].toStringList();
			// backup needed because calling clear will kill what ever has been saved.
			theValueEditListBox->clear();
			theValueEditListBox->insertStringList(backup);
			arguments[index].asStringList() = backup;
			break;
		}
		default:
			theValue->raiseWidget(0);
			theValueLineEdit->setText(arguments[index].toString());
		}
		theValue->setEnabled(true);
	}
	else
	{	theValueLineEdit->setText("");
		theValueCheckBox->setChecked(false);
		theValueIntNumInput->setValue(0);
		theValueDoubleNumInput->setValue(0.0);
		theValue->setEnabled(false);
	}
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
	if(theApplications->currentText().isNull() || theApplications->currentText().isEmpty()) return;

	const Profile *p = theServer->profiles()[applicationMap[theApplications->currentText()]];

	QDict<ProfileAction> dict = p->actions();
	for(QDictIterator<ProfileAction> i(dict); i.current(); ++i)
	{	theFunctions->insertItem(i.current()->name());
		functionMap[i.current()->name()] = i.currentKey();
	}
	updateArguments();
}

void EditAction::updateDCOPApplications()
{
	QStringList names;

	theDCOPApplications->clear();
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	QCStringList theApps = theClient->registeredApplications();
	for(QCStringList::iterator i = theApps.begin(); i != theApps.end(); ++i)
	{
		if(!QString(*i).find("anonymous")) continue;
		QRegExp r("(.*)-[0-9]+");
		QString name = r.exactMatch(QString(*i)) ? r.cap(1) : *i;
		if(names.contains(name)) continue;
		names += name;

		theDCOPApplications->insertItem(name);
		uniqueProgramMap[name] = name == QString(*i);
		nameProgramMap[name] = *i;


	}
	updateDCOPObjects();
}

void EditAction::updateDCOPObjects()
{
	theDCOPObjects->clear();
	DCOPClient *theClient = KApplication::kApplication()->dcopClient();
	if(theDCOPApplications->currentText().isNull() || theDCOPApplications->currentText().isEmpty()) return;
	QCStringList theObjects = theClient->remoteObjects(nameProgramMap[theDCOPApplications->currentText()].utf8());
	if(!theObjects.size() && theDCOPApplications->currentText() == (*theAction).program()) theDCOPObjects->insertItem((*theAction).object());
	for(QCStringList::iterator j = theObjects.begin(); j != theObjects.end(); ++j)
		if(*j != "ksycoca" && *j != "qt" && AddAction::getFunctions(nameProgramMap[theDCOPApplications->currentText()], *j).count())
			theDCOPObjects->insertItem(QString::fromUtf8(*j));
	updateDCOPFunctions();
}

void EditAction::updateDCOPFunctions()
{
	theDCOPFunctions->clear();
	if(theDCOPApplications->currentText().isNull() || theDCOPApplications->currentText().isEmpty()) return;
	QStringList functions = AddAction::getFunctions(nameProgramMap[theDCOPApplications->currentText()], theDCOPObjects->currentText());
	if(!functions.size() && theDCOPApplications->currentText() == (*theAction).program()) theDCOPFunctions->insertItem((*theAction).method().prototype());
	for(QStringList::iterator i = functions.begin(); i != functions.end(); ++i)
		theDCOPFunctions->insertItem(*i);
	updateArguments();
}


#include "editaction.moc"
