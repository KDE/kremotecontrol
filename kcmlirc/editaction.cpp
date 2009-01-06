/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood
  */

#include "editaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "addaction.h"

#include <QRegExp>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDomDocument>
#include <QDomElement>

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>

EditAction::EditAction(IRAction *action, QWidget *parent, const bool &modal): KDialog(parent)
{



    theAction = action;
    editActionBaseWidget = new EditActionBaseWidget();
    setMainWidget(editActionBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    //TODO: Layout theValue
    editActionBaseWidget->theValue->layout()->setMargin(0);

//    QMetaObject::connectSlotsByName(this->mainWidget());

    connect(editActionBaseWidget->theApplications,SIGNAL(activated(QString)),this,SLOT(updateFunctions()));
    connect(editActionBaseWidget->theApplications,SIGNAL(activated(QString)),this,SLOT(updateOptions()));
    connect(editActionBaseWidget->theFunctions,SIGNAL(activated(QString)),this,SLOT(updateArguments()));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),this,SLOT(updateOptions()));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setChecked(bool)));

    connect(editActionBaseWidget->theDBusApplications,SIGNAL(activated(QString)),this,SLOT(updateDCOPObjects()));
    connect(editActionBaseWidget->theDBusApplications,SIGNAL(activated(QString)),this,SLOT(updateOptions()));
    connect(editActionBaseWidget->theDBusFunctions,SIGNAL(activated(QString)),this,SLOT(updateArguments()));
    connect(editActionBaseWidget->theDBusObjects,SIGNAL(activated(QString)),this,SLOT(updateDCOPFunctions()));


    connect(editActionBaseWidget->theValueCheckBox,SIGNAL(toggled(bool)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueDoubleNumInput,SIGNAL(valueChanged(double)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueEditListBox,SIGNAL(changed()),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueIntNumInput,SIGNAL(valueChanged(int)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueLineEdit,SIGNAL(textChanged(QString)),this,SLOT(slotParameterChanged()));

    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theArguments, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theApplications, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theNotJustStart, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theJustStart, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theNotJustStart, SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theUseProfileAppLabel, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),this, SLOT(updateFunctions()));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),this, SLOT(updateArguments()));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)), this, SLOT(updateOptions()));

    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat, SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theArguments,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theNotJustStart,SIGNAL(toggled(bool)),this, SLOT(updateOptions()));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theModes, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoAfter, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoBefore, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theAppDbusOptionsLabel, SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),this, SLOT(updateOptions()));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusApplications, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusObjects, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusFunctions, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusObjectsLabel, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusApplicationsLabel, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theAppDbusOptionsLabel, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), editActionBaseWidget->theDBusFunctionsLabel, SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), this, SLOT(updateArguments()));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)), this, SLOT(updateOptions()));

    mainGroup.addButton(editActionBaseWidget->theUseDBus);
    mainGroup.addButton(editActionBaseWidget->theUseProfile);
    mainGroup.addButton(editActionBaseWidget->theChangeMode);

    initDBusApplications();
    initApplications();
    readFrom();
}

EditAction::~EditAction()
{
}

void EditAction::on_theUseProfile_toggled(bool toogle)
{
//  editActionBaseWidget->theArguments->setEnabled(toogle);
//  editActionBaseWidget->theApplications->setEnabled(toogle);
//  editActionBaseWidget->theAutoStart->setEnabled(toogle);
//  editActionBaseWidget->theRepeat->setEnabled(toogle);
//  editActionBaseWidget->theNotJustStart->setEnabled(toogle);
//  editActionBaseWidget->theJustStart->setEnabled(toogle);
//  editActionBaseWidget->theNotJustStart->setChecked(toogle);
//  editActionBaseWidget->theUseProfileAppLabel->setEnabled(toogle);
//  editActionBaseWidget->theFunctions->setEnabled(toogle);
//  updateFunctions();
//  updateArguments();
//  updateOptions();
}

void EditAction::on_theNotJustStart_toggled(bool toogle)
{
//  theRepeat->setChecked(toogle);
//  theAutoStart->setEnabled(toogle);
//  theRepeat->setEnabled(toogle);
//  theArguments->setEnabled(toogle);
//  theFunctions->setEnabled(toogle);
//  updateOptions();
}

void EditAction::on_theChangeMode_toggled(bool toogle)
{
//  theModes->setEnabled(toogle);
//   theDoAfter->setEnabled(toogle);
//   theDoBefore->setEnabled(toogle);
//   theAppDbusOptionsLabel->setDisabled(toogle);
//   updateOptions();
}

