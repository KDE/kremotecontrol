/*
    Copyright (C) 2010 Michael Zanetti <mzanetti@kde.org>

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

#include "keypressactionexecutor.h"
#include "keypressaction.h"

#include <KDebug>
#include <KWindowSystem>

#include <netwm_def.h>

#include <QtGui/QX11Info>

#include <X11/keysym.h>
#include <X11/extensions/XTest.h>


void KeypressActionExecutor::execute(Action* action) {
    KeypressAction *keypressAction = dynamic_cast<KeypressAction*>(action);
    if(keypressAction){
        foreach(const QKeySequence &key, keypressAction->keySequenceList()) {
            kDebug() << "executing keypressaction:" << key;
            executeKeypress(key);
        }
    } else {
        kDebug() << "KeypressActionExecutor: action does not appear to be a KeypressAction";
    }
}

bool KeypressActionExecutor::executeKeypress(const QKeySequence &key)
{

    QList<KeyCode> modList = extractMods(key);

    bool ret = true;
    foreach(KeyCode modcode, modList) {
        kDebug() << "pressing modifier:" << modcode;
        ret = ret && XTestFakeKeyEvent(QX11Info::display(), modcode, true, 0);
    }

    int keySym;
    KKeyServer::keyQtToSymX(key[0], &keySym);
    kDebug() << "keysym" << keySym;
    KeyCode keycode = XKeysymToKeycode(QX11Info::display(), keySym);

    kDebug() << "pressing key:" << keycode;
    ret = ret && XTestFakeKeyEvent(QX11Info::display(), keycode, true, 0);
    kDebug() << "releasing key:" << keycode;
    ret = ret && XTestFakeKeyEvent(QX11Info::display(), keycode, false, 0);


    foreach(KeyCode modcode, modList) {
        kDebug() << "releasing modifier:" << modcode;
        ret = ret && XTestFakeKeyEvent(QX11Info::display(), modcode, false, 0);
    }

    // Qt's autorepeat compression is broken and can create "aab" from "aba"
    // XSync() should create delay longer than Qt's max autorepeat interval
    XSync( QX11Info::display(), False );
    return ret;
}

QList<KeyCode> KeypressActionExecutor::extractMods(const QKeySequence &keySequence)
{
    QList<KeyCode> ret;

    if(keySequence.toString(QKeySequence::PortableText).contains("Shift+")) {
        kDebug() << "sequence matches Shift!";
        ret.append(XKeysymToKeycode(QX11Info::display(), XK_Shift_L));
    }
    if(keySequence.toString(QKeySequence::PortableText).contains("Alt+")) {
        kDebug() << "sequence matches Alt!";
        ret.append(XKeysymToKeycode(QX11Info::display(), XK_Alt_L));
    }
    if(keySequence.toString(QKeySequence::PortableText).contains("Ctrl+")) {
        kDebug() << "sequence matches Ctrl!";
        ret.append(XKeysymToKeycode(QX11Info::display(), XK_Control_L));
    }
    if(keySequence.toString(QKeySequence::PortableText).contains("Meta+")) {
        kDebug() << "sequence matches Meta!";
        ret.append(XKeysymToKeycode(QX11Info::display(), XK_Super_L));
    }
    return ret;

}


