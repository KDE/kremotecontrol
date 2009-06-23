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


#ifndef NEWMODEDIALOG_H
#define NEWMODEDIALOG_H

#include "ui_newmode.h"
#include "mode.h"
#include "modes.h"
#include <QStringList>

class NewModeBaseWidget : public QWidget, public Ui::NewMode
{
public:
    NewModeBaseWidget(QWidget *parent = 0) : QWidget(parent)
    {
        setupUi(this);
    }
};


class NewModeDialog : public KDialog
{
    Q_OBJECT

public:
    explicit NewModeDialog(const Modes &allModes, QWidget *parent = 0, const bool &modal = false);
    ~NewModeDialog();

private:
    NewModeBaseWidget *newModeBaseWidget;
    Modes allModes;

private slots:
    void checkForComplete();

public:
    Mode getMode();
    bool isDefaultMode() const;
};

#endif /* NEWMODEDIALOG_H */
