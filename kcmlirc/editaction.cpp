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
  * @author Gav Wood, Michael Zanetti & Frank Scheffold
  */

#include "editaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "addaction.h"
//#include "model.h"
#include "dbusinterface.h"

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>
#include <QStandardItemModel>

EditAction::EditAction(IRAction *action, QWidget *parent, const bool &modal): KDialog(parent)
{
    theAction = action;
    editActionBaseWidget = new EditActionBaseWidget();
    setMainWidget(editActionBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    //editActionBaseWidget->theDBusApplications->setModel(new DBusServiceModel(0));
        dbusServiceModel = new QStandardItemModel(editActionBaseWidget->theDBusApplications);
    editActionBaseWidget->theDBusApplications->setModel(dbusServiceModel);
    editActionBaseWidget->theDBusFunctions->setModel(new DBusFunctionModel(0));
    editActionBaseWidget->theDBusFunctions->setModelColumn(2);
    mainGroup.addButton(editActionBaseWidget->theUseDBus);
    mainGroup.addButton(editActionBaseWidget->theUseProfile);
    mainGroup.addButton(editActionBaseWidget->theChangeMode);




    updateApplications();
    connectSignalsAndSlots();
    //initDBusApplications();

    argumentsModel = new QStandardItemModel(editActionBaseWidget->argumentsView);
    editActionBaseWidget->argumentsView->setModel(argumentsModel);
    editActionBaseWidget->argumentsView->setItemDelegate(new ArgumentDelegate());
    argumentsModel->setHeaderData(0, Qt::Horizontal, i18n("Name"));
    argumentsModel->setHeaderData(1, Qt::Horizontal, i18n("Value"));

    readFrom();

}

EditAction::~EditAction()
{
}

void EditAction::connectSignalsAndSlots() {


    //Profile Action

    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theUseProfileAppLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theApplications,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theJustStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->thePerformFunction,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->thePerformFunction,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));

    //When only just start ist checked
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setDisabled(bool)));

    connect(editActionBaseWidget->theApplications,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateFunctions()));


    connect(editActionBaseWidget->theFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateArguments()));
    connect(editActionBaseWidget->theFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateInstancesOptions()));

    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));

    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),this,SLOT(updateFunctions()));

    //DBus Action Enabling the widgets
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusApplicationsLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusObjectsLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusFunctionsLabel,SLOT(setEnabled(bool)));

    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusApplications,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusObjects,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));

    //Fill in DBUS Application Comboboxes
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),this, SLOT(updateDBusApplications()));
    connect(editActionBaseWidget->theDBusApplications,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateDBusObjects()));
    connect(editActionBaseWidget->theDBusObjects,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateDBusFunctions()));
    connect(editActionBaseWidget->theDBusFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateArguments()));

    //Mode Action
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->argumentsView ,SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoAfter,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoBefore,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theModes,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),this,SLOT(updateInstancesOptions()));
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
        const Profile *p = ProfileServer::profileServer()->getProfileById(theAction->program());
        editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(p->name()));
        editActionBaseWidget->theJustStart->setChecked(true);
    } else if (ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype())) { // profile action
        editActionBaseWidget->theUseProfile->setChecked(true);
  editActionBaseWidget->thePerformFunction->setChecked(true);
    } else { // DBus action
        editActionBaseWidget->theUseDBus->setChecked(true);
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
        QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();
        QString function = editActionBaseWidget->theFunctions->itemData(editActionBaseWidget->theFunctions->currentIndex()).toString();
        const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(application, function);
        if ( profileAction  || (editActionBaseWidget->theJustStart->isChecked() &&  ProfileServer::profileServer()->getProfileById(application))) {
            theAction->setProgram(ProfileServer::profileServer()->getProfileById(application)->id());
            if (editActionBaseWidget->theJustStart->isChecked()) {
                theAction->setObject("");
            } else {
                kDebug() << "wrote back: " << application;
                theAction->setObject(profileAction->objId());
                theAction->setMethod(profileAction->prototype());
                theAction->setArguments(getCurrentArgs());
            }
        }
    } else {
        theAction->setProgram(getCurrentDbusApp());
        theAction->setObject(editActionBaseWidget->theDBusObjects->currentText());
        theAction->setMethod(getCurrentDBusFunction());
        theAction->setArguments(getCurrentArgs());
    }
    theAction->setRepeat(editActionBaseWidget->theRepeat->isChecked());
    theAction->setAutoStart(editActionBaseWidget->theAutoStart->isChecked());
    theAction->setUnique(isUnique);

    if (editActionBaseWidget->theDontSend->isChecked()) {
        theAction->setIfMulti(IM_DONTSEND);
    } else if ( editActionBaseWidget->theSendToTop->isChecked()) {
        theAction->setIfMulti(IM_SENDTOTOP);
    } else if ( editActionBaseWidget->theSendToBottom->isChecked()) {
        theAction->setIfMulti(IM_SENDTOBOTTOM);
    } else {
        theAction->setIfMulti(IM_SENDTOALL);
    }
}

