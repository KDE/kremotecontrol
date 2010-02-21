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

#ifndef EDITPROFILEACTION_H
#define EDITPROFILEACTION_H

#include "ui_editdbusaction.h"

#include "profileaction.h"
#include "model.h"

class EditProfileAction: public QWidget{
    Q_OBJECT
    
    public:
    
        EditProfileAction(ProfileAction *action, QWidget* parent = 0, Qt::WFlags flags = 0);
        ~EditProfileAction();
        
        bool checkForComplete() const;
        void applyChanges();
        ProfileAction action() const;

    private:
        Ui::EditDBusAction ui;
        
        ProfileAction *m_action;
        ProfileModel *m_profileModel;
        ActionTemplateModel *m_templateModel;
        ArgumentsModel *m_argumentsModel;
        
    private slots:
        void refreshTemplates(const QModelIndex &index);
        void refreshArguments(const QModelIndex &index);
        
    signals:
        void formComplete(bool complete);

};

#endif // EDITDBUSACTION_H
