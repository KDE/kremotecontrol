/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#ifndef MODE_H
#define MODE_H

#include "kremotecontrol_export.h"

#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QVector>

class Action;

class KREMOTECONTROL_EXPORT Mode
{
    public:
        explicit Mode(const QString &name = QString(), const QString &iconName = QLatin1String( "infrared-remote" ));
        ~Mode();

        QString name() const;
        QString iconName() const;
        QString button() const;


        void setName(const QString &name);
        void setIconName(const QString &iconName);
        void setButton(const QString &button);

        /**
          * Add the given Action to the Mode. The mode takes ownership of the action. Don't delete it!
          */
        void addAction(Action *action);

        /**
          * Remove the given action from this mode and delete it. Don't try to access the action after this call!
          */
        void removeAction(Action *action);
        void moveActionUp(Action *action);
        void moveActionDown(Action *action);

        QVector<Action*> actions() const;
        QVector<Action*> actionsForButton(const QString& button) const;

        bool operator==(const Mode &mode) const;
        bool doAfter();
    protected:
        QString m_name;
        QString m_iconName;
        QString m_button;
        QVector<Action*> m_actionList;
        bool m_doAfter;
};

Q_DECLARE_METATYPE(Mode*)

#endif
