/*************************************************************************
 * Copyright (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>       *
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
#include "model.h"

ModeDialog::ModeDialog(Remote *remote, Mode *mode, QWidget *parent): KDialog(parent), m_remote(remote), m_mode(mode) {
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);

    connect(ui.leName, SIGNAL(textChanged(QString)), this, SLOT(checkForComplete()));
    connect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged()));
    connect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged()));
    connect(ui.gbModeCycle, SIGNAL(clicked(bool)), this, SLOT(modeHandlerChanged()));

    ui.cbButtons->addItem(i18n("No button"), QString());
    foreach(const QString &button, remote->availableModeSwitchButtons(mode)){
        ui.cbButtons->addItem(RemoteControlButton(remote->name(), button).description(), button);
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
            ui.cbButtonBackward->addButtons(remote->availableModeCycleButtons());
            ui.cbButtonForward->addButtons(remote->availableModeCycleButtons());

            // Load currently configured buttons
            ui.cbButtonBackward->setCurrentIndex(ui.cbButtonBackward->findData(remote->previousModeButton()));
            ui.cbButtonForward->setCurrentIndex(ui.cbButtonForward->findData(remote->nextModeButton()));

            if(remote->modeChangeMode() == Remote::Cycle){
                ui.gbModeCycle->setChecked(true);
            } else {
                ui.gbModeCycle->setChecked(false);
            }
        } else {
            ui.gbModeCycle->setVisible(false);
        }
    } else {
        ui.ibIcon->setIcon(QLatin1String( "infrared-remote" ));
        ui.gbModeCycle->setVisible(false);
    }

    checkForComplete();

    // Pause remote to make use of button presses here
    DBusInterface::getInstance()->ignoreButtonEvents(remote->name());
    connect(new RemoteControl(remote->name()), SIGNAL(buttonPressed(RemoteControlButton)), SLOT(buttonPressed(RemoteControlButton)));
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
            m_remote->setNextModeButton(ui.cbButtonForward->itemData(ui.cbButtonForward->currentIndex()).toString());
            m_remote->setPreviousModeButton(ui.cbButtonBackward->itemData(ui.cbButtonBackward->currentIndex()).toString());
        }
    }
    DBusInterface::getInstance()->considerButtonEvents(m_remote->name());
    KDialog::slotButtonClicked(button);
}

void ModeDialog::forwardButtonChanged() {
    disconnect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged()));
    QString forwardButton = ui.cbButtonForward->itemData(ui.cbButtonForward->currentIndex()).toString();
    ui.cbButtonBackward->hideButton(forwardButton);
    connect(ui.cbButtonBackward, SIGNAL(currentIndexChanged(int)), this, SLOT(backwardButtonChanged()));
}

void ModeDialog::backwardButtonChanged() {
    disconnect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged()));
    QString backwardButton = ui.cbButtonBackward->itemData(ui.cbButtonBackward->currentIndex()).toString();
    ui.cbButtonForward->hideButton(backwardButton);
    connect(ui.cbButtonForward, SIGNAL(currentIndexChanged(int)), this, SLOT(forwardButtonChanged()));
}

void ModeDialog::buttonPressed(const RemoteControlButton& button) {
    kDebug() << "button event received";
    if(button.remoteName() == m_remote->name()) {
        if(m_mode != m_remote->masterMode()){
            ui.cbButtons->setCurrentIndex(ui.cbButtons->findText(button.name()));
        }
    }
}

void ModeDialog::modeHandlerChanged() {
    Remote::ModeChangeMode modeChangeMode = ui.gbModeCycle->isChecked() ? Remote::Cycle : Remote::Group;
    if(m_remote->modeChangeMode() != modeChangeMode){
        if(modeChangeMode == Remote::Cycle){
            if(KMessageBox::questionYesNo(this, i18n("Using the cycle function will remove all duplicate button assignments from your modes. Are you sure that you would like to proceed?"), i18n("Change mode switch behavior")) == KMessageBox::Yes){
                m_remote->setModeChangeMode(modeChangeMode);
            } else {
                ui.gbModeCycle->setChecked(false);
            }
        } else {
            m_remote->setModeChangeMode(modeChangeMode);
        }
    }
}

ButtonComboBox::ButtonComboBox(QWidget* parent): KComboBox(parent) {
    addItem(i18n("No Button"), QString());
}

void ButtonComboBox::addButtons(const QStringList &buttonList){
    kDebug() << "adding buttons";
    foreach(const QString &button, buttonList){
        kDebug() << "adding button" << button;
        addItem(button, button);
    }
}

void ButtonComboBox::hideButton(const QString& button) {
    kDebug() << "hiding button" << button;
    if(!m_hiddenButton.isEmpty()){
        kDebug() << "inserting old hidden button" << m_hiddenButton;
        insertItem(m_hiddenIndex, m_hiddenButton);
    }
    if(!button.isEmpty()){
        m_hiddenIndex = findData(button);
        m_hiddenButton = itemData(m_hiddenIndex).toString();
        removeItem(m_hiddenIndex);
    }
}

#include "modedialog.moc"
