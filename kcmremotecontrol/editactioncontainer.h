/*
    Copyright (C) 2010 Michael Zanetti <michael_zanetti@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef ACTIONCONTAINER_H
#define ACTIONCONTAINER_H

#include "ui_editactioncontainer.h"

#include <kdialog.h>
#include <action.h>


class EditActionContainer : public KDialog {
    Q_OBJECT
    public:
        EditActionContainer(Action *action, const QString &remote, QWidget* parent = 0, Qt::WFlags flags = 0);
        
    private:
        Ui::ActionContainer ui;
        Action *m_action;
        QWidget *m_innerWidget;
        QString m_remote;
        
    protected Q_SLOTS:
        void checkForComplete();
        virtual void slotButtonClicked(int button);
        void buttonPressed(const Solid::Control::RemoteControlButton &button);
};

#endif // ACTIONCONTAINER_H
