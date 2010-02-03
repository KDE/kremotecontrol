/*************************************************************************
 * Copyright: (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>      *
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


#include "kcmlirc.h"
#include "addaction.h"
#include "editactioncontainer.h"
#include "remote.h"
#include "editactioncontainer.h"

#include "newmodedialog.h"
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



K_PLUGIN_FACTORY( KCMLircFactory, registerPlugin<KCMLirc>();)
K_EXPORT_PLUGIN( KCMLircFactory( "kcm_lirc" ) )

KCMLirc::KCMLirc(QWidget *parent, const QVariantList &args) :
        KCModule(KCMLircFactory::componentData(), parent, args)
{

    QDBusConnection::sessionBus().registerObject("/KCMLirc", this, QDBusConnection::ExportAllSlots);

    KGlobal::locale()->insertCatalog("kcm_lirc");
    setAboutData(
        new KAboutData(
            "kcm_lirc",
            0,
            ki18n("KDE Lirc"),
            VERSION, ki18n("The KDE Remote Control System"),
            KAboutData::License_GPL_V2,
            ki18n("Copyright (c)2003 Gav Wood, 2007 Michael Zanetti, 2009 Frank Scheffold"),
            ki18n(
                "Use this to configure KDE's remote control system in order to control any KDE application with your remote control."),
            "http://utils.kde.org/projects/kdelirc"));
    setQuickHelp(
        i18n(
            "<h1>Remote Controls</h1><p>This module allows you to configure bindings between your remote controls and KDE applications. Simply select your remote control and click Add under the Actions/Buttons list. If you want KDE to attempt to automatically assign buttons to a supported application's actions, try clicking the Auto-Populate button.</p><p>To view the recognized applications and remote controls, simply select the <em>Loaded Profiles</em> tab.</p>"));

//     if (!DBusInterface::getInstance()->isProgramRunning("org.kde.irkick")) {
//         if (KMessageBox::questionYesNo(
//                     this,
//                     i18n(
//                         "The Infrared Remote Control software is not currently running. This configuration module will not work properly without it. Would you like to start it now?"),
//                     i18n("Software Not Running"), KGuiItem(i18nc("Start irkick daemon","Start")), KGuiItem(
//                         i18nc("Do not start irkick daemon", "Do Not Start"))) == KMessageBox::Yes) {
//             kDebug() << "S" << KToolInvocation::startServiceByDesktopName("irkick");
//             KConfig theConfig("irkickrc");
//             KConfigGroup generalGroup = theConfig.group("General");
//             if (generalGroup.readEntry("AutoStart", true) == false)
//                 if (KMessageBox::questionYesNo(
//                             this,
//                             i18n(
//                                 "Would you like the infrared remote control software to start automatically when you begin KDE?"),
//                             i18n("Automatically Start?"), KGuiItem(i18n("Start Automatically")),
//                             KGuiItem(i18n("Do Not Start"))) == KMessageBox::Yes)
//                     generalGroup.writeEntry("AutoStart", true);
//         }
//     }

    QHBoxLayout *layout = new QHBoxLayout(this);
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    layout->addWidget(widget);

    ui.pbAddMode->setIcon(KIcon("list-add"));
    connect(ui.pbAddMode, SIGNAL(clicked(bool)), SLOT(addMode()));

    ui.pbRemoveMode->setIcon(KIcon("list-remove"));
    connect(ui.pbRemoveMode, SIGNAL(clicked(bool)), SLOT(removeMode()));

    ui.pbAddAction->setIcon(KIcon("list-add"));
    connect(ui.pbAddAction, SIGNAL(clicked(bool)), SLOT(addAction()));

    ui.pbRemoveAction->setIcon(KIcon("list-remove"));

    ui.pbEditMode->setIcon(KIcon("configure"));
    ui.pbEditAction->setIcon(KIcon("configure"));
    
    ui.pbMoveModeUp->setIcon(KIcon("arrow-up"));
    ui.pbMoveModeDown->setIcon(KIcon("arrow-down"));
    ui.pbMoveActionUp->setIcon(KIcon("arrow-up"));
    ui.pbMoveActionDown->setIcon(KIcon("arrow-down"));

    ui.pbCopyAction->setIcon(KIcon("edit-copy"));

//    connectSignalsAndSlots();
//    load();
    
  
    m_remoteModel = new RemoteModel(m_remoteList, ui.tvRemotes);
    ui.tvRemotes->setModel(m_remoteModel);
    connect(ui.tvRemotes->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), SLOT(updateModeButtons(const QModelIndex &)));
    
    
    
     updateProfileInfo();
     updateModes();
     updateActions();
     updateRemoteInfo();

    
//    save();
}


// void KCMLirc::connectSignalsAndSlots() {
//   
//   connect(ui.pbAddAction, SIGNAL(clicked(bool)), SLOT(addAction()));
//   
//     connect(ui.tvRemotes, SIGNAL(itemSelectionChanged()), this, SLOT(updateActions()));
//     connect(ui.tvRemotes, SIGNAL(itemSelectionChanged()), this, SLOT(updateModesStatus()));
// //     connect(ui.theActions, SIGNAL(itemSelectionChanged()), this, SLOT(updateActionsStatus()));
// //     connect(ui.theActions, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotEditAction()));
// 
//     connect(ui.theAvailableProfiles, SIGNAL(itemSelectionChanged()), this, SLOT(updateProfileDetails()));
// 
// /*    connect(ui.theAddActions, SIGNAL(clicked()), this, SLOT(slotAddActions()));
//     connect(ui.theAddAction, SIGNAL(clicked()), this, SLOT(slotAddAction()));
//     connect(ui.theEditAction, SIGNAL(clicked()), this, SLOT(slotEditAction()));
// 
//     connect(ui.theRemoveAction, SIGNAL(clicked()), this, SLOT(slotRemoveAction()));
//     connect(ui.theAddMode, SIGNAL(clicked()), this, SLOT(slotAddMode()));
//     connect(ui.theEditMode, SIGNAL(clicked()), this, SLOT(slotEditMode()));
//     connect(ui.theRemoveMode, SIGNAL(clicked()), this, SLOT(slotRemoveMode()));*/
//     connect(ui.theAvailableProfiles, SIGNAL(clicked(QModelIndex)), this, SLOT(updateProfileDetails(QModelIndex)));
//     connect(ui.theRemotes, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRemoteDetails(QModelIndex)));
// }
// 


