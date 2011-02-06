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

#include "dbusaction.h"

DBusAction::DBusAction(const QString &button): Action(Action::DBusAction, button) {

}

DBusAction::DBusAction(): Action(Action::DBusAction) {
}

QString DBusAction::application() const {
    return m_application;
}

void DBusAction::setApplication(const QString& application) {
    m_application = application;
}

QString DBusAction::node() const {
    return m_node;
}

void DBusAction::setNode(const QString& node) {
    m_node = node;
}

Prototype DBusAction::function() const {
    return m_function;
}

void DBusAction::setFunction(const Prototype& function) {
    m_function = function;
}

QString DBusAction::name() const {
    return m_application + QLatin1Char( ':' ) + m_node;
}

QString DBusAction::description() const {
    return m_function.name();
}

void DBusAction::saveToConfig(KConfigGroup& config) {
    Action::saveToConfig(config);
    config.writeEntry("Application", m_application);
    config.writeEntry("Node", m_node);
    config.writeEntry("Function", m_function.name());
    int i = 0; // GroupID for arguments
    foreach(const Argument &arg, m_function.args()){
        KConfigGroup argumentGroup(&config, QLatin1String( "Argument" ) + QString::number(i));
        argumentGroup.writeEntry("Description", arg.description());
        argumentGroup.writeEntry("Value", arg.value());
        argumentGroup.writeEntry("Type", arg.value().typeName());
        i++;
    }
}

void DBusAction::loadFromConfig(const KConfigGroup& config) {
    Action::loadFromConfig(config);
    m_application = config.readEntry("Application");
    m_node = config.readEntry("Node");
    m_function = Prototype(config.readEntry("Function"));
    QStringList argGroupList = config.groupList();
    argGroupList.sort();
    QList<Argument> argList;
    foreach(const QString &argID, argGroupList){
        KConfigGroup argumentGroup = KConfigGroup(&config, argID);
        QVariant argValue = QVariant(QVariant::nameToType(argumentGroup.readEntry("Type").toLocal8Bit()));
        argValue = argumentGroup.readEntry("Value", argValue);
        QString description = argumentGroup.readEntry("Description");
        Argument arg(argValue, description);
        argList.append(arg);
    }
    m_function.setArgs(argList);
}

Action* DBusAction::clone() const {
    DBusAction *action = new DBusAction();
    action->setApplication(m_application);
    action->setAutostart(m_autostart);
    action->setDestination(m_destination);
    action->setFunction(m_function);
    action->setNode(m_node);
    action->setRepeat(m_repeat);
    action->setButton(m_button);
    return action;
}
