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


#include "kcmlirc.h"
// #include "addaction.h"
// #include "newmodedialog.h"
#include "profileserver.h"
// #include "selectprofile.h"
// #include "editaction.h"
// #include "editmode.h"
#include "dbusinterface.h"
#include "model.h"
#include "remotelist.h"

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kconfiggroup.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kaboutdata.h>
#include <solid/control/remotecontrol.h>

#include <QStandardItemModel>
#include <QListWidget>
#include <QEvent>
#include <QHBoxLayout>
#include <QWidget>
#include <QDBusInterface>

#define VERSION "version name goes here"
#include <remote.h>



K_PLUGIN_FACTORY( KCMLircFactory, registerPlugin<KCMLirc>();)
K_EXPORT_PLUGIN( KCMLircFactory( "kcm_lirc" ) )

KCMLirc::KCMLirc(QWidget *parent, const QVariantList &args) :
        KCModule(KCMLircFactory::componentData(), parent, args)
{

    QDBusConnection dBusConnection = QDBusConnection::sessionBus();
    dBusConnection.registerObject("/KCMLirc", this,
                                  QDBusConnection::ExportAllSlots);

    qRegisterMetaType<Action*>("Action*");
    qRegisterMetaType<Mode*>("Mode*");
    qRegisterMetaType<Remote*>("Remote*");

    KGlobal::locale()->insertCatalog("kcm_lirc");
    setAboutData(
        new KAboutData(
            "kcm_lirc",
            0,
            ki18n("KDE Lirc"),
            VERSION, ki18n("The KDE IR Remote Control System"),
            KAboutData::License_GPL_V2,
            ki18n("Copyright (c)2003 Gav Wood"),
            ki18n(
                "Use this to configure KDE's infrared remote control system in order to control any KDE application with your infrared remote control."),
            "http://utils.kde.org/projects/kdelirc"));
    setQuickHelp(
        i18n(
            "<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognized applications and remote controls, simply select the <em>Loaded Profiles</em> tab.</p>"));

    if (!DBusInterface::getInstance()->isProgramRunning("org.kde.irkick")) {
        if (KMessageBox::questionYesNo(
                    this,
                    i18n(
                        "The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"),
                    i18n("Software Not Running"), KGuiItem(i18nc("Start irkick daemon","Start")), KGuiItem(
                        i18nc("Do not start irkick daemon", "Do Not Start"))) == KMessageBox::Yes) {
            kDebug() << "S" << KToolInvocation::startServiceByDesktopName("irkick");
            KConfig theConfig("irkickrc");
            KConfigGroup generalGroup = theConfig.group("General");
            if (generalGroup.readEntry("AutoStart", true) == false)
                if (KMessageBox::questionYesNo(
                            this,
                            i18n(
                                "Would you like the infrared remote control software to start automatically when you begin KDE?"),
                            i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")),
                            KGuiItem(i18n("Do Not Start"))) == KMessageBox::Yes)
                    generalGroup.writeEntry("AutoStart", true);
        }
    }

    QHBoxLayout *layout = new QHBoxLayout(this);

    QWidget *widget = new QWidget(this);
    theKCMLircBase = new Ui::KCMLircBase();
    theKCMLircBase->setupUi(widget);

    QStringList headers = (QStringList() << i18nc("Column which shows the available remotes on system", "Remote"));
    theKCMLircBase->theModes->setHeaderLabels(headers);
    layout->addWidget(widget);
    
    
    
    connectSignalsAndSlots();
    load();
}