void EditAction::updateArguments()
{
    editActionBaseWidget->argumentsView->setEnabled(true);
    argumentsModel->clear();
    QStringList headerLabels;
    headerLabels << i18n("Name") << i18n("Value");
    argumentsModel->setHorizontalHeaderLabels(headerLabels);

    if (editActionBaseWidget->theUseProfile->isChecked()) {
  QString function = editActionBaseWidget->theFunctions->itemData(editActionBaseWidget->theFunctions->currentIndex()).toString();
  QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();
        const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(application, function);

        // No profile action configured or theJustStart is checked... No need for arguments
        if (!profileAction ||  editActionBaseWidget->theJustStart->isChecked()) {
            editActionBaseWidget->argumentsView->setEnabled(false);
            return;
        }

        // Is the configured Action the selected one?
        if(profileAction && (profileAction->profile()->name() == theAction->application()) &&
           profileAction->name() == theAction->function())
        {
            kDebug() << "appending configured args";
            for(int i = 0; i < profileAction->arguments().count(); ++i){
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(profileAction->arguments().at(i).comment() + " (" + profileAction->arguments().at(i).type() + ")"));
                tmp.append(new ArgumentsModelItem(theAction->arguments().at(i)));
                argumentsModel->appendRow(tmp);
            }

        } else {
            kDebug() << "Appending default args";
            const QList<ProfileActionArgument> &profileActionArguments = profileAction->arguments();
            for (int i = 0; i < profileActionArguments.count(); ++i) {
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).comment() + " (" + profileActionArguments.at(i).type() + ")"));
                tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).getDefault()));
                argumentsModel->appendRow(tmp);
            }
        }
    } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
  Prototype p = editActionBaseWidget->theDBusFunctions->itemData(editActionBaseWidget->theDBusFunctions->currentIndex(), Qt::UserRole).value<Prototype>().prototype();


        // Check if the current selected function is the configured one
        if(!ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
                getCurrentDbusApp() == theAction->program() && // The correct app is selected!
                editActionBaseWidget->theDBusObjects->currentText() == theAction->object() && // And the Object is selected too!
                getCurrentDBusFunction() == theAction->method().prototype()){ // And also the Function. Fill in the arguments
            for(int i = 0; i < theAction->arguments().size(); ++i){
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(p.getArguments().at(i).second + " (" + QVariant::typeToName(p.getArguments().at(i).first) + ")"));
                tmp.append(new ArgumentsModelItem(theAction->arguments().at(i)));
                argumentsModel->appendRow(tmp);
            }
        } else {
            for(int i = 0; i < p.getArguments().size(); ++i){
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(p.getArguments().at(i).second + " (" + QVariant::typeToName(p.getArguments().at(i).first) + ")"));
                tmp.append(new ArgumentsModelItem(QVariant(p.getArguments().at(i).first)));
                argumentsModel->appendRow(tmp);
            }
        }
    }
    editActionBaseWidget->argumentsView->resizeColumnsToContents();
    editActionBaseWidget->argumentsView->resizeRowsToContents();
    editActionBaseWidget->argumentsView->horizontalHeader()->setStretchLastSection(true);
}

void EditAction::updateInstancesOptions()
{
    if (editActionBaseWidget->theUseProfile->isChecked()) {
        ProfileServer *theServer = ProfileServer::profileServer();
        if (editActionBaseWidget->theApplications->currentIndex() == -1) return;
        const Profile *p = theServer->getProfileById(editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString());
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

void EditAction::updateApplications()
{
    editActionBaseWidget->theApplications->clear();
    foreach(Profile *tmp, ProfileServer::profileServer()->profiles()){
  editActionBaseWidget->theApplications->addItem(tmp->name(), tmp->id());
    }
    editActionBaseWidget->theApplications->model()->sort(0);

    const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());
    if(profileAction){
      int index = editActionBaseWidget->theApplications->findText(profileAction->profile()->name());
      editActionBaseWidget->theApplications->setCurrentIndex(index < 0 ? 0: index);
    } else {
      editActionBaseWidget->theApplications->setCurrentIndex(0);
    }
}

void EditAction::updateFunctions()
{
    editActionBaseWidget->theFunctions->clear();
    if(editActionBaseWidget->theJustStart->isChecked()){
      return;
    }

    QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();
    kDebug() << "app:" << application;
    QHash<QString, ProfileAction*> dict = ProfileServer::profileServer()->getProfileById(application)->actions();
    QHash<QString, ProfileAction*>::const_iterator i;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
      editActionBaseWidget->theFunctions->addItem(i.value()->name(), i.key());
    }
    const ProfileAction *action = ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());
    if(action && (action->profile()->name() == editActionBaseWidget->theApplications->currentText())){
      int index = editActionBaseWidget->theFunctions->findText(action->name());
      editActionBaseWidget->theFunctions->setCurrentIndex(index < 0 ? 0 : index);
    }
}

