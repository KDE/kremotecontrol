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

#include "action.h"

Action::Action(ActionType type, const QString &button): m_type(type), m_button(button) {
}

Action::Action(const Action& action): m_button(action.button()) {
    m_type = action.type();
}

Action::Action(ActionType type) {
    m_type = type;
}

Action::ActionType Action::type() const {
    return m_type;
}

QString Action::button() const {
    return m_button;
}

void Action::setButton(const QString& button) {
    m_button = button;
}

bool Action::repeat() const {
    return m_repeat;
}

void Action::setRepeat(bool repeat) {
    m_repeat = repeat;
}

bool Action::autostart() const {
    return m_autostart;
}

void Action::setAutostart(bool autostart) {
    m_autostart = autostart;
}

Action::ActionDestination Action::destination() const {
    return m_destination;
}

void Action::setDestination(Action::ActionDestination destination) {
    m_destination = destination;
}

void Action::saveToConfig(KConfigGroup& config) {
    config.writeEntry("Type", (int)m_type);
    config.writeEntry("Button", m_button);
    switch(m_destination){
        case Unique:
            config.writeEntry("Destination", "Unique");
            break;
        case Top:
            config.writeEntry("Destination", "Top");
            break;
        case Bottom:
            config.writeEntry("Destination", "Bottom");
            break;
        case None:
            config.writeEntry("Destination", "None");
            break;
        case All:
        default:
            config.writeEntry("Destination", "All");
            break;
    }
    config.writeEntry("Autostart", m_autostart);
    config.writeEntry("Repeat", m_repeat);
}

void Action::loadFromConfig(const KConfigGroup &config) {
    m_type = (ActionType)config.readEntry("Type").toInt();
    m_button = config.readEntry("Button");
    QString destination = config.readEntry("Destination");
    if(destination == QLatin1String( "Unique" )){
        m_destination = Unique;
    }
    else if(destination == QLatin1String( "Top" )){
        m_destination = Top;
    }
    else if(destination == QLatin1String( "Bottom" )){
        m_destination = Bottom;
    }
    else if(destination == QLatin1String( "None" )){
        m_destination = None;
    }
    else if(destination == QLatin1String( "All" )){
        m_destination = All;
    }
    m_autostart = config.readEntry("Autostart", QVariant(false)).toBool();
    m_repeat = config.readEntry("Repeat", QVariant(false)).toBool();
}