void EditAction::on_theUseDBus_toggled(bool toogle)
{
//  editActionBaseWidget->theDBusApplications->setEnabled(toogle);
//  editActionBaseWidget->theDBusObjects->setEnabled(toogle);
//  editActionBaseWidget->theDBusFunctions->setEnabled(toogle);
//  editActionBaseWidget->theDBusObjectsLabel->setEnabled(toogle);
//  editActionBaseWidget->theDBusApplicationsLabel->setEnabled(toogle);
//  editActionBaseWidget->theAppDbusOptionsLabel->setEnabled(toogle);
//  editActionBaseWidget->theDBusFunctionsLabel->setEnabled(toogle);
//  updateArguments();
//  updateOptions();
}

void EditAction::readFrom()
{
  editActionBaseWidget->theRepeat->setChecked(theAction->repeat());
  editActionBaseWidget->theAutoStart->setChecked(theAction->autoStart());
  editActionBaseWidget->theDoBefore->setChecked(theAction->doBefore());
  editActionBaseWidget->theDoAfter->setChecked(theAction->doAfter());
  editActionBaseWidget->theDontSend->setChecked(theAction->ifMulti() == IM_DONTSEND);
  editActionBaseWidget->theSendToTop->setChecked(theAction->ifMulti() == IM_SENDTOTOP);
  editActionBaseWidget->theSendToBottom->setChecked(theAction->ifMulti() == IM_SENDTOBOTTOM);
  editActionBaseWidget->theSendToAll->setChecked(theAction->ifMulti() == IM_SENDTOALL);

  if (theAction->isModeChange()) { // change mode
    editActionBaseWidget->theChangeMode->setChecked(true);
    if (theAction->object().isEmpty())
      editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(i18n("[Exit current mode]")));
    else
      editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(theAction->object()));
  } else if (theAction->isJustStart()) { // profile action
    editActionBaseWidget->theUseProfile->setChecked(true);
    const Profile *p = ProfileServer::profileServer()->profiles()[theAction->program()];
    editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(p->name()));
    editActionBaseWidget->theJustStart->setChecked(true);
  } else if (ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype())) { // profile action
    editActionBaseWidget->theUseProfile->setChecked(true);
    const ProfileAction *a = ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());
    editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(a->profile()->name()));
    editActionBaseWidget->theFunctions->setCurrentIndex(editActionBaseWidget->theFunctions->findText(a->name()));
    arguments = theAction->arguments();
    editActionBaseWidget->theNotJustStart->setChecked(true);
  } else { // DBus action
    editActionBaseWidget->theUseDBus->setChecked(true);
    QString program = theAction->program();
    editActionBaseWidget->theDBusApplications->setCurrentIndex(editActionBaseWidget->theDBusApplications->findText(program.remove(0, 8)));
    updateDBusObjects();
    editActionBaseWidget->theDBusObjects->setCurrentIndex(editActionBaseWidget->theDBusObjects->findText(theAction->object()));
    updateDBusFunctions();
    editActionBaseWidget->theDBusFunctions->setCurrentIndex(editActionBaseWidget->theDBusFunctions->findText(theAction->method().prototype()));
    arguments = theAction->arguments();
  }
}

void EditAction::writeBack()
{
  if (editActionBaseWidget->theChangeMode->isChecked()) {
    theAction->setProgram("");
    if (editActionBaseWidget->theModes->currentText() == i18n("[Exit current mode]"))
      theAction->setObject("");
    else
      theAction->setObject(editActionBaseWidget->theModes->currentText());
    theAction->setDoBefore(editActionBaseWidget->theDoBefore->isChecked());
    theAction->setDoAfter(editActionBaseWidget->theDoAfter->isChecked());
  } else if (editActionBaseWidget->theUseProfile->isChecked()) {
      QString application = applicationMap[editActionBaseWidget->theApplications->currentText()];
      QString function = functionMap[editActionBaseWidget->theFunctions->currentText()];
      const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(applicationMap[application], functionMap[function]);
      if( profileAction != 0 || (editActionBaseWidget->theJustStart->isChecked() &&  ProfileServer::profileServer()->profiles()[application])) {
            if (editActionBaseWidget->theJustStart->isChecked()) {
              theAction->setProgram(ProfileServer::profileServer()->profiles()[applicationMap[editActionBaseWidget->theApplications->currentText()]]->id());
              theAction->setObject("");
            } else {
              theAction->setProgram(ProfileServer::profileServer()->profiles()[applicationMap[application]]->id());
              kDebug() << "wrote back: " << applicationMap[application];
              theAction->setObject(profileAction->objId());
              theAction->setMethod(profileAction->prototype());
              theAction->setArguments(arguments);
            }
  }
  } else {
    theAction->setProgram("org.kde." + program);
    theAction->setObject(editActionBaseWidget->theDBusObjects->currentText());
    theAction->setMethod(editActionBaseWidget->theDBusFunctions->currentText());
    theAction->setArguments(arguments);
  }
  theAction->setRepeat(editActionBaseWidget->theRepeat->isChecked());
  theAction->setAutoStart(editActionBaseWidget->theAutoStart->isChecked());
  theAction->setUnique(isUnique);
  if(editActionBaseWidget->theDontSend->isChecked()){
    theAction->setIfMulti(IM_DONTSEND);
  }else if ( editActionBaseWidget->theSendToTop->isChecked()){
    theAction->setIfMulti(IM_SENDTOTOP);
  }else if( editActionBaseWidget->theSendToBottom->isChecked()){
    theAction->setIfMulti(IM_SENDTOBOTTOM);
  }else{
    theAction->setIfMulti(IM_SENDTOALL);
  }
//  theAction->setIfMulti(addActionBaseWidget->theDontSend->isChecked() ? IM_DONTSEND : addActionBaseWidget->theSendToTop->isChecked() ? IM_SENDTOTOP : addActionBaseWidget->theSendToBottom->isChecked() ? IM_SENDTOBOTTOM : IM_SENDTOALL);
}