KCMLirc::~KCMLirc()
{
}

void KCMLirc::updateModesStatus()
{
/*    if (! ui.tvRemotes->selectionModel()->hasSelection()) {
//        QTreeWidgetItem *item = ui.tvRemotes->selectionModel()->se.first();
        bool remoteSelected = item->isSelected();
        ui.theAddActions->setEnabled(remoteSelected && ProfileServer::allProfiles().count());
        ui.theAddAction->setEnabled(remoteSelected);
        ui.theAddMode->setEnabled(remoteSelected);
        ui.theRemoveMode->setEnabled(item->parent() != 0);
        ui.theEditMode->setEnabled(remoteSelected);
    }*/
}

void KCMLirc::updateActionsStatus()
{
/*    ui.theRemoveAction->setEnabled(ui.theActions->currentIndex().isValid());
    ui.theEditAction->setEnabled(ui.theActions->currentIndex().isValid());*/
}

void KCMLirc::slotEditAction()
{
//     QTreeWidgetItem *item = ui.theModes->selectedItems().first();
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
//     if (ui.theModes->selectedItems().isEmpty()) {
//         return;
//     }
//     Mode mode = ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//      QPointer<SelectProfile> tDialog = new SelectProfile( mode.remote(),this);
//     if (tDialog->exec() == QDialog::Accepted) {
//       autoPopulate(tDialog->getSelectedProfile(),mode);
//       updateActions();
//       emit changed(true);
//     }
}

void KCMLirc::addAction()
{
    // TODO: get currently selected remote!!!
    QString remote = Solid::Control::RemoteControl::allRemotes().first()->name();

    AddAction addActionDialog;
    Action *newAction = addActionDialog.createAction(remote);
    if(newAction != 0){
        // Yay... the new action is here
        kDebug() << "created action:" << newAction->type() << newAction->name() << newAction->description();
    }
}

