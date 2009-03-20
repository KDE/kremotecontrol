/*************************************************************************
 * Copyright            : (C) 2002 by Gav Wood <gav@kde.org>             *
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
  * @author Gav Wood
  */

//                          main.cpp  -  Implementation of the main window

#include "irkick.h"

#include <kcmdlineargs.h>
#include <kuniqueapplication.h>

extern "C" KDE_EXPORT int kdemain(int argc, char *argv[])
{

    //FIXME: What to do with VERSION???
    KAboutData *aboutData = new KAboutData("irkick", 0, ki18n("IRKick"), "x.x", ki18n("The KDE Infrared Remote Control Server"), KAboutData::License_GPL, ki18n("(c) 2003, Gav Wood"), ki18n("text"), "gav@kde.org");
    aboutData->addAuthor(ki18n("Gav Wood"), ki18n("Author"), "gav@kde.org", "http://www.indigoarchive.net/gav/");
    aboutData->addCredit(ki18n("Malte Starostik"), ki18n("Original LIRC interface code"), "malte.starostik@t-online.de");
    aboutData->addCredit(ki18n("Dirk Ziegelmeier"), ki18n("Ideas, concept code"), "dirk@ziegelmeier.net");
    aboutData->addCredit(ki18n("Zsolt Rizsanyi"), ki18n("Random patches"), "rizsanyi@myrealbox.com");
    aboutData->addCredit(ki18n("Antonio Larrosa Jiménez"), ki18n("Ideas"), "larrosa@kde.org");
    aboutData->addCredit(ki18n("Michael Zanetti"), ki18n("KDE4-Port"), "michael_zanetti@gmx.net");
    aboutData->addCredit(ki18n("Frank Scheffold"), ki18n("KDE4-Port"), "fscheffold@googlemail.com");


    KCmdLineArgs::init(argc, argv, aboutData);
    KUniqueApplication::addCmdLineOptions();
    KUniqueApplication app;
    KGlobal::locale()->insertCatalog("kdelirc");
    app.disableSessionManagement();
    IRKick *theIRKick = new IRKick("/irkick");

    int ret = app.exec();
    delete theIRKick;
    return ret;
}
