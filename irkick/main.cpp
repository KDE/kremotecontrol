/***************************************************************************
                          main.cpp  -  description
                             -------------------
    copyright            : (C) 2002 by Gav Wood
    email                : gav@indigoarchive.net
***************************************************************************/

// This program is free software.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <kconfig.h>
#include <ksimpleconfig.h>
#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "irkick.h"

static const char *description = I18N_NOOP("IRKick");

static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
};

KSimpleConfig *configData;
KAboutData *aboutData;

void initConfig()
{
	// Write application defaults in here.
}

int main(int argc, char *argv[])
{
	aboutData = new KAboutData("irkick", I18N_NOOP("IRKick"), VERSION, description, KAboutData::License_GPL, "(c) 2002, Gav Wood", 0, 0, "gav@indigoarchive.net");
	aboutData->addAuthor("Gav Wood", 0, "gav@indigoarchive.net");

	KCmdLineArgs::init(argc, argv, aboutData);
	KCmdLineArgs::addCmdLineOptions(options);

	KUniqueApplication a;
	KSimpleConfig theRealConfig("irkick");
	configData = &theRealConfig;
	initConfig();
	IRKick *theIRKick = new IRKick();
	a.setMainWidget(theIRKick);
	theIRKick->show();

	return a.exec();
}
