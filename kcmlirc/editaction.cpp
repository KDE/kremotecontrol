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
#include "addaction.h"
#include "dbusinterface.h"

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>
#include <solid/control/remotecontrol.h>
#include <QStandardItemModel>
#include <QHeaderView>

EditAction::EditAction(IRAction *action, const QStringList &modeList, QWidget *parent, const bool &modal): KDialog(parent)
{
    theAction = action;
    editActionBaseWidget = new EditActionBaseWidget();
    setMainWidget(editActionBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    dbusServiceModel = new QStandardItemModel(editActionBaseWidget->theDBusApplications);
    editActionBaseWidget->theDBusApplications->setModel(dbusServiceModel);
    mainGroup.addButton(editActionBaseWidget->theUseDBus);
    mainGroup.addButton(editActionBaseWidget->theUseProfile);
    mainGroup.addButton(editActionBaseWidget->theChangeMode);


    editActionBaseWidget->theModes->addItem(i18n("[Exit current mode]"));
    foreach(const QString &mode, modeList) {
        editActionBaseWidget->theModes->addItem(mode);
    }
    buttonModel = new RemoteButtonModel(Solid::Control::RemoteControl(theAction->remote()).buttons(), editActionBaseWidget->theButtons);
    editActionBaseWidget->theButtons->setModel(buttonModel);
    
    editActionBaseWidget->theButtons->setCurrentIndex(buttonModel->indexOfButtonName(theAction->button()));

    updateApplications();
    connectSignalsAndSlots();

    argumentsModel = new QStandardItemModel(editActionBaseWidget->argumentsView);    
    editActionBaseWidget->argumentsView->setModel(argumentsModel);
    editActionBaseWidget->argumentsView->setItemDelegate(new ArgumentDelegate());
    argumentsModel->setHeaderData(0, Qt::Horizontal, i18n("Argument name"));
    argumentsModel->setHeaderData(1, Qt::Horizontal, i18nc("The value of an argument", "Value"));

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
    connect(editActionBaseWidget->theDBusFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateInstancesOptions()));

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
        kDebug() << "is Mode change:" << theAction->object() << "at index" << editActionBaseWidget->theModes->findText(theAction->object());
        for (int i = 0; i < editActionBaseWidget->theModes->count(); ++i) {
            kDebug() << "combobox contents:" << editActionBaseWidget->theModes->itemText(i);
        }
        if (theAction->object().isEmpty())
            editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(i18n("[Exit current mode]")));
        else
            editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(theAction->object()));
    } else if (theAction->isJustStart()) { // profile action
        editActionBaseWidget->theUseProfile->setChecked(true);
        const Profile *p = ProfileServer::getInstance()->getProfileById(theAction->program());
        editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(p->name()));
        editActionBaseWidget->theJustStart->setChecked(true);
    } else if (ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype())) { // profile action
        editActionBaseWidget->theUseProfile->setChecked(true);
        editActionBaseWidget->thePerformFunction->setChecked(true);
    } else { // DBus action
        editActionBaseWidget->theUseDBus->setChecked(true);
    }
}

IRAction* EditAction::getAction()
{
    RemoteControlButton *tButton = buttonModel->getButton( editActionBaseWidget->theButtons->currentIndex());
    IRAction* tAction = new IRAction(tButton->remoteName(), tButton->name());
    tAction->setMode(theAction->mode());
    if (editActionBaseWidget->theChangeMode->isChecked()) {
        tAction->setProgram("");
        if (editActionBaseWidget->theModes->currentText() == i18n("[Exit current mode]"))
            tAction->setObject("");
        else
            tAction->setObject(editActionBaseWidget->theModes->currentText());
        tAction->setDoBefore(editActionBaseWidget->theDoBefore->isChecked());
        tAction->setDoAfter(editActionBaseWidget->theDoAfter->isChecked());
    } else if (editActionBaseWidget->theUseProfile->isChecked()) {
        QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();       
        ProfileAction *profileAction = profileModel->getProfileAction(editActionBaseWidget->theFunctions->currentIndex());
        if ( profileAction  || (editActionBaseWidget->theJustStart->isChecked() &&  ProfileServer::getInstance()->getProfileById(application))) {
            tAction->setProgram(ProfileServer::getInstance()->getProfileById(application)->id());
            if (editActionBaseWidget->theJustStart->isChecked()) {
                tAction->setObject("");
            } else {
                tAction->setObject(profileAction->objId());
                tAction->setMethod(profileAction->prototype());
                tAction->setArguments(getCurrentArgs());
            }
        }
    } else {
        tAction->setProgram(getCurrentDbusApp());
        tAction->setObject(editActionBaseWidget->theDBusObjects->currentText());
        tAction->setMethod(getCurrentDBusFunction());
        tAction->setArguments(getCurrentArgs());
    }
    tAction->setRepeat(editActionBaseWidget->theRepeat->isChecked());
    tAction->setAutoStart(editActionBaseWidget->theAutoStart->isChecked());
    tAction->setUnique(isUnique);

    if (editActionBaseWidget->theDontSend->isChecked()) {
        tAction->setIfMulti(IM_DONTSEND);
    } else if ( editActionBaseWidget->theSendToTop->isChecked()) {
        tAction->setIfMulti(IM_SENDTOTOP);
    } else if ( editActionBaseWidget->theSendToBottom->isChecked()) {
        tAction->setIfMulti(IM_SENDTOBOTTOM);
    } else {
        tAction->setIfMulti(IM_SENDTOALL);
    }
    return tAction;
}