void KCMLirc::connectSignalsAndSlots() {
    connect(theKCMLircBase->theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateActions()));
    connect(theKCMLircBase->theModes, SIGNAL(itemSelectionChanged()), this, SLOT(updateModesStatus()));
    connect(theKCMLircBase->theActions, SIGNAL(itemSelectionChanged()), this, SLOT(updateActionsStatus()));
    connect(theKCMLircBase->theActions, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotEditAction()));

    connect(theKCMLircBase->theAvailableProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateProfileDetails()));

    connect(theKCMLircBase->theAddActions, SIGNAL(clicked()), this, SLOT(slotAddActions()));
    connect(theKCMLircBase->theAddAction, SIGNAL(clicked()), this, SLOT(slotAddAction()));
    connect(theKCMLircBase->theEditAction, SIGNAL(clicked()), this, SLOT(slotEditAction()));

    connect(theKCMLircBase->theRemoveAction, SIGNAL(clicked()), this, SLOT(slotRemoveAction()));
    connect(theKCMLircBase->theAddMode, SIGNAL(clicked()), this, SLOT(slotAddMode()));
    connect(theKCMLircBase->theEditMode, SIGNAL(clicked()), this, SLOT(slotEditMode()));
    connect(theKCMLircBase->theRemoveMode, SIGNAL(clicked()), this, SLOT(slotRemoveMode()));
    connect(theKCMLircBase->theAvailableProfiles, SIGNAL(clicked(QModelIndex)), this, SLOT(updateProfileDetails(QModelIndex)));
    connect(theKCMLircBase->theRemotes, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRemoteDetails(QModelIndex)));
}



KCMLirc::~KCMLirc()
{
}

void KCMLirc::updateModesStatus()
{
    if (! theKCMLircBase->theModes->selectedItems().isEmpty()) {
        QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
        bool remoteSelected = item->isSelected();
        theKCMLircBase->theAddActions->setEnabled(remoteSelected && ProfileServer::allProfiles().count());
        theKCMLircBase->theAddAction->setEnabled(remoteSelected);
        theKCMLircBase->theAddMode->setEnabled(remoteSelected);
        theKCMLircBase->theRemoveMode->setEnabled(item->parent() != 0);
        theKCMLircBase->theEditMode->setEnabled(remoteSelected);
    }
}

void KCMLirc::updateActionsStatus()
{
    theKCMLircBase->theRemoveAction->setEnabled(theKCMLircBase->theActions->currentIndex().isValid());
    theKCMLircBase->theEditAction->setEnabled(theKCMLircBase->theActions->currentIndex().isValid());
}

void KCMLirc::slotEditAction()
{
//     QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
//     if (item->parent())
//         item = item->parent();
//     QStringList modeList;
//     for (int i = 0; i < item->childCount(); i++) {
//         modeList << item->child(i)->text(0);
//     }
//     QPointer<EditAction > theDialog = new EditAction(currentAction(), modeList,this);
//     if (theDialog->exec() == QDialog::Accepted) {
//         allActions[allActions.indexOf(currentAction())] = theDialog->getAction();
//         updateActions();
// 	emit changed(true);
//     }
}

void KCMLirc::slotAddActions()
{
//     if (theKCMLircBase->theModes->selectedItems().isEmpty()) {
//         return;
//     }
//     Mode mode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//      QPointer<SelectProfile> tDialog = new SelectProfile( mode.remote(),this);
//     if (tDialog->exec() == QDialog::Accepted) {
//       autoPopulate(tDialog->getSelectedProfile(),mode);
//       updateActions();
//       emit changed(true);
//     }
}

void KCMLirc::slotAddAction()
{
//     kDebug();
//     if (theKCMLircBase->theModes->selectedItems().isEmpty()){
//         return;
//     }
//     Mode m = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//     QPointer<AddAction> theDialog = new AddAction(this, 0, m);
// 
//     // populate the modes list box
//     QTreeWidgetItem *item = theKCMLircBase->theModes->selectedItems().first();
//     if (item->parent())
//         item = item->parent();
//     theDialog->theModes->setEnabled(item->child(0));
//     theDialog->theSwitchMode->setEnabled(item->child(0));
//     for (int i = 0; i < item->childCount(); i++) {
//         QListWidgetItem *a = new QListWidgetItem(item->child(i)->text(0),
//                 theDialog->theModes);
//         if (item->isSelected()) {
//             a->setSelected(true);
//             theDialog->theModes->setCurrentItem(a);
//         }
//     }
// 
//     if (theDialog->exec() == QDialog::Accepted) {
//         allActions.addAction(theDialog->getAction());
//         updateActions();
//         emit changed(true);
//     }    
}

