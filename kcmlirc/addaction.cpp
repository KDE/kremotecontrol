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

#include "addaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "mode.h"
#include "arguments.h"
#include "iraction.h"
#include "model.h"
#include "dbusinterface.h"
#include <QRegExp>

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>
#include <klocale.h>

AddAction::AddAction(QWidget *parent, const char *name, const Mode &mode): theMode(mode)
{
    Q_UNUSED(name)
    Q_UNUSED(parent)
    setupUi(this);
    theFunctions->setModel(new DBusFunctionModel(theFunctions));
    theFunctions->setSelectionBehavior(QAbstractItemView::SelectRows);
    theFunctions->setSelectionMode(QAbstractItemView::SingleSelection);
    //theFunctions->setShowGrid(false);
    dbusAppsModel = new QStandardItemModel(theObjects);
    dbusAppsModel->setHorizontalHeaderLabels(QStringList() << i18n("DBus functions"));
    theObjects->setModel(dbusAppsModel);


    connect(this, SIGNAL(currentIdChanged(int)), SLOT(updateForPageChange()));
    connect(this, SIGNAL(currentIdChanged(int)), SLOT(slotCorrectPage()));

    connect(theValueDoubleNumInput, SIGNAL(valueChanged(double)), this, SLOT(slotParameterChanged()));
    connect(theValueIntNumInput, SIGNAL(valueChanged(int)), this, SLOT(slotParameterChanged()));
    connect(theValueLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotParameterChanged()));
    connect(theValueCheckBox, SIGNAL(toggled(bool)), this, SLOT(slotParameterChanged()));
    connect(theValueEditListBox, SIGNAL(changed()), this, SLOT(slotParameterChanged()));
    connect(theChangeMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theParameters, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameter()));
    connect(theSwitchMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theExitMode, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theUseDBus, SIGNAL(clicked()), this, SLOT(updateButtonStates()));    

    connect(theUseProfile, SIGNAL(toggled(bool)), theProfiles, SLOT(setEnabled(bool)));
    connect(theUseProfile, SIGNAL(clicked()), this, SLOT(updateButtonStates()));

    connect(theNotJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theProfileFunctions, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theAutoStart, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theRepeat, SLOT(setEnabled(bool)));
    connect(theNotJustStart, SIGNAL(toggled(bool)), theRepeat, SLOT(setChecked(bool)));

    connect(theJustStart, SIGNAL(toggled(bool)), theAutoStart, SLOT(setChecked(bool)));
    connect(theJustStart, SIGNAL(clicked()), this, SLOT(updateButtonStates()));


    connect(theObjects, SIGNAL(clicked(QModelIndex)), this, SLOT(updatePrototyes(QModelIndex)));
    connect(theObjects, SIGNAL(clicked(QModelIndex)), this, SLOT(updateButtonStates()));
    connect(theObjects, SIGNAL(clicked(QModelIndex)), theObjects, SLOT(expand(QModelIndex)));

    connect(theProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateProfileFunctions()));
    connect(theProfiles, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theModes, SIGNAL(itemSelectionChanged()), this, SLOT(slotModeSelected()));
    connect(theModes, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theButtons, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theButtons, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(next()));

    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtonStates()));
    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameter()));
    connect(theProfileFunctions, SIGNAL(itemSelectionChanged()), this, SLOT(updateInstancesOptions()));
    connect(theProfileFunctions, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(next()));


    connect(theFunctions, SIGNAL(clicked(QModelIndex)), this, SLOT(updateButtonStates()));
    connect(theFunctions, SIGNAL(clicked(QModelIndex)), this, SLOT(updateParameter()));
    connect(theFunctions, SIGNAL(clicked(QModelIndex)), this, SLOT(updateInstancesOptions()));
    connect(theFunctions,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(next()));

    connect(DBusInterface::getInstance(), SIGNAL(haveButton(const QString &, const QString &)), this, SLOT(updateButton(const QString &, const QString &)));


    curPage = 0;
    updateProfiles();
    updateButtons();
    updateObjects();
    updateProfileFunctions();

}

AddAction::~AddAction()
{
}


