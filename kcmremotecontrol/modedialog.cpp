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


/**
  * @author Michael Zanetti
  */


#include "modedialog.h"
#include "mode.h"
#include "dbusinterface.h"

#include <kdebug.h>
#include <kmessagebox.h>

ModeDialog::ModeDialog(Remote *remote, Mode *mode, QWidget *parent): KDialog(parent), m_remote(remote), m_mode(mode) {
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);

    ui.cbButtons->addItem(i18n("No button"), "");
    foreach(const QString &button, remote->availableModeSwitchButtons(mode)){
        ui.cbButtons->addItem(button, button);
    }
    
    if(m_mode){
        ui.leName->setText(m_mode->name());
        ui.ibIcon->setIcon(m_mode->iconName());
        ui.cbButtons->setCurrentIndex(ui.cbButtons->findData(m_mode->button()));
        ui.cbSetDefault->setChecked(remote->defaultMode() == mode);
        if(m_mode == remote->masterMode()){
            // Hide Non-Master-Mode options
            ui.cbButtons->setVisible(false);
            ui.lButton->setVisible(false);
            ui.leName->setEnabled(false);
            
            // Fill in Cycle mode buttons
            ui.cbButtonForward->addItem(i18n("No Button"), "");                        
            foreach(const QString &button, remote->availableNextModeButtons()){
                ui.cbButtonForward->addItem(button, button);
            }
            ui.cbButtonForward->setCurrentIndex(ui.cbButtonForward->findData(remote->nextModeButton()));

            ui.cbButtonBackward->addItem(i18n("No Button"), "");
            foreach(const QString &button, remote->availablePreviousModeButtons()){
                ui.cbButtonBackward->addItem(button, button);
            }
            ui.cbButtonBackward->setCurrentIndex(ui.cbButtonBackward->findData(remote->previousModeButton()));

            if(remote->modeChangeMode() == Remote::Cycle){
                ui.gbModeCycle->setChecked(true);
            } else {
                ui.gbModeCycle->setChecked(false);
            }
        } else {
            ui.gbModeCycle->setVisible(false);
        }
    } else {
        ui.ibIcon->setIcon("infrared-remote");
    }
    
    connect(ui.leName, SIGNAL(textChanged(const QString&)), this, SLOT(checkForComplete()));
    connect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged(int)));
    connect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged(int)));
    checkForComplete();

    // Pause remote to make use of button presses here
    DBusInterface::getInstance()->ignoreButtonEvents(remote->name());
    connect(new Solid::Control::RemoteControl(remote->name()), SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), SLOT(buttonPressed(const Solid::Control::RemoteControlButton &)));
}

ModeDialog::~ModeDialog() {
}

void ModeDialog::checkForComplete() {
    if(ui.leName->text().isEmpty()){
        enableButtonOk(false);
        return;
    }
    foreach(const Mode *mode, m_remote->allModes()){
        if(m_mode == mode){
            continue; // Don't check the current Mode during Edit
        }
        if(mode->name() == ui.leName->text()){
            enableButtonOk(false);
            return;
        }
    }
    enableButtonOk(true);
}

void ModeDialog::slotButtonClicked(int button) {
    if(button == KDialog::Ok){
        if(!m_mode){
            m_mode = new Mode();
            m_remote->addMode(m_mode);
        }
        m_mode->setName(ui.leName->text());
        m_mode->setIconName(ui.ibIcon->icon());
        m_mode->setButton(ui.cbButtons->itemData(ui.cbButtons->currentIndex()).toString());
        if(ui.cbSetDefault->isChecked()){
            m_remote->setDefaultMode(m_mode);
        } else if(m_remote->defaultMode() == m_mode){
            m_remote->setDefaultMode(m_remote->masterMode());            
        }
        if(m_mode == m_remote->masterMode()){
            Remote::ModeChangeMode modeChangeMode = ui.gbModeCycle->isChecked() ? Remote::Cycle : Remote::Group;
            if(m_remote->modeChangeMode() != modeChangeMode){
                if(modeChangeMode == Remote::Cycle){
                    if(KMessageBox::questionYesNo(this, i18n("Using the cycle function will remove all duplicate button assignments from your modes. Are you sure that you would like to proceed?"), i18n("Change mode switch behaviour")) == KMessageBox::Yes){
                        m_remote->setModeChangeMode(modeChangeMode);
                    } else {
                        return;
                    }
                } else {
                    m_remote->setModeChangeMode(modeChangeMode);                  
                }
            }
            m_remote->setNextModeButton(ui.cbButtonForward->itemData(ui.cbButtonForward->currentIndex()).toString());
            m_remote->setPreviousModeButton(ui.cbButtonBackward->itemData(ui.cbButtonBackward->currentIndex()).toString());
        }
    }
    DBusInterface::getInstance()->considerButtonEvents(m_remote->name());
    KDialog::slotButtonClicked(button);
}

void ModeDialog::forwardButtonChanged(int index) {
    disconnect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged(int)));
    ui.cbButtonBackward->clear();
    
    ui.cbButtonBackward->addItem(i18n("No Button"), "");
    foreach(const QString &button, m_remote->availablePreviousModeButtons()){
        ui.cbButtonBackward->addItem(button, button);
    }

    ui.cbButtonBackward->setCurrentIndex(ui.cbButtonBackward->findData(m_remote->previousModeButton()));
    connect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged(int)));
}

void ModeDialog::backwardButtonChanged(int index) {
    disconnect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged(int)));
    ui.cbButtonForward->clear();
 
    ui.cbButtonForward->addItem(i18n("No Button"), "");                
    foreach(const QString &button, m_remote->availableNextModeButtons()){
        ui.cbButtonForward->addItem(button, button);
    }

    ui.cbButtonForward->setCurrentIndex(ui.cbButtonForward->findData(m_remote->nextModeButton()));
    connect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged(int)));
}

void ModeDialog::buttonPressed(const Solid::Control::RemoteControlButton& button) {
    kDebug() << "button event received";
    if(button.remoteName() == m_remote->name()) {
        if(m_mode != m_remote->masterMode()){
            ui.cbButtons->setCurrentIndex(ui.cbButtons->findText(button.name()));            
        }
    }
}

#include "modedialog.moc"
