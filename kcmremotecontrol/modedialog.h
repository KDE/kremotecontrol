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


#ifndef MODEDIALOG_H
#define MODEDIALOG_H

#include "KComboBox"

class ButtonComboBox: public KComboBox
{
    Q_OBJECT
public:
    ButtonComboBox(QWidget* parent = 0);
    void addButtons(const QStringList &buttonList);

public slots:
    void hideButton(const QString &button);

private:
    int m_hiddenIndex;
    QString m_hiddedButton;
};

#include "ui_modedialog.h"
#include "remote.h"

#include "kcombobox.h"

class ModeDialog : public KDialog
{
    Q_OBJECT

public:
  
    /**
      * @brief Creates a ModeDialog for the given Remote.
      * @param Remote The Remote that owns to Mode to edit or create
      * @param Mode The Mode to be edited. If 0 a new one will be created and inserted into the Remote
      */
    explicit ModeDialog(Remote *remote, Mode *mode = 0, QWidget *parent = 0);
    ~ModeDialog();

private:
    Ui::ModeDialog ui;
    Remote *m_remote;
    Mode *m_mode;

private slots:
    void checkForComplete();
    void slotButtonClicked(int button);
    void forwardButtonChanged();
    void backwardButtonChanged();
    void buttonPressed(const Solid::Control::RemoteControlButton &button);
    void modeHandlerChanged();
};



#endif /* NEWMODEDIALOG_H */