void AddAction::slotNextParam()
{
    // TODO: go on to next parameter
}

void AddAction::slotModeSelected()
{
    theSwitchMode->setChecked(true);
}

void AddAction::slotCorrectPage()
{
     int lastPage = curPage;
    curPage = this->currentId();

    kDebug() << "lastPage:" << lastPage << "; curPage:" << curPage;

    if (theUseProfile->isChecked() || theUseDBus->isChecked()) {
        QWizard::page(5)->setFinalPage(true);
    } else {
        QWizard::page(5)->setFinalPage(false);
    }


    if (curPage == 2 && theUseProfile->isChecked()) {
        if (lastPage > 1) {
            back();
        } else {
            next();
        }
    }
    if (curPage == 2  && theChangeMode->isChecked() && lastPage == 1) {
        next();
        next();
        next();
        next();
    }

    if (curPage == 5 && theChangeMode->isChecked() && lastPage == 6) {
        back();
        back();
        back();
        back();
    }

    if (curPage == 3 && theUseDBus->isChecked()) {
        if (lastPage == 4) {
            back();
        } else {
            next();
        }
    }

    if (curPage == 3) {
        updateProfileFunctions();
    }

    if (curPage == 4) {
        updateParameters();
        Prototype  tType = theFunctions->model()->data(theFunctions->currentIndex()).value<Prototype>();
        if ( (theUseDBus->isChecked() && theFunctions->currentIndex().row() >= 0 && tType.count() == 0) ||
                (theUseProfile->isChecked() && (theProfileFunctions->currentItem() && !theProfileFunctions->currentItem()->text(1).toInt())) || theJustStart->isChecked()
           ) {
//  showPage(((QWizard *)this)->page(lastPage == 5 ? (theUseDBus->isChecked() ? 2 : 3) : 5));

            if (lastPage == 5) {
                if (theUseDBus->isChecked()) {
                    back();
//    back();
                } else {
                    back();
                }
                // Restore the Wizard layout in case its modified
                QList<QWizard::WizardButton> layout;
                layout << QWizard::Stretch << QWizard::BackButton << QWizard::NextButton << QWizard::FinishButton << QWizard::CancelButton;
                setButtonLayout(layout);
            } else {
                next();
                QList<QWizard::WizardButton> layout;
                layout << QWizard::Stretch << QWizard::BackButton << QWizard::FinishButton << QWizard::CancelButton;
                setButtonLayout(layout);
            }
        }
    }
}

void AddAction::updateButton(const QString &remote, const QString &button)
{
    if (theMode.remote() == remote) { // note this isn't the "correct" way of doing it; really i should iterate throughg the items and try to find the item which when put through buttonMap[item] returns the current button name. but i cant be arsed.
        theButtons->setCurrentItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
        theButtons->scrollToItem(theButtons->findItems(RemoteServer::remoteServer()->getButtonName(remote, button), 0).first());
    } else
        KMessageBox::error(0, i18n("You did not select a mode of that remote control. Please use %1, "
                                   "or revert back to select a different mode.", theMode.remoteName()),
                           i18n("Incorrect Remote Control Detected"));

    updateButtonStates();

}

void AddAction::updateButtons()
{



    theButtons->clear();
    foreach(QString buttonName, DBusInterface::getInstance()->getButtons(theMode.remote())) {
        kDebug() << "foud buttonName " << buttonName;
        QListWidgetItem *tItem =  new  QListWidgetItem(RemoteServer::remoteServer()->getButtonName(theMode.remote(), buttonName),theButtons);
        tItem->setData(Qt::UserRole,buttonName);

    }
}

void AddAction::updateForPageChange()
{
    if (currentId() == 1){
	DBusInterface::getInstance()->requestNextKeyPress();
    } else {
	DBusInterface::getInstance()->cancelKeyPressRequest();
    }
    updateButtonStates();
}