void KCMLirc::slotRemoveAction()
{
//     m_actionList.erase(currentAction());
    m_actionList.removeAll(currentAction());
    updateActions();
    emit changed(true);
}

void KCMLirc::autoPopulate(const Profile &profile, const Remote &remote)
{
 /*   QStringList buttonList = Remotes::remotes()
    
    foreach (Solid::Control::RemoteControlButton button, remote.remote().buttons() ) {
        const ProfileAction *pa = profile->getProfileActionByButton(button);

        if (pa) {
	  ProfileAction tAction();
            Action *action = new Action(*pamode);
	   action->
            action->setMode(mode.name());
            action->setRepeat(pa->repeat());
            action->setAutoStart(pa->autoStart());
            action->setProgram(pa->profile()->id());
            action->setObject(pa->objId());
            action->setMethod(pa->prototype());
            action->setUnique(pa->profile()->unique());
            action->setIfMulti(pa->profile()->ifMulti());
            Arguments arguments;
            kDebug() << "Argumentcount" << Prototype(pa->prototype()).argumentCount();

            for (int j = 0; j < pa->arguments().size(); ++j) {
                ProfileActionArgument tArgument= pa->arguments().at(j);
                QVariant tVariant;
                if (!tArgument.getDefault().toString().isEmpty()) {
                    tVariant = tArgument.getDefault();
                    tVariant.convert(QVariant::nameToType(tArgument.type().toLocal8Bit()));
                } else {
                    tVariant = QString().setNum(pa->multiplier());
                    tVariant.convert(QVariant::nameToType(Prototype(pa->prototype()).type(0).toLocal8Bit()));
                }
                arguments.append(tVariant);
                kDebug() << "added argument: " << tVariant;
            }
            action->setArguments(arguments);
           */
// 	   m_actionList.addAction(action);
//         }
//     }
}

void KCMLirc::slotAddMode()
{
//     if (theKCMLircBase->theModes->selectedItems().isEmpty()) {
//         return;
//     }
//     QPointer<NewModeDialog>  theDialog = new NewModeDialog(allModes, this,0);
//     if (theDialog->exec() == QDialog::Accepted) {
//         Mode mode = theDialog->getMode();
//         allModes.add(mode);
//         if (theDialog->isDefaultMode()) {
//             allModes.setDefault(mode);
//         }
//         updateModes();
//         emit changed(true);
//     }
}

void KCMLirc::slotEditMode()
{
//     if (theKCMLircBase->theModes->selectedItems().isEmpty())
//         return;
// 
//     Mode mode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//     QPointer<EditMode> theDialog = new EditMode(mode,allModes, this, 0);
// 
//     if (theDialog->exec() == QDialog::Accepted) {
//         Mode newMode = theDialog->getMode();
//         mode.setIconFile(newMode.iconFile());
//         if (!mode.name().isEmpty()) {
//             allActions.renameMode(mode, newMode.name());
//             allModes.rename(mode, newMode.name());
//         }
//         if (theDialog->isDefaultMode()) {
//             allModes.setDefault(mode);
//         }
//         allModes.updateMode(mode);
//         emit changed(true);
//         updateModes();
//     }
}

void KCMLirc::slotRemoveMode()
{

    Mode *tMode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>();
    Remote *remote =  theKCMLircBase->theModes->currentItem()->parent()->data(0, Qt::UserRole).value<Remote*>();
    if (KMessageBox::warningContinueCancel(this, i18n(
                                               "Are you sure you want to remove %1 and all its actions?",
                                               tMode->name()), i18n(
                                               "Erase Actions?")) == KMessageBox::Continue) {
        theKCMLircBase->theModes->clear();

	remote->removeMode(*tMode);
        updateModes();
        emit changed(true);
    }

}

void KCMLirc::slotSetDefaultMode()
{
    if (!theKCMLircBase->theModes->currentItem()) {
        return;
    }
    Remote *remote = theKCMLircBase->theModes->currentItem()->parent()->data(0, Qt::UserRole).value<Remote*>();
    remote->setDefaultMode(*(theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>()));
    updateModes();
    emit changed(true);
}