void EditAction::updateArguments()
{
    editActionBaseWidget->argumentsView->setEnabled(true);
    argumentsModel->clear();
    QStringList headerLabels;
    headerLabels << i18n("Argument name") << i18nc("Value of the argument", "Value");
    argumentsModel->setHorizontalHeaderLabels(headerLabels);

    if (editActionBaseWidget->theUseProfile->isChecked()) {
        QString function = editActionBaseWidget->theFunctions->itemData(editActionBaseWidget->theFunctions->currentIndex()).toString();
        QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();
        const ProfileAction *profileAction = ProfileServer::getInstance()->getAction(application, function);

        // No profile action configured or theJustStart is checked... No need for arguments
        if (!profileAction ||  editActionBaseWidget->theJustStart->isChecked()) {
            editActionBaseWidget->argumentsView->setEnabled(false);
            return;
        }

        // Is the configured Action the selected one?
        if (profileAction && (profileAction->profile()->name() == theAction->application()) &&
                profileAction->name() == theAction->function())
        {
            for (int i = 0; i < profileAction->arguments().count(); ++i) {
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(profileAction->arguments().at(i).comment() + " (" + profileAction->arguments().at(i).type() + ')'));
                tmp.append(new ArgumentsModelItem(theAction->arguments().at(i)));
                argumentsModel->appendRow(tmp);
            }

        } else {
            const QList<ProfileActionArgument> &profileActionArguments = profileAction->arguments();
            for (int i = 0; i < profileActionArguments.count(); ++i) {
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).comment() + " (" + profileActionArguments.at(i).type() + ')'));
                tmp.append(new ArgumentsModelItem(profileActionArguments.at(i).getDefault()));
                argumentsModel->appendRow(tmp);
            }
        }
    } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
        Prototype p = editActionBaseWidget->theDBusFunctions->itemData(editActionBaseWidget->theDBusFunctions->currentIndex(), Qt::UserRole).value<Prototype>();


        // Check if the current selected function is the configured one
        if (!ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
                getCurrentDbusApp() == theAction->program() && // The correct app is selected!
                editActionBaseWidget->theDBusObjects->currentText() == theAction->object() && // And the Object is selected too!
                getCurrentDBusFunction() == theAction->method().prototype()) { // And also the Function. Fill in the arguments
            for (int i = 0; i < theAction->arguments().size(); ++i) {
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(p.getArguments().at(i).second + " (" + QVariant::typeToName(p.getArguments().at(i).first) + ')'));
                tmp.append(new ArgumentsModelItem(theAction->arguments().at(i)));
                argumentsModel->appendRow(tmp);
            }
        } else {
            for (int i = 0; i < p.getArguments().size(); ++i) {
                QList<QStandardItem*> tmp;
                tmp.append(new ArgumentsModelItem(p.getArguments().at(i).second + " (" + QVariant::typeToName(p.getArguments().at(i).first) + ')'));
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
        ProfileServer *theServer = ProfileServer::getInstance();
        if (editActionBaseWidget->theApplications->currentIndex() == -1) return;
        const Profile *p = theServer->getProfileById(editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString());
        isUnique = p->unique();
    } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
        program =  getCurrentDbusApp();
        if ( program.isNull() ||  program.isEmpty()) {
            return;
        }
        isUnique = DBusInterface::getInstance()->isUnique(program);
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
    foreach(Profile *tmp, ProfileServer::getInstance()->profiles()) {
        editActionBaseWidget->theApplications->addItem(tmp->name(), tmp->id());
    }
    editActionBaseWidget->theApplications->model()->sort(0);

    const ProfileAction *profileAction = ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());
    if (profileAction) {
        int index = editActionBaseWidget->theApplications->findText(profileAction->profile()->name());
        editActionBaseWidget->theApplications->setCurrentIndex(index < 0 ? 0: index);
    } else {
        editActionBaseWidget->theApplications->setCurrentIndex(0);
    }
}