void AddAction::updateButtonStates()
{
    kDebug() << "Updating button states";
    switch (currentId()) {
    case 0:
        button(QWizard::NextButton)->setEnabled(!theProfiles->selectedItems().isEmpty() || !theUseProfile->isChecked());
        break;
    case 1:
        button(QWizard::NextButton)->setEnabled(!theButtons->selectedItems().isEmpty());
        break;
    case 2:

        button(QWizard::NextButton)->setEnabled(theFunctions->currentIndex().isValid());
        break;
    case 3:
        button(QWizard::NextButton)->setEnabled(theProfileFunctions->currentItem() != 0 || theJustStart->isChecked());
        break;
    case 4:
        button(QWizard::NextButton)->setEnabled(true);
        break;
    case 5:
        button(QWizard::NextButton)->setEnabled(true);
        button(QWizard::FinishButton)->setEnabled(true);
        break;
    case 6:
        button(QWizard::NextButton)->setEnabled(false);
        button(QWizard::FinishButton)->setEnabled(theModes->currentItem() || !theSwitchMode->isChecked());
        break;
    }
}



void AddAction::updateProfiles()
{
    theProfiles->clear();
    profileMap.clear();

    foreach (Profile *tmp, ProfileServer::profileServer()->profiles())
    profileMap[new QListWidgetItem(tmp->name(), theProfiles)] = tmp->id();
}

void AddAction::updateOptions()
{
    IfMulti im;
    if (theUseProfile->isChecked()) {
        ProfileServer *theServer = ProfileServer::profileServer();
        if (!theProfiles->currentItem()) return;
        const Profile *p = theServer->getProfileById(profileMap[theProfiles->currentItem()]);
        im = p->ifMulti();
        isUnique = p->unique();
    } else if (theUseDBus->isChecked()) {
        //TODO: rewrite
        /*if (!theObjects->selectedItems().first()) return;
        QTreeWidgetItem* i = theObjects->selectedItems().first()->parent();
        */
//        if (!i) return;
        /*        isUnique = uniqueProgramMap[i];
                QRegExp r("(.*)-[0-9]+");
                program = r.exactMatch(nameProgramMap[i]) ? r.cap(1) : nameProgramMap[i];
                im = IM_DONTSEND;*/
    } else return;

    theIMLabel->setEnabled(!isUnique);
// theIMGroup->setEnabled(!isUnique);
    theIMLine->setEnabled(!isUnique);
    theIMTop->setEnabled(!isUnique);
    theDontSend->setEnabled(!isUnique);
    theSendToBottom->setEnabled(!isUnique);
    theSendToTop->setEnabled(!isUnique);
    theSendToAll->setEnabled(!isUnique);
    switch (im) {
    case IM_DONTSEND:
        theDontSend->setChecked(true);
        break;
    case IM_SENDTOTOP:
        theSendToTop->setChecked(true);
        break;
    case IM_SENDTOBOTTOM:
        theSendToBottom->setChecked(true);
        break;
    case IM_SENDTOALL:
        theSendToAll->setChecked(true);
        break;
    }
}

void AddAction::updateProfileFunctions()
{
    kDebug() << "updateProfileFunctions called";
    ProfileServer *theServer = ProfileServer::profileServer();
    theProfileFunctions->clear();
    profileFunctionMap.clear();
    if (theProfiles->selectedItems().isEmpty()) {
        return;
    }

    const Profile *p = theServer->getProfileById(profileMap[theProfiles->currentItem()]);
    QHash<QString, ProfileAction*> dict = p->actions();
    kDebug() << "actions: " << p->actions();
    QHash<QString, ProfileAction*>::const_iterator i;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
        kDebug() << "got function: " << i.value()->name();
        QStringList parameters;
        parameters << i.value()->name() << QString().setNum(i.value()->arguments().count()) << i.value()->comment();
        profileFunctionMap[new QTreeWidgetItem(theProfileFunctions, parameters)] = i.key();
    }
    updateParameters();
    updateOptions();
}

