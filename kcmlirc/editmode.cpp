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


#include "editmode.h"

EditMode::EditMode(QWidget *parent, Qt::WFlags fl) : KDialog(parent, fl)
{
    setupUi(this);
    connect(theName, SIGNAL(textChanged(QString)), this, SLOT(slotCheckText(QString)));
    connect(theOK, SIGNAL(clicked()), this, SLOT(accept()));
    connect(kPushButton8, SIGNAL(clicked()), this, SLOT(reject()));
    connect(checkBox, SIGNAL(toggled(bool)), theIcon, SLOT(setEnabled(bool)));


    theIcon->setIconType(KIconLoader::Panel, KIconLoader::Any);
    setButtons(KDialog::None);
}

EditMode::~EditMode()
{
}

void EditMode::slotClearIcon()
{
    theIcon->setIcon("irkick");
}

void EditMode::slotCheckText(const QString &newText)
{
    theOK->setEnabled(!newText.isEmpty());
}

#include "editmode.moc"