void EditAction::updateArguments()
{
  if (editActionBaseWidget->theUseProfile->isChecked()) {
    editActionBaseWidget->theArguments->clear();
    const ProfileAction *a = ProfileServer::profileServer()->getAction(applicationMap[editActionBaseWidget->theApplications->currentText()], functionMap[editActionBaseWidget->theFunctions->currentText()]);
    if (!a) {
      arguments.clear(); return;
    }
    const QList<ProfileActionArgument> &actionArguments = a->arguments();
    if (actionArguments.count() != arguments.count()) {
      arguments.clear();
      for (int i = 0; i < actionArguments.count(); i++) {
        arguments.append(QVariant(""));
      }
    }
    editActionBaseWidget->theArguments->setEnabled(actionArguments.count());
    for (int i = 0; i < actionArguments.count(); i++) {
      editActionBaseWidget->theArguments->addItem(actionArguments[i].comment() + " (" + actionArguments[i].type() + ')');
      arguments[i].convert(QVariant::nameToType(actionArguments[i].type().toLocal8Bit()));
    }
    actionArguments.count() ? updateArgument(0) : updateArgument(-1);

  } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
    editActionBaseWidget->theArguments->clear();
    Prototype p(editActionBaseWidget->theDBusFunctions->currentText());
    if (p.count() != arguments.count()) {
      arguments.clear();
      for (int i = 0; i < p.count(); i++)
        arguments.append(QVariant(""));
    }
    editActionBaseWidget->theArguments->setEnabled(p.count());
    for (int i = 0; i < p.count(); i++) {
      editActionBaseWidget->theArguments->addItem(QString().setNum(i + 1) + ": " + (p.name(i).isEmpty() ? p.type(i) : p.name(i) + " (" + p.type(i) + ')'));
      arguments[i].convert(QVariant::nameToType(p.type(i).toLocal8Bit()));
    }
    p.count() ?  updateArgument(0) : updateArgument(-1);
  }
}

void EditAction::updateOptions()
{
  if (editActionBaseWidget->theUseProfile->isChecked()) {
    ProfileServer *theServer = ProfileServer::profileServer();
    if (editActionBaseWidget->theApplications->currentIndex() == -1) return;
    const Profile *p = theServer->profiles()[applicationMap[editActionBaseWidget->theApplications->currentText()]];
    isUnique = p->unique();
  } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
    program =  editActionBaseWidget->theDBusApplications->currentText();
    if ( program.isNull() ||  program.isEmpty()) {
      return;
    }
    isUnique = uniqueProgramMap[program];
  } else
    isUnique = true;

  editActionBaseWidget->theIMLabel->setEnabled(!isUnique);
  // theIMGroup->setEnabled(!isUnique);
  editActionBaseWidget->theDontSend->setEnabled(!isUnique);
  editActionBaseWidget->theSendToTop->setEnabled(!isUnique);
  editActionBaseWidget->theSendToBottom->setEnabled(!isUnique);
  editActionBaseWidget->theSendToAll->setEnabled(!isUnique);
}