void KCMLirc::slotRemoveAction()
{
//     m_actionList.erase(currentAction());
//     m_actionList.removeAll(currentAction());
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

void KCMLirc::addMode()
{
    Remote *remote = m_remoteModel->remote(ui.tvRemotes->selectionModel()->currentIndex());
    kDebug() << "current selected remote:" << remote;
    NewModeDialog newModeDialog(remote);
    if(newModeDialog.exec()){
        m_remoteModel->refresh(m_remoteList);
        foreach(const Mode *mode, remote->allModes()){
            kDebug() << "Created Mode" << mode->name();
        }
        updateModes();
        emit changed(true);
    }
  
//     if (ui.theModes->selectedItems().isEmpty()) {
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
//     if (ui.theModes->selectedItems().isEmpty())
//         return;
//
//     Mode mode = ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
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

void KCMLirc::removeMode() {

    QModelIndex currentIndex = ui.tvRemotes->selectionModel()->currentIndex();
    Remote *remote = m_remoteModel->remote(currentIndex);
    Mode *mode = m_remoteModel->mode(currentIndex);
    if(remote && remote->allModes().contains(mode)){
        remote->removeMode(mode);
        updateModes();
        emit changed(true);
    }
  
//     Mode *tMode = ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>();
//     Remote *remote =  ui.theModes->currentItem()->parent()->data(0, Qt::UserRole).value<Remote*>();
//     if (KMessageBox::warningContinueCancel(this, i18n(
//                                                "Are you sure you want to remove %1 and all its actions?",
//                                                tMode->name()), i18n(
//                                                "Erase Actions?")) == KMessageBox::Continue) {
//         ui.theModes->clear();
// 
// 	remote->removeMode(*tMode);
//         updateModes();
//         emit changed(true);
//     }

}

void KCMLirc::slotSetDefaultMode()
{
/*    if (!ui.theModes->currentItem()) {
        return;
    }
    Remote *remote = ui.theModes->currentItem()->parent()->data(0, Qt::UserRole).value<Remote*>();
    remote->setDefaultMode(*(ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>()));
    updateModes();
    emit changed(true);*/
}

void KCMLirc::updateActions()
{
//     if (!ui.theModes->currentItem()) {
//         return;
//     }


//     IRAction *oldCurrent = 0;
//     if (ui.theActions->currentIndex().isValid()) {
//         oldCurrent = currentAction();
//     }
//     ui.theActions->clear();
//     Mode m = ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode>();
//     ui.theModeLabel->setText(m.remote() + ": "
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
//         ui.theActions->addTopLevelItem(tActionItem);
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
    m_remoteModel->refresh(m_remoteList);
    ui.tvRemotes->expandAll();
    updateModeButtons(ui.tvRemotes->selectionModel()->currentIndex());
  
/*    Mode *currentSelectedMode;
    if (ui.theModes->currentItem()) {
        currentSelectedMode = ui.theModes->currentItem()->data(0, Qt::UserRole).value<Mode*>();
    }

    ui.theModes->clear();

    if (Remotes::remotes().size()==0) {
        //ui.theMainLabel->setMaximumSize(32767, 32767);
	ui.tabWidget->setEnabled(false);
        return;
    }
    else {
      ui.tabWidget->setEnabled(true);
//         ui.theMainLabel->setMaximumSize(0, 0);
    }
    foreach(Remote remote, Remotes::remotes()){
        QFont font = KApplication::font();
        font.setBold(true);
        QFontMetrics tFontMetric(font);
        QFont tFont = QFont();
	QTreeWidgetItem *remoteTreeWidgetIcon = new QTreeWidgetItem(ui.theModes, QStringList() << remote.remoteName());
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
            ui.theModes->setCurrentItem(modeWidgetItem);
           }

	}
	remoteTreeWidgetIcon->sortChildren(0, Qt::AscendingOrder);
// 	remoteTreeWidgetIcon->setData(0, Qt::UserRole, qVariantFromValue(remote));
    }

    if (ui.theModes->currentItem() == 0) {
        ui.theModes->setCurrentItem(ui.theModes->topLevelItem(0));
    }
    ui.theModes->currentItem()->setSelected(true);
    updateModesStatus();
    updateActions();*/
}

void KCMLirc::updateModeButtons(const QModelIndex &index){
    if(index.isValid()){
        ui.pbAddMode->setEnabled(true);
    } else {
        ui.pbAddMode->setEnabled(false);
    }
    
    if(index.isValid() && index.parent().isValid()){
        ui.pbRemoveMode->setEnabled(true);
        ui.pbEditMode->setEnabled(true);
    } else {
        ui.pbRemoveMode->setEnabled(false);
        ui.pbEditMode->setEnabled(false);
    }
}

void KCMLirc::updateProfileInfo()
{
    ProfileServer::allProfiles();

    QStandardItemModel *tModel = new QStandardItemModel( ui.theAvailableProfiles);
    foreach(Profile *profile , ProfileServer::allProfiles()) {
      QStandardItem *tItem = new QStandardItem();
      tItem->setData(profile->name(), Qt::DisplayRole);
      tItem->setData(profile->profileId(), Qt::UserRole);
      tModel->appendRow(tItem);
      tModel->setHorizontalHeaderLabels(QStringList()<< i18n("Profiles"));
    }
    tModel->sort(0, Qt::AscendingOrder);
    ui.theAvailableProfiles->setModel(tModel);
    //updateInformation();
}

void KCMLirc::updateProfileDetails(QModelIndex index)
{
    ui.theProfileInformation->clear();
    ui.theProfileInformation->clear();
    if (!index.isValid()){
        return;
    }
    Profile *tProfile = ProfileServer::profile(ui.theAvailableProfiles->model()->data(index,Qt::UserRole).toString());
    if(tProfile){
    QStringList infoList;
    infoList << i18n("Profile Name") << tProfile->name();
    new QTreeWidgetItem(ui.theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Profile Author") << tProfile->author();
    new QTreeWidgetItem(ui.theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Profile Identifier") << tProfile->profileId();
    new QTreeWidgetItem(ui.theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Profile Version") << tProfile->version();
    new QTreeWidgetItem(ui.theProfileInformation, infoList);
    infoList.clear();
    infoList << i18n("Number of Actions") << QString().setNum(tProfile->actionTemplates().count());
    new QTreeWidgetItem(ui.theProfileInformation, infoList);
    ui.theProfileActions->setModel(new ProfileModel(tProfile, ui.theProfileActions));
    }
  }



void KCMLirc::updateRemoteDetails(QModelIndex index)
{
//   QString tSelectedRemote = ui.theRemotes->model()->data(index).toString();
//   ui.theRemoteButtons->setModel(new RemoteButtonModel(Solid::Control::RemoteControl(tSelectedRemote).buttons(), ui.theRemoteButtons));
}


void KCMLirc::updateRemoteInfo()
{
//   ui.theRemotes->setModel(new  RemoteModel(Solid::Control::RemoteControl::allRemoteNames(), ui.theRemoteButtons));
}


void KCMLirc::load() {
  
    KConfig config("kremotecontrolrc", KConfig::NoGlobals);
        
    foreach(const QString &remoteGroupName, config.groupList()){
        Remote *remote = new Remote(remoteGroupName);
        KConfigGroup remoteGroup(&config, remoteGroupName);
        foreach(const QString &modeName, remoteGroup.groupList()){
            KConfigGroup modegroup(&remoteGroup, modeName);
            remote->addMode(new Mode(modeName, modegroup.readEntry("IconName")));
        }
        
        m_remoteList.append(remote);
    }

    updateModes();
//     KConfigGroup modesGroup = theConfig.group("Modes");
//     clear();
//     QString helperString = modesGroup.readEntry("Modes", QString());
//     int numModes = helperString.toInt();
//     for (int i = 0; i < numModes; i++) {
//         add(Mode().loadFromConfig(theConfig, i));
//     }
// 
//     for (iterator i = begin(); i != end(); ++i)
//         theDefaults[i.key()] = modesGroup.readEntry("Default" + i.key(), QString());

  
  
  
  
  
  
  
  
/*    foreach(Solid::Control::RemoteControl *remote, Solid::Control::RemoteControl::allRemotes()){
        
        m_remoteList.append(remote->name());
    }*/
    
/*/*    m_actionList.loadFromConfig(theConfig);
    m_modeList.loadFromConfig(theConfig);*/
    QStringList remotes = DBusInterface::getInstance()->getRemotes();

//     m_modeList.generateNulls(remotes);

}

void KCMLirc::defaults()
{
    emit changed(true);
}

void KCMLirc::save() {
    KConfig config("kremotecontrolrc");    
    foreach(const Remote *remote, m_remoteList){
        config.deleteGroup(remote->name());
        KConfigGroup remoteGroup(&config, remote->name());
        remoteGroup.writeEntry("DefaultMode", remote->defaultMode()->name());
        foreach(const Mode *mode, remote->allModes()){
            KConfigGroup modeGroup(&remoteGroup, mode->name());
            modeGroup.writeEntry("IconName", mode->iconName());
                
        }        
    }

    //Todo save to config interface
//     m_actionList.saveToConfig(theConfig);
//     m_modeList.saveToConfig(theConfig);

//    theConfig.sync();

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