void KCMLirc::updateActions()
{
    if (!theKCMLircBase->theModes->currentItem()) {
        return;
    }

  
//     IRAction *oldCurrent = 0;
//     if (theKCMLircBase->theActions->currentIndex().isValid()) {
//         oldCurrent = currentAction();
//     }
//     theKCMLircBase->theActions->clear();
//     Mode m = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//     theKCMLircBase->theModeLabel->setText(m.remote() + ": "
//                                           + (m.name().isEmpty() ? i18n("Actions <i>always</i> available") :
//                                              i18n("Actions available only in mode <b>%1</b>", m.name())));
//     IRActions allActionsList = m_actionList.findByMode(m);
// 
//     foreach(IRAction *tAction, allActionsList) {
//         QStringList tActionRow;
//         tActionRow << tAction->getButton().description();
//         tActionRow << tAction->application() << tAction->function() << tAction->arguments().toString() << notes(tAction);
//         QTreeWidgetItem *tActionItem = new  QTreeWidgetItem(tActionRow);
//         tActionItem->setData(0, Qt::UserRole, qVariantFromValue(tAction));
//         theKCMLircBase->theActions->addTopLevelItem(tActionItem);
//     }
//     updateActionsStatus();
}


const QString KCMLirc::notes(Action* action) const
{

//     if (action->isModeChange())
//         return QString(action->doBefore() ? i18n("Do actions before. ") : "") +
//                QString(action->doAfter() ? i18n("Do actions after. ") : "");
//     else if (action->isJustStart())
//         return "";
//     else
//         return QString(action->autoStart() ? i18n("Auto-start. ") : "")
//                + QString(action->repeat() ? i18n("Repeatable. ") : "")
//                + QString(!action->unique() ? (action->ifMulti() == IM_DONTSEND ? i18n("Do nothing if many instances. ")
//                                        : action->ifMulti()== IM_SENDTOTOP ? i18n("Send to top instance. ")
//                                        : action->ifMulti() == IM_SENDTOBOTTOM ? i18n("Send to bottom instance. ") : i18n("Send to all instances. "))
//                                  : "");
}

void KCMLirc::updateModes()
{
    Mode *currentSelectedMode;
    if (theKCMLircBase->theModes->currentItem()) {
        currentSelectedMode = theKCMLircBase->theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>();
    }

    theKCMLircBase->theModes->clear();

    if (Remotes::remotes().size()==0) {
        //theKCMLircBase->theMainLabel->setMaximumSize(32767, 32767);
	theKCMLircBase->theMainLabel->setVisible(true);
	theKCMLircBase->tabWidget->setEnabled(false);
        return;
    }
    else {
      theKCMLircBase->theMainLabel->setVisible(false);
      theKCMLircBase->tabWidget->setEnabled(true);
//         theKCMLircBase->theMainLabel->setMaximumSize(0, 0);
    }
    foreach(Remote remote, Remotes::remotes()){
        QFont font = KApplication::font();
        font.setBold(true);
        QFontMetrics tFontMetric(font);
        QFont tFont = QFont();
	QTreeWidgetItem *remoteTreeWidgetIcon = new QTreeWidgetItem(theKCMLircBase->theModes, QStringList() << remote.remoteName());
	tFont.setBold(true);
	remoteTreeWidgetIcon->setFont(0, tFont);
        remoteTreeWidgetIcon->setExpanded(true);	
	foreach(Mode mode, remote.allModes()){
	  QTreeWidgetItem *modeWidgetItem = new QTreeWidgetItem(remoteTreeWidgetIcon, QStringList() << mode.name());
	  if (! mode.iconName().isNull()) {
	    modeWidgetItem->setIcon(0, KIconLoader().loadIcon(mode.iconName(), KIconLoader::Panel));
	  }
	    tFont.setBold(remote.defaultMode() == mode);
            modeWidgetItem->setFont(0, tFont);
	    modeWidgetItem->setToolTip(0,mode.name());
            modeWidgetItem->setToolTip(1,mode.name());
            modeWidgetItem->setData(0, Qt::UserRole, qVariantFromValue(mode));
	   if (mode.name() == currentSelectedMode->name()) {
            theKCMLircBase->theModes->setCurrentItem(modeWidgetItem);
           }
          
	}
	remoteTreeWidgetIcon->sortChildren(0, Qt::AscendingOrder);
// 	remoteTreeWidgetIcon->setData(0, Qt::UserRole, qVariantFromValue(remote));
    }

    if (theKCMLircBase->theModes->currentItem() == 0) {
        theKCMLircBase->theModes->setCurrentItem(theKCMLircBase->theModes->topLevelItem(0));
    }    
    theKCMLircBase->theModes->currentItem()->setSelected(true);
    updateModesStatus();
    updateActions();
}