void EditAction::updateFunctions()
{
    editActionBaseWidget->theFunctions->clear();
    if (editActionBaseWidget->theJustStart->isChecked()) {
        return;
    }

     const QString application = editActionBaseWidget->theApplications->itemData(editActionBaseWidget->theApplications->currentIndex()).toString();
     Profile *tProfile =const_cast<Profile*> (ProfileServer::getInstance()->getProfileById(application));
     profileModel = new ProfileModel(tProfile, editActionBaseWidget->theFunctions);
     editActionBaseWidget->theFunctions->setModel(new ProfileModel(tProfile, editActionBaseWidget->theFunctions));
     const ProfileAction *action = ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());   
    if (action && (action->profile()->name() == editActionBaseWidget->theApplications->currentText())) {
        int index = editActionBaseWidget->theFunctions->findText(action->name());
        editActionBaseWidget->theFunctions->setCurrentIndex(index < 0 ? 0 : index);
    }

    updateArguments();
}

void EditAction::updateDBusApplications()
{
    kDebug()<< "update dbus applications";
    editActionBaseWidget->theDBusApplications->clear();

    foreach(const QString &item, DBusInterface::getInstance()->getRegisteredPrograms()) {
        dbusServiceModel->appendRow(new DBusServiceItem(item));
    }

    if (!ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
            !DBusInterface::getInstance()->isProgramRunning(theAction->program()) && 
	    !theAction->isModeChange()) {
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
    if (!ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
            !DBusInterface::getInstance()->isProgramRunning(theAction->program())) {
        if (editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex()).toString() ==
                theAction->program()) { // Is selected!
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
    
    const QList<Prototype> tList = DBusInterface::getInstance()->getFunctions(getCurrentDbusApp(), editActionBaseWidget->theDBusObjects->currentText());
    foreach(const Prototype &tType, tList) {
        editActionBaseWidget->theDBusFunctions->addItem(tType.prototype(), qVariantFromValue(tType));
    }

    // Check if configured Action is a DBus Action but not running
    if (!ProfileServer::getInstance()->getAction(theAction->program(), theAction->object(), theAction->method().prototype()) &&
            !DBusInterface::getInstance()->isProgramRunning(theAction->program())) {
        if (editActionBaseWidget->theDBusApplications->itemData(editActionBaseWidget->theDBusApplications->currentIndex()).toString() ==
                theAction->program()) { // The correct app is selected!
            kDebug() << "theAction->object" << theAction->object();
            kDebug() << "current selected" << editActionBaseWidget->theDBusObjects->currentText();
            if (editActionBaseWidget->theDBusObjects->currentText() ==
                    theAction->object()) { // And the Object is selected too!
		editActionBaseWidget->theDBusFunctions->addItem(theAction->function(), qVariantFromValue(Prototype(theAction->function())));
            }
        }
    }


    kDebug() << "searching for" << theAction->method().prototype();
    int ti =editActionBaseWidget->theDBusFunctions->findData(theAction->method().prototype(), Qt::DisplayRole);
    editActionBaseWidget->theDBusFunctions->setCurrentIndex(ti == -1 ? 0 : ti);

    updateArguments();
}

void EditAction::addItem(QString item)
{
    editActionBaseWidget->theModes->addItem(item);
}

Arguments EditAction::getCurrentArgs() {
    Arguments retList;
    foreach(QStandardItem *item, argumentsModel->takeColumn(1)) {
        retList.append(item->data(Qt::EditRole));
    }
    return retList;
}

#include "editaction.moc"
