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

#include "mode.h"
#include "action.h"

#include <KDebug>

Mode::Mode(const QString &name, const QString &iconName): m_name(name), m_iconName(iconName) {
}

Mode::~Mode() {
    while(!m_actionList.isEmpty()){
        Action *action = m_actionList.first();
        m_actionList.remove(0);
        delete action;
    }
}

QString Mode::name() const {
    return m_name;
}

QString Mode::iconName() const {
    return m_iconName;
}

QString Mode::button() const {
    return m_button;
}

void Mode::setName(const QString& name) {
    m_name = name;
}

void Mode::setIconName(const QString& iconName) {
    m_iconName = iconName;
}

void Mode::setButton(const QString& button) {
    m_button = button;
}

bool Mode::operator==(const Mode& mode) const {
    return m_name == mode.name() && m_iconName == mode.iconName() && m_button == mode.button();
}

void Mode::addAction(Action* action) {
    m_actionList.append(action);
}

void Mode::removeAction(Action* action) {
    m_actionList.remove(m_actionList.indexOf(action));
    delete action;
}

void Mode::moveActionUp(Action* action) {
    int oldIndex = m_actionList.indexOf(action);
    if(oldIndex > 0) {
        m_actionList.remove(oldIndex);
        m_actionList.insert(oldIndex - 1, action);
    }
}

void Mode::moveActionDown(Action* action) {
    int oldIndex = m_actionList.indexOf(action);
    if(oldIndex < (m_actionList.count() - 1)) {
        m_actionList.remove(oldIndex);
        m_actionList.insert(oldIndex + 1, action);
    }
}

QVector<Action* > Mode::actions() const {
    return m_actionList;
}

QVector<Action*> Mode::actionsForButton(const QString &button) const {
    QVector<Action*> retList;
    foreach(Action *action, m_actionList){
        kDebug() << "checking action:" << action->name() << action->button();
        if(action->button() == button){
            kDebug() << "Found action for button:" << action->name();
            retList.append(action);
        }
    }
    return retList;
}

bool Mode::doAfter(){
    return m_doAfter;
}
