/***************************************************************************
                          main.cpp  -  Implementation of the main window
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@kde.org
***************************************************************************/

// This program is free software.

#include <klocale.h>
#include <kcmdlineargs.h>
#include <kwin.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kuniqueapplication.h>

#include "irkick.h"

extern "C" KDE_EXPORT int kdemain(int argc, char *argv[])
{
	KAboutData *aboutData = new KAboutData("irkick", I18N_NOOP("IRKick"), VERSION, I18N_NOOP("The KDE Infrared Remote Control Server"), KAboutData::License_GPL, "(c) 2003, Gav Wood", 0, 0, "gav@kde.org");
	aboutData->addAuthor("Gav Wood", I18N_NOOP("Author"), "gav@kde.org", "http://www.indigoarchive.net/gav/");
	aboutData->addCredit("Malte Starostik", I18N_NOOP("Original LIRC interface code"), "malte.starostik@t-online.de");
	aboutData->addCredit("Dirk Ziegelmeier", I18N_NOOP("Ideas, concept code"), "dirk@ziegelmeier.net");
	aboutData->addCredit("Zsolt Rizsanyi", I18N_NOOP("Random patches"), "rizsanyi@myrealbox.com");
	aboutData->addCredit("Antonio Larrosa Jiménez", I18N_NOOP("Ideas"), "larrosa@kde.org");

	KCmdLineArgs::init( argc, argv, aboutData );
	KUniqueApplication::addCmdLineOptions();
	KUniqueApplication app;
	KGlobal::locale()->insertCatalogue( "kdelirc" );
	app.disableSessionManagement();
	IRKick *theIRKick = new IRKick("IRKick");

	int ret = app.exec();
	delete theIRKick;
	return ret;
}
