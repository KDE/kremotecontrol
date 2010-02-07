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

#include <QString>
#include <qmetatype.h>

class Action;

class KREMOTECONTROL_EXPORT Mode
{
  public:
    Mode(const QString &name = QString(), const QString &iconName = "infrared-remote");

    QString name() const;
    QString iconName() const;
    QString button() const;
    
    
    void setName(const QString &name);
    void setIconName(const QString &iconName);
    void setButton(const QString &button);

    void addAction(Action *action);
    void removeAction(Action *action);
    
    QList<Action*> actions() const;
    QList<Action*> actionsForButton(const QString& button) const;

    bool operator==(const Mode &mode) const;
    bool doAfter();
  protected:
    QString m_name;
    QString m_iconName;
    QString m_button;
    QList<Action*> m_actionList;
    bool m_doAfter;
};

Q_DECLARE_METATYPE(Mode*)

#endif
