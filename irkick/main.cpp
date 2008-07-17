/***************************************************************************
                          main.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@kde.org
***************************************************************************/

// This program is free software.

//#include <klocale.h>
#include <kcmdlineargs.h>
#include <kwinglobals.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kuniqueapplication.h>

#include "irkick.h"


extern "C" KDE_EXPORT int kdemain(int argc, char *argv[])
{

	//FIXME: What to do with VERSION???
	KAboutData *aboutData = new KAboutData("irkick", 0, ki18n("IRKick"), "x.x", ki18n("The KDE Infrared Remote Control Server"), KAboutData::License_GPL, ki18n("(c) 2003, Gav Wood"), ki18n("text"), "gav@kde.org");
	aboutData->addAuthor(ki18n("Gav Wood"), ki18n("Author"), "gav@kde.org", "http://www.indigoarchive.net/gav/");
	aboutData->addCredit(ki18n("Malte Starostik"), ki18n("Original LIRC interface code"), "malte.starostik@t-online.de");
	aboutData->addCredit(ki18n("Dirk Ziegelmeier"), ki18n("Ideas, concept code"), "dirk@ziegelmeier.net");
	aboutData->addCredit(ki18n("Zsolt Rizsanyi"), ki18n("Random patches"), "rizsanyi@myrealbox.com");
	aboutData->addCredit(ki18n("Antonio Larrosa Jiménez"), ki18n("Ideas"), "larrosa@kde.org");


	KCmdLineArgs::init( argc, argv, aboutData );
	KUniqueApplication::addCmdLineOptions();
	KUniqueApplication app;
	KGlobal::locale()->insertCatalog( "kdelirc" );
	app.disableSessionManagement();
	IRKick *theIRKick = new IRKick("/irkick");

	int ret = app.exec();
	delete theIRKick;
	return ret;
}