void AddAction::updateParameters()
{
    theParameters->clear();
    kDebug() << "clearing arguments";
    theArguments.clear();
    if (theUseDBus->isChecked() && theFunctions->currentIndex().row() >= 0) {
        Prototype p =  theFunctions->model()->data(theFunctions->currentIndex()).value<Prototype>();
        for (int k = 0; k < p.count(); k++) {
            QStringList parameters;
            parameters << (p.name(k).isEmpty() ? i18nc("Unknown parameter name in function", "&lt;anonymous&gt;") : p.name(k)) << "" << p.type(k) << QString().setNum(k + 1);
            new QTreeWidgetItem(theParameters, parameters);
            theArguments.append(QVariant(""));
            kDebug() << "converting argument to:" << p.type(k).toLocal8Bit();
            theArguments.back().convert(QVariant::nameToType(p.type(k).toLocal8Bit()));
        }
    } else if (theUseProfile->isChecked() && theProfiles->currentItem()) {
        ProfileServer *theServer = ProfileServer::profileServer();

        if (!theProfiles->currentItem()) return;
        if (!theProfileFunctions->currentItem()) return;
        const Profile *p = theServer->getProfileById(profileMap[theProfiles->currentItem()]);
        const ProfileAction *pa = p->actions()[profileFunctionMap[theProfileFunctions->currentItem()]];

        int index = 1;
        for (int i = 0; i < pa->arguments().size(); ++i, index++) {
            theArguments.append(QVariant((pa->arguments().at(i)).getDefault()));
            kDebug() << "converting argument to:" << QVariant::nameToType(pa->arguments().at(i).type().toLocal8Bit());
            theArguments.back().convert(QVariant::nameToType(pa->arguments().at(i).type().toLocal8Bit()));
            QStringList parameters;
            parameters << pa->arguments().at(i).comment() << theArguments.back().toString() << pa->arguments().at(i).type() << QString().setNum(index);
            new QTreeWidgetItem(theParameters, parameters);
        }

        // quicky update options too...
        theRepeat->setChecked(pa->repeat());
        theAutoStart->setChecked(pa->autoStart());
    }

    updateParameter();
}

void AddAction::updateParameter()
{
    kDebug() << "Update parameter called";
    if (!theParameters->selectedItems().isEmpty()) {
        QString type = theParameters->currentItem()->text(2);
        int index = theParameters->currentItem()->text(3).toInt() - 1;
        kDebug() << "Parameter type:" << type;
        if (type.contains("int") || type.contains("short") || type.contains("long") || type.contains("uint"))
//  if(type.contains("i"))
        {
            theValue->setCurrentIndex(2);
            theValueIntNumInput->setValue(theArguments[index].toInt());
        } else if (type.contains("double") || type.contains("float")) {
            theValue->setCurrentIndex(3);
            theValueDoubleNumInput->setValue(theArguments[index].toDouble());
        } else if (type.contains("bool")) {
            theValue->setCurrentIndex(1);
            theValueCheckBox->setChecked(theArguments[index].toBool());
        } else if (type.contains("QStringList")) {
            theValue->setCurrentIndex(4);
            QStringList backup = theArguments[index].toStringList();
            // backup needed because calling clear will kill what ever has been saved.
            theValueEditListBox->clear();
            theValueEditListBox->insertStringList(backup);
            theArguments[index].toStringList() = backup;
        } else {
            theValue->setCurrentIndex(0);
            theValueLineEdit->setText(theArguments[index].toString());
        }
        theCurParameter->setText(theParameters->currentItem()->text(0));
        theCurParameter->setEnabled(true);
        theValue->setEnabled(true);
    } else {
        theCurParameter->setText("");
        theValueLineEdit->setText("");
        theValueCheckBox->setChecked(false);
        theValueIntNumInput->setValue(0);
        theValueDoubleNumInput->setValue(0.0);
        theCurParameter->setEnabled(false);
        theValue->setEnabled(false);
    }
}

