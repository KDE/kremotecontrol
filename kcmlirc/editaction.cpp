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
#include <QRegExp>
#include <QLabel>
#include <qradiobutton.h>
#include <QComboBox>
#include <QCheckBox>
#include <q3widgetstack.h>
#include <q3buttongroup.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDomDocument>
#include <QDomElement>


#include <kdebug.h>
#include <klineedit.h>
#include <k3listview.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>

//#include <dcopclient.h>
//#include <irkick_stub.h>

#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "editaction.h"
#include "addaction.h"

EditAction::EditAction(IRAIt action, QWidget *parent, const char *name) //: EditActionBase(parent, name)
{
	theAction = action;

	//KWindowSystem::setState(widget->winId(), NET::StaysOnTop );
	setupUi( this );

	updateApplications();
	updateDCOPApplications();
	
	mainGroup.addButton(theUseDCOP);
	mainGroup.addButton(theUseProfile);
	mainGroup.addButton(theChangeMode);
	
	
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
		(*theAction).setProgram(program);
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
	kDebug() ;
	if(theUseProfile->isChecked())
	{
		theArguments->clear();
		const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[theApplications->currentText()], functionMap[theFunctions->currentText()]);
		if(!a) { arguments.clear(); return; }
		const Q3ValueList<ProfileActionArgument> &p = a->arguments();
		if(p.count() != arguments.count())
		{	arguments.clear();
			for(unsigned i = 0; i < p.count(); i++)
				arguments.append(QVariant(""));
		}
		theArguments->setEnabled(p.count());
		for(unsigned i = 0; i < p.count(); i++)
		{	theArguments->addItem(p[i].comment() + " (" + p[i].type() + ")");
			arguments[i].convert(QVariant::nameToType(p[i].type().toLocal8Bit()));
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
		{	theArguments->addItem(QString().setNum(i + 1) + ": " + (p.name(i).isEmpty() ? p.type(i) : p.name(i) + " (" + p.type(i) + ")"));
			arguments[i].convert(QVariant::nameToType(p.type(i).toLocal8Bit()));
		}
		if(p.count()) updateArgument(0); else updateArgument(-1);
	}
}