void KCMLirc::updateProfileInfo()
{
    ProfileServer::allProfiles();
    
    QStandardItemModel *tModel = new QStandardItemModel( theKCMLircBase->theAvailableProfiles);
    foreach(const Profile &profile , ProfileServer::allProfiles()) {
      QStandardItem *tItem = new QStandardItem();
      tItem->setData(profile.name(), Qt::DisplayRole);
      tItem->setData(profile.profileId(), Qt::UserRole);
      tModel->appendRow(tItem);
      tModel->setHorizontalHeaderLabels(QStringList()<< i18n("Profiles"));
    }
    tModel->sort(0, Qt::AscendingOrder);
    theKCMLircBase->theAvailableProfiles->setModel(tModel);
    //updateInformation();
}

void KCMLirc::updateProfileDetails(QModelIndex index)
{
    theKCMLircBase->theProfileInformation->clear();
    theKCMLircBase->theProfileInformation->clear();
    if (!index.isValid()){
        return;
    }
    Profile tProfile = ProfileServer::profile(theKCMLircBase->theAvailableProfiles->model()->data(index,Qt::UserRole).toString());
    QStringList infoList;
    infoList << i18n("Profile Name") << tProfile.name();
    new QTreeWidgetItem(theKCMLircBase->theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Profile Author") << tProfile.author();
    new QTreeWidgetItem(theKCMLircBase->theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Profile Identifier") << tProfile.profileId();
    new QTreeWidgetItem(theKCMLircBase->theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Number of Actions") << QString().setNum(tProfile.actionTemplates().count());
    new QTreeWidgetItem(theKCMLircBase->theProfileInformation, infoList);
    theKCMLircBase->theProfileActions->setModel(new ProfileModel(&tProfile, theKCMLircBase->theProfileActions));
  }



void KCMLirc::updateRemoteDetails(QModelIndex index)
{
//   QString tSelectedRemote = theKCMLircBase->theRemotes->model()->data(index).toString();
//   theKCMLircBase->theRemoteButtons->setModel(new RemoteButtonModel(Solid::Control::RemoteControl(tSelectedRemote).buttons(), theKCMLircBase->theRemoteButtons));
}


void KCMLirc::updateRemoteInfo()
{
//   theKCMLircBase->theRemotes->setModel(new  RemoteModel(Solid::Control::RemoteControl::allRemoteNames(), theKCMLircBase->theRemoteButtons));
}


void KCMLirc::load()
{
    KConfig theConfig("irkickrc");
/*/*    m_actionList.loadFromConfig(theConfig);
    m_modeList.loadFromConfig(theConfig);*/
    QStringList remotes = DBusInterface::getInstance()->getRemotes();

//     m_modeList.generateNulls(remotes);

     updateProfileInfo();
    updateModes();
     updateActions();
     updateRemoteInfo();
}

void KCMLirc::defaults()
{
    emit changed(true);
}

void KCMLirc::save()
{
    KConfig theConfig("irkickrc");

    //Todo save to config interface
//     m_actionList.saveToConfig(theConfig);
//     m_modeList.saveToConfig(theConfig);

    theConfig.sync();

    DBusInterface::getInstance()->reloadIRKick();

    emit changed(true);
}

void KCMLirc::configChanged()
{
    // insert your saving code here...
    emit changed(true);
}

void KCMLirc::gotButton(QString remote, QString button)
{
    emit haveButton(remote, button);
}

#include "kcmlirc.moc"