// called when the textbox/checkbox/whatever changes value
void AddAction::slotParameterChanged()
{
    kDebug() << "slotParameterChanged() called";
    if (!theParameters->currentItem()) return;
    int index = theParameters->currentItem()->text(3).toInt() - 1;
    QString type = theParameters->currentItem()->text(2);
    if (type.contains("int") || type.contains("short") || type.contains("long")) {
        theArguments[index] = theValueIntNumInput->value();
    } else if (type.contains("double") || type.contains("float")) {
        theArguments[index] = theValueDoubleNumInput->value();
    } else if (type.contains("bool")) {
        theArguments[index] = theValueCheckBox->isChecked();
    } else if (type.contains("QStringList")) {
        theArguments[index] = theValueEditListBox->items();
    } else {
        theArguments[index] = theValueLineEdit->text();
        kDebug() << "setting argument" << theArguments[index];
    }

// kDebug() << "setting argument nr: " << index << " to:" << theValueLineEdit->text() << "type is:" << type;
    theArguments[theParameters->currentItem()->text(3).toInt() - 1].convert(QVariant::nameToType(theParameters->currentItem()->text(2).toLocal8Bit()));
    updateArgument(theParameters->currentItem());
}

// takes theArguments[theIndex] and puts it into theItem
void AddAction::updateArgument(QTreeWidgetItem *theItem)
{
    kDebug() << "theArgument:" << theArguments[theItem->text(3).toInt() - 1] << "index:" << theItem->text(3).toInt() - 1;
    theItem->setText(1, theArguments[theItem->text(3).toInt() - 1].toString());
}

void AddAction::updateObjects()
{
    foreach(QString item, DBusInterface::getInstance()->getRegisteredPrograms()) {
        DBusServiceItem *tServiceItem = new DBusServiceItem(item);
	tServiceItem->setEditable(false);
        dbusAppsModel->appendRow(tServiceItem);
        foreach(QString object, DBusInterface::getInstance()->getObjects(item)) {
            tServiceItem->appendRow(new QStandardItem(object));
        }
    }
    dbusAppsModel->sort(0, Qt::AscendingOrder);
}


void AddAction::updatePrototyes(QModelIndex pIndex) {
    QModelIndex tParent = pIndex.parent();
    if (tParent.isValid()) {
        QList<Prototype> tList = DBusInterface::getInstance()->getFunctions(dbusAppsModel->data(tParent, Qt::UserRole).toString(), dbusAppsModel->data(pIndex).toString() );
        theFunctions->model()->insertRows(-1, tList.size());
        for (int i = 0; i < tList.size(); i++) {
            theFunctions->model()->setData(theFunctions->model()->index(i,0),qVariantFromValue( tList.at(i)), Qt::UserRole);
        }
        theFunctions->model()->sort(0, Qt::AscendingOrder);

    }
       theFunctions->resizeColumnsToContents();
}


IRAction* AddAction::getAction()
{
    IRAction *action = new IRAction();
    action->setRemote(theMode.remote());
    action->setMode(theMode.name());
    action->setButton(theButtons->currentItem()->data(Qt::UserRole).toString());
    action->setRepeat(theRepeat->isChecked());
    action->setAutoStart(theAutoStart->isChecked());
    action->setDoBefore(theDoBefore->isChecked());
    action->setDoAfter(theDoAfter->isChecked());
    action->setUnique(isUnique);
    action->setIfMulti(theDontSend->isChecked() ? IM_DONTSEND
                       : theSendToTop->isChecked() ? IM_SENDTOTOP
                       : theSendToBottom->isChecked() ? IM_SENDTOBOTTOM
                       : IM_SENDTOALL);
    // change mode?
    if (theChangeMode->isChecked()) {
        if (theSwitchMode->isChecked()
                && !theModes->selectedItems().isEmpty()) {
            action->setProgram("");
            action->setObject(theModes->selectedItems().first()->text());
        } else if (theExitMode->isChecked()) {
            action->setProgram("");
            action->setObject("");
        }
        action->setAutoStart(false);
        action->setRepeat(false);
    }
    // DBus?
    else if (theUseDBus->isChecked()
//TODO: do w need all thes checks ??
//              && !theObjects->selectedItems().isEmpty()
//              && theObjects->selectedItems().first()->parent()
             && !theFunctions->currentIndex().row() != -1) {
        action->setProgram(program);
        kDebug() << "programm                 ++++++++++++++++++  " << program;
//    kDebug() << "function                 ++++++++++++++++++  " << theFunctions->selectedItems().first()->text(2);
        Prototype p =   theFunctions->model()->data(theFunctions->currentIndex()).value<Prototype>();
        action->setMethod(p.argumentList());
        theParameters->sortItems(3, Qt::AscendingOrder);
        action->setArguments(theArguments);
    }
    // profile?
    else if (theUseProfile->isChecked()
             && !theProfiles->selectedItems().isEmpty()
             && (!theProfileFunctions->selectedItems().isEmpty()
                 || theJustStart->isChecked())) {
        ProfileServer *theServer = ProfileServer::profileServer();

        if (theNotJustStart->isChecked()) {
            const ProfileAction
            *theAction =
                theServer->getAction(
                    profileMap[theProfiles->selectedItems().first()],
                    profileFunctionMap[theProfileFunctions->selectedItems().first()]);
            action->setProgram(theAction->profile()->id());
            action->setObject(theAction->objId());
            action->setMethod(theAction->prototype());
            theParameters->sortItems(3, Qt::AscendingOrder);
            action->setArguments(theArguments);
        } else {
            action->setProgram(
                theServer->getProfileById(profileMap[theProfiles->selectedItems().first()])->id());
            action->setObject("");
        }
    }
    return action;
}
#include "addaction.moc"