void EditAction::updateOptions()
{
	if (theUseProfile->isChecked())
	{
		ProfileServer *theServer = ProfileServer::profileServer();
		if(theApplications->currentIndex() == -1) return;
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
	kDebug() << "in: " << arguments[theArguments->currentIndex()].toString() ;
	int type = arguments[theArguments->currentIndex()].type();
	kDebug() << type ;
	switch(type)
	{
	case QVariant::Int: case QVariant::UInt:
		arguments[theArguments->currentIndex()] = theValueIntNumInput->value();
		break;
	case QVariant::Double:
		arguments[theArguments->currentIndex()] = theValueDoubleNumInput->value();
		break;
	case QVariant::Bool:
		arguments[theArguments->currentIndex()] = theValueCheckBox->isChecked();
		break;
	case QVariant::StringList:
		arguments[theArguments->currentIndex()] = theValueEditListBox->items();
		break;
	default:
		arguments[theArguments->currentIndex()] = theValueLineEdit->text();
	}
	arguments[theArguments->currentIndex()].convert(QVariant::Type(type));
	kDebug() << "out: " << arguments[theArguments->currentIndex()].toString() ;

}

void EditAction::updateArgument(int index)
{
	kDebug() << " i: " << index ;
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
			arguments[index] = backup;
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

	QHash<QString, Profile*> dict = theServer->profiles();
	QHash<QString, Profile*>::const_iterator i;
	for(i = dict.constBegin(); i != dict.constEnd(); ++i)
	{	theApplications->addItem(i.value()->name());
		applicationMap[i.value()->name()] = i.key();
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

	QHash<QString, ProfileAction*> dict = p->actions();
	QHash<QString, ProfileAction*>::const_iterator i;
	for(i = dict.constBegin(); i != dict.constEnd(); ++i)
	{	theFunctions->addItem(i.value()->name());
		functionMap[i.value()->name()] = i.key();
	}
	updateArguments();
}

void EditAction::updateDCOPApplications()
{
	QStringList names;

	theDCOPApplications->clear();
	
	QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
	QStringList allServices = dBusIface->registeredServiceNames();
	
	for(QStringList::const_iterator i = allServices.constBegin(); i != allServices.constEnd(); i++){
		// Use only KDE-Apps
		if(!(*i).contains("org.kde")){
			continue;
		}
		
		// Remove the "org.kde."
		QString name = (*i);
		name.remove(0, 8);
		
		// Remove "human unreadable" entries
		QRegExp r("[a-zA-Z]*");
		if(! r.exactMatch(name)){
			continue;
		}
		
		//remove duplicates
		if(names.contains(name)){
			continue;
		}
		
		theDCOPApplications->insertItem(name);
		nameProgramMap[name] = *i;
	}
	
	
	updateDCOPObjects();
}

void EditAction::updateDCOPObjects()
{
	theDCOPObjects->clear();	

	kDebug() << "ProgramMap: " << nameProgramMap[theDCOPApplications->currentText()];

	QDBusInterface *dBusIface = new QDBusInterface(nameProgramMap[theDCOPApplications->currentText()], "/", "org.freedesktop.DBus.Introspectable");
	QDBusReply<QString> response = dBusIface->call("Introspect");
	
	QDomDocument domDoc;
	domDoc.setContent(response);
	
	QDomElement node = domDoc.documentElement();
	QDomElement child = node.firstChildElement();
	while (!child.isNull()) {
		kDebug() << child.tagName() << ":" << child.attribute(QLatin1String("name"));
		if (child.tagName() == QLatin1String("node")) {
			theDCOPObjects->insertItem(child.attribute(QLatin1String("name")));
		}
		child = child.nextSiblingElement();
	}	


	


	updateDCOPFunctions();
}

void EditAction::updateDCOPFunctions()
{
	theDCOPFunctions->clear();

 	QDBusInterface *dBusIface = new QDBusInterface(nameProgramMap[theDCOPApplications->currentText()], "/" + theDCOPObjects->currentText(), "org.freedesktop.DBus.Introspectable");
 	QDBusReply<QString> response = dBusIface->call("Introspect");
 	
	QDomDocument domDoc;
	domDoc.setContent(response);	

	QDomElement node = domDoc.documentElement();
 	QDomElement child = node.firstChildElement();

	QString function;
 
	while (!child.isNull()) {
		if (child.tagName() == QLatin1String("interface")) {
			if(child.attribute("name") == "org.freedesktop.DBus.Properties" || 
			   child.attribute("name") == "org.freedesktop.DBus.Introspectable"){
				child = child.nextSiblingElement();
				continue;	
			}
			QDomElement subChild = child.firstChildElement();
			while(!subChild.isNull()){
				if(subChild.tagName() == QLatin1String("method")){
					QString method = subChild.attribute(QLatin1String("name"));
					kDebug() << "Method: " << method;
					function = "QString " + method + "(";
					QDomElement arg = subChild.firstChildElement();
					QString argStr;
					while(!arg.isNull()){
						if(arg.tagName() == QLatin1String("arg")){
							if(arg.attribute(QLatin1String("direction")) == "in"){
								if(!argStr.isEmpty()){
									argStr += ", ";
								}
								if(arg.attribute(QLatin1String("type")) == "i" ){
									argStr += "int";
								} else if(arg.attribute(QLatin1String("type")) == "u"){
									argStr += "uint";
								} else if(arg.attribute(QLatin1String("type")) == "s"){
									argStr += "QString";
								} else if(arg.attribute(QLatin1String("type")) == "b"){
									argStr += "bool";
								} else if(arg.attribute(QLatin1String("type")) == "d"){
									argStr += "double";
								} else if(arg.attribute(QLatin1String("type")) == "as"){
									argStr += "QStringList";
								} else if(arg.attribute(QLatin1String("type")) == "ay"){
									argStr += "QByteArray";
								} else if(arg.attribute(QLatin1String("type")) == "(iii)"){
									kDebug() << "got a (iii) type";
									QString helper = arg.attribute("name");
									arg = arg.nextSiblingElement();
									argStr += arg.attribute(QLatin1String("value"));
									argStr += " " + helper;
									arg = arg.nextSiblingElement();
									continue;
								} else {
									argStr += arg.attribute(QLatin1String("type"));
								}
								argStr += " " + arg.attribute(QLatin1String("name"));
								kDebug() << "Arg: " << argStr;
							}
						}
						arg = arg.nextSiblingElement();
					}
					function +=  argStr + ")";
					theDCOPFunctions->insertItem(function);
				}
				subChild = subChild.nextSiblingElement();
			}
		}
		child = child.nextSiblingElement();
	}
		
	updateArguments();
}

void EditAction::addItem(QString item){
	theModes->addItem(item);
}

#include "editaction.moc"