// called when the textbox/checkbox/whatever changes value
void EditAction::slotParameterChanged()
{

  int index =  editActionBaseWidget->theArguments->currentIndex();
  kDebug() << "in: " << arguments[index].toString() ;
  int type = arguments[editActionBaseWidget->theArguments->currentIndex()].type();
  kDebug() << type ;
  switch (type) {
  case QVariant::Int: case QVariant::UInt:
    arguments[index] = editActionBaseWidget->theValueIntNumInput->value();
    break;
  case QVariant::Double:
    arguments[index] = editActionBaseWidget->theValueDoubleNumInput->value();
    break;
  case QVariant::Bool:
    arguments[index] = editActionBaseWidget->theValueCheckBox->isChecked();
    break;
  case QVariant::StringList:
    arguments[index] = editActionBaseWidget->theValueEditListBox->items();
    break;
  default:
    arguments[index] = editActionBaseWidget->theValueLineEdit->text();
  }
  arguments[index].convert(QVariant::Type(type));
  kDebug() << "out: " << arguments[index].toString() ;

}

void EditAction::updateArgument(int index)
{
  kDebug() << " i: " << index ;
  if (index >= 0 && ! arguments.isEmpty()) {
    switch (arguments[index].type()) {
    case QVariant::Int: case QVariant::UInt:
      editActionBaseWidget->theValue->setCurrentIndex(4);
      editActionBaseWidget->theValueIntNumInput->setValue(arguments[index].toInt());
      break;
    case QVariant::Double:
      editActionBaseWidget->theValue->setCurrentIndex(1);
      editActionBaseWidget->theValueDoubleNumInput->setValue(arguments[index].toDouble());
      break;
    case QVariant::Bool:
      editActionBaseWidget->theValue->setCurrentIndex(3);
      editActionBaseWidget->theValueCheckBox->setChecked(arguments[index].toBool());
      break;

    case QVariant::StringList:{
      editActionBaseWidget->theValue->setCurrentIndex(0);
      QStringList backup = arguments[index].toStringList();
      // backup needed because calling clear will kill what ever has been saved.
      editActionBaseWidget->theValueEditListBox->clear();
      editActionBaseWidget->theValueEditListBox->insertStringList(backup);
      arguments[index] = backup;
      break;
    }
    default:
      editActionBaseWidget->theValue->setCurrentIndex(2);
      editActionBaseWidget->theValueLineEdit->setText(arguments[index].toString());
    }
    editActionBaseWidget->theValue->setEnabled(true);
  } else {
    editActionBaseWidget->theValueLineEdit->setText("");
    editActionBaseWidget->theValueCheckBox->setChecked(false);
    editActionBaseWidget->theValueIntNumInput->setValue(0);
    editActionBaseWidget->theValueDoubleNumInput->setValue(0.0);
    editActionBaseWidget->theValue->setEnabled(false);
  }
}

void EditAction::initApplications()
{
  ProfileServer *theServer = ProfileServer::profileServer();
  editActionBaseWidget->theApplications->clear();
  applicationMap.clear();

  QHash<QString, Profile*> dict = theServer->profiles();
  QHash<QString, Profile*>::const_iterator i;
  for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
    editActionBaseWidget->theApplications->addItem(i.value()->name());
    applicationMap[i.value()->name()] = i.key();
    kDebug() << "read Application: " << i.value()->name() << i.key();
  }
  updateFunctions();
}

void EditAction::updateFunctions()
{
  ProfileServer *theServer = ProfileServer::profileServer();
  editActionBaseWidget->theFunctions->clear();
  functionMap.clear();
  QString application = editActionBaseWidget->theApplications->currentText();
  if (application.isNull() || application.isEmpty()){
    return;
  }

  const Profile *p = theServer->profiles()[applicationMap[application]];

  QHash<QString, ProfileAction*> dict = p->actions();
  QHash<QString, ProfileAction*>::const_iterator i;
  for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
    editActionBaseWidget->theFunctions->addItem(i.value()->name());
    functionMap[i.value()->name()] = i.key();
  }
  updateArguments();
}

void EditAction::initDBusApplications()
{
  QStringList names;

  editActionBaseWidget->theDBusApplications->clear();

  QDBusConnectionInterface *dBusIface = QDBusConnection::sessionBus().interface();
  QStringList allServices = dBusIface->registeredServiceNames();
  allServices.sort();

  for (QStringList::const_iterator i = allServices.constBegin(); i != allServices.constEnd(); ++i) {
    // Use only KDE-Apps
    if (!(*i).contains("org.kde")) {
      continue;
    }

    // Remove the "org.kde."
    QString name = (*i);
    name.remove(0, 8);

    // Remove "human unreadable" entries
    QRegExp r("[a-zA-Z]*");
    if (! r.exactMatch(name)) {
      continue;
    }

    //remove duplicates
    if (names.contains(name)) {
      continue;
    }

    editActionBaseWidget->theDBusApplications->addItem(name);
    nameProgramMap[name] = *i;
  }


  updateDBusObjects();
}

