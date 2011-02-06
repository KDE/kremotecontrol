/*
    Copyright (C) 2010  Michael Zanetti <mzanetti@kde.org>

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

#include "keypressaction.h"

#include <KDebug>
#include <KLocale>

KeypressAction::KeypressAction(const QString &button): Action(Action::KeypressAction, button) {

}

KeypressAction::KeypressAction(): Action(Action::KeypressAction) {
}

QString KeypressAction::name() const {
    return i18n("Workspace");
}

QString KeypressAction::description() const {
    QString ret = i18n("Keypress:");
    ret.append(' ');
    for(int i = 0; i < m_keySequenceList.count(); ++i) {
        if(i != 0) {
            ret.append(", ");
        }
        ret.append(m_keySequenceList.at(i).toString(QKeySequence::NativeText));
    }

    return ret;
}

void KeypressAction::saveToConfig(KConfigGroup& config) {
    Action::saveToConfig(config);
    for(int i = 0; i < m_keySequenceList.count(); ++i) {
        config.writeEntry("KeySequence" + QString::number(i), m_keySequenceList.at(i).toString(QKeySequence::PortableText));
    }

}

void KeypressAction::loadFromConfig(const KConfigGroup& config) {
    Action::loadFromConfig(config);
    int i = 0;
    while(!config.readEntry("KeySequence" + QString::number(i)).isEmpty()) {
        m_keySequenceList.append(QKeySequence(config.readEntry("KeySequence" + QString::number(i++)),QKeySequence::PortableText));
    }
}

Action* KeypressAction::clone() const {
    KeypressAction *action = new KeypressAction();
    action->setKeySequenceList(m_keySequenceList);
    action->setRepeat(m_repeat);
    return action;
}

void KeypressAction::setKeySequenceList(const QList<QKeySequence> &keySequenceList) {
    m_keySequenceList = keySequenceList;
}

QList<QKeySequence> KeypressAction::keySequenceList() const {
    return m_keySequenceList;
}