void EditAction::updateDBusApplications()
{
kDebug()<< "update dbus applications";
    editActionBaseWidget->theDBusApplications->clear();
    //editActionBaseWidget->theDBusApplications->addItems(DBusInterface::getInstance()->getRegisteredPrograms());

    foreach(QString item, DBusInterface::getInstance()->getRegisteredPrograms()){
        dbusServiceModel->appendRow(new DBusServiceItem(item));
    }

    if(!ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
    !DBusInterface::getInstance()->isProgramRunning(theAction->program())){      
      dbusServiceModel->appendRow(new DBusServiceItem(theAction->program()));      
    }
    editActionBaseWidget->theDBusApplications->model()->sort( Qt::AscendingOrder);
    int ti = editActionBaseWidget->theDBusApplications->findData(theAction->program());
    editActionBaseWidget->theDBusApplications->setCurrentIndex(ti == -1 ? 0 : ti);

}

void EditAction::updateDBusObjects()
{
    editActionBaseWidget->theDBusObjects->clear();
    editActionBaseWidget->theDBusObjects->insertItems(0, DBusInterface::getInstance()->getObjects(getCurrentDbusApp()));
    kDebug() << "Currnet app " << getCurrentDbusApp();
    // Check if configured Action is a DBus Action but not running
    if(!ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
    !DBusInterface::getInstance()->isProgramRunning(theAction->program())){
  if(editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex()).toString() ==
        theAction->program()){ // Is selected!
    editActionBaseWidget->theDBusObjects->addItem(theAction->object());
  }
    }

    editActionBaseWidget->theDBusObjects->model()->sort( Qt::AscendingOrder);
    int ti = editActionBaseWidget->theDBusObjects->findText(theAction->object());
    editActionBaseWidget->theDBusObjects->setCurrentIndex(ti == -1 ? 0 : ti);
}

void EditAction::updateDBusFunctions()
{
    editActionBaseWidget->theDBusFunctions->clear();

    QList<Prototype> tList = DBusInterface::getInstance()->getFunctions(getCurrentDbusApp(), editActionBaseWidget->theDBusObjects->currentText());
    foreach(Prototype tType, tList) {
        editActionBaseWidget->theDBusFunctions->addItem(0, qVariantFromValue(tType));
    }

    // Check if configured Action is a DBus Action but not running
    if(!ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
    !DBusInterface::getInstance()->isProgramRunning(theAction->program())){
  if(editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex()).toString() ==
              theAction->program()){ // The correct app is selected!
    kDebug() << "theAction->object" << theAction->object();
    kDebug() << "current selected" << editActionBaseWidget->theDBusObjects->currentText();
    if(editActionBaseWidget->theDBusObjects->currentText() ==
    theAction->object()){ // And the Object is selected too!
      editActionBaseWidget->theDBusFunctions->addItem(0, qVariantFromValue(Prototype(theAction->function())));
    }
  }
    }


    editActionBaseWidget->theDBusFunctions->model()->sort( Qt::AscendingOrder);
    kDebug() << "searching for" << theAction->method().prototype();
    int ti =editActionBaseWidget->theDBusFunctions->findData(qVariantFromValue(theAction->method()), Qt::EditRole);
    editActionBaseWidget->theDBusFunctions->setCurrentIndex(ti == -1 ? 0 : ti);
}

void EditAction::addItem(QString item)
{
    editActionBaseWidget->theModes->addItem(item);
}

Arguments EditAction::getCurrentArgs() {
    Arguments retList;
    foreach(QStandardItem *item, argumentsModel->takeColumn(1)){
	retList.append(item->data(Qt::EditRole));
    }
    return retList;
}

#include "editaction.moc"