void EditAction::updateDBusObjects()
{
  editActionBaseWidget->theDBusObjects->clear();
  QString dbusApp = editActionBaseWidget->theDBusApplications->currentText();
  kDebug() << "ProgramMap: " << nameProgramMap[dbusApp];

  QDBusInterface *dBusIface = new QDBusInterface(nameProgramMap[dbusApp], "/", "org.freedesktop.DBus.Introspectable");
  QDBusReply<QString> response = dBusIface->call("Introspect");

  QDomDocument domDoc;
  domDoc.setContent(response);

  QDomElement node = domDoc.documentElement();

  QDomElement child = node.firstChildElement();
  QStringList  tObjectsList;
  while (!child.isNull()) {
    kDebug() << child.tagName() << ":" << child.attribute(QLatin1String("name"));
    if (child.tagName() == QLatin1String("node")) {
      tObjectsList << child.attribute(QLatin1String("name"));
    }
    child = child.nextSiblingElement();
  }
  tObjectsList.sort();
  editActionBaseWidget->theDBusObjects->insertItems(0, tObjectsList);
  updateDBusFunctions();
}

void EditAction::updateDBusFunctions()
{
  editActionBaseWidget->theDBusFunctions->clear();

  QDBusInterface *dBusIface = new QDBusInterface(nameProgramMap[editActionBaseWidget->theDBusApplications->currentText()], '/' + editActionBaseWidget->theDBusObjects->currentText(), "org.freedesktop.DBus.Introspectable");
  QDBusReply<QString> response = dBusIface->call("Introspect");

  QDomDocument domDoc;
  domDoc.setContent(response);

  QDomElement node = domDoc.documentElement();
  QDomElement child = node.firstChildElement();

  QString function;
  QStringList functionsList;
  while (!child.isNull()) {
    if (child.tagName() == QLatin1String("interface")) {
      if (child.attribute("name") == "org.freedesktop.DBus.Properties" ||
          child.attribute("name") == "org.freedesktop.DBus.Introspectable") {
            child = child.nextSiblingElement();
            continue;
      }
      QDomElement subChild = child.firstChildElement();
      while (!subChild.isNull()) {
        if (subChild.tagName() == QLatin1String("method")) {
          QString method = subChild.attribute(QLatin1String("name"));
          kDebug() << "Method: " << method;
          function = "QString " + method + '(';
          QDomElement arg = subChild.firstChildElement();
          QString argStr;
          while (!arg.isNull()) {
            if (arg.tagName() == QLatin1String("arg")) {
              if (arg.attribute(QLatin1String("direction")) == "in") {
                if (!argStr.isEmpty()) {
                  argStr += ", ";
                }
                if (arg.attribute(QLatin1String("type")) == "i") {
                  argStr += "int";
                } else if (arg.attribute(QLatin1String("type")) == "u") {
                  argStr += "uint";
                } else if (arg.attribute(QLatin1String("type")) == "s") {
                  argStr += "QString";
                } else if (arg.attribute(QLatin1String("type")) == "b") {
                  argStr += "bool";
                } else if (arg.attribute(QLatin1String("type")) == "d") {
                  argStr += "double";
                } else if (arg.attribute(QLatin1String("type")) == "as") {
                  argStr += "QStringList";
                } else if (arg.attribute(QLatin1String("type")) == "ay") {
                  argStr += "QByteArray";
                } else if (arg.attribute(QLatin1String("type")) == "(iii)") {
                  kDebug() << "got a (iii) type";
                  QString helper = arg.attribute("name");
                  arg = arg.nextSiblingElement();
                  argStr += arg.attribute(QLatin1String("value"));
                  argStr += ' ' + helper;
                  arg = arg.nextSiblingElement();
                  continue;
                } else {
                  argStr += arg.attribute(QLatin1String("type"));
                }
                argStr += ' ' + arg.attribute(QLatin1String("name"));
                kDebug() << "Arg: " << argStr;
              }
            }
            arg = arg.nextSiblingElement();
          }
          function +=  argStr + ')';
          functionsList << function;
        }
        subChild = subChild.nextSiblingElement();
      }
    }
    functionsList.sort();
    editActionBaseWidget->theDBusFunctions->addItems(functionsList);

    child = child.nextSiblingElement();
  }

  updateArguments();
}

void EditAction::addItem(QString item)
{
  editActionBaseWidget->theModes->addItem(item);
}

#include "editaction.moc"
