/*************************************************************************
 * Copyright: (C) 2003 by Gav Wood <gav@kde.org>                         *
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
  * @author Gav Wood, Michael Zanetti
  */


#include "modedialog.h"
#include "mode.h"

ModeDialog::ModeDialog(Remote *remote, Mode *mode, QWidget *parent): KDialog(parent), m_remote(remote), m_mode(mode) {
    QWidget *widget = new QWidget(this);
    ui.setupUi(widget);
    setMainWidget(widget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);

    ui.cbButtons->addItem(i18n("No button"), "");
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(remote->name()).buttons()){
        ui.cbButtons->addItem(button.name(), button.name());
    }
    
    if(m_mode){
        ui.leName->setText(m_mode->name());
        ui.ibIcon->setIcon(m_mode->iconName());
        ui.cbButtons->setCurrentIndex(ui.cbButtons->findData(m_mode->button()));
    } else {
        ui.ibIcon->setIcon("infrared-remote");
    }
    
    connect(ui.leName, SIGNAL(textChanged(const QString&)), this, SLOT(checkForComplete()));
    checkForComplete();
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
    }
    KDialog::slotButtonClicked(button);
}

#include "modedialog.moc"