/*
void AddAction::slotCorrectPage()
{
    int lastPage = curPage;
    curPage = this->currentId();
    int pageToShow=-1;
    kDebug() << "lastPage:" << lastPage << "; curPage:" << curPage;

    if (theUseProfile->isChecked() || theUseDBus->isChecked()) {
        QWizard::page(ACTION_ARGUMENTS)->setFinalPage(true);
    } else {
        QWizard::page(ACTION_ARGUMENTS)->setFinalPage(false);
    }

    //user has clicked back
    if (lastPage > curPage) {
	  if (curPage == SELECT_BUTTON) {            
                pageToShow = START;
         }
        else if (curPage == SELECT_FUNCTION_DBUS || curPage == SELECT_FUNCTION_PROFILE) {
            pageToShow = SELECT_BUTTON;
        }
        else if (curPage == ACTION_OPTIONS) {
	    if(theUseDBus->isChecked()){
		  curPage=SELECT_FUNCTION_DBUS;
	    } else if(theUseProfile->isChecked()){
		  curPage = SELECT_FUNCTION_PROFILE;
	    }            
        }
        else if (curPage == ACTION_ARGUMENTS) {
            if(theUseProfile->isChecked()){
	  curPage = SELECT_FUNCTION_PROFILE;
	  }else
            return;
        }


    } else if (lastPage < curPage) { //user has clicked next        
        if (curPage == START) {
	  pageToShow = SELECT_BUTTON;
	}
        else if (curPage == SELECT_BUTTON) {
            if (theUseDBus->isChecked()) {
                pageToShow = SELECT_FUNCTION_DBUS;
            }
            else if (theUseProfile->isChecked()) {
                pageToShow = SELECT_FUNCTION_PROFILE;
            }
            else if (theChangeMode->isChecked()) {
                pageToShow = ACTION_ARGUMENTS;
            }
        }
        else if (curPage == SELECT_FUNCTION_DBUS) {
            pageToShow = ACTION_ARGUMENTS;
        }
        else if (curPage == SELECT_FUNCTION_PROFILE) {
            pageToShow = ACTION_OPTIONS;
        }

        else if (curPage == ACTION_OPTIONS) {
            pageToShow = ACTION_ARGUMENTS;
            QList<QWizard::WizardButton> layout;
            layout << QWizard::Stretch << QWizard::BackButton << QWizard::FinishButton << QWizard::CancelButton;
            setButtonLayout(layout);
        }
        else if (curPage == ACTION_ARGUMENTS) {
            QList<QWizard::WizardButton> layout;
            layout << QWizard::Stretch << QWizard::BackButton << QWizard::FinishButton << QWizard::CancelButton;
            setButtonLayout(layout);
            return;
        }

    }
    else{
      
      pageToShow = START;
    }
    kDebug()<< "now wer are showing page " << pageToShow; 
      QWizard::page(curPage)->cleanupPage();
    QWizard::page(pageToShow)->show();
}
*/