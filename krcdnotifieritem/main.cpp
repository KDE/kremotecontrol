/*
  Copyright (c) 2010 Michael Zanetti <michael_zanetti@gmx.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

#include "krcdnotifieritem.h"

#include <kdeutils-version.h>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <kdebug.h>

#include <QDBusConnection>
#include <kconfiggroup.h>

class KrcdNotifierApp : public KUniqueApplication
{
    public:
      KrcdNotifierApp() : m_item( 0 ) {
          // ensure the Quit dialog's Cancel reponse does not close the app
          setQuitOnLastWindowClosed( false );
          QDBusConnection::sessionBus().connect("org.kde.kded", "/modules/kremotecontrol", "org.kde.krcd", "unloadTray",  this, SLOT(quit()));
      }

      int newInstance() {
          // Check if we already have a running alarm daemon widget
          if(m_item) {
              return 0;
          }

          m_item = new KrcdNotifierItem();

          return 0;
      }

    private:
        KrcdNotifierItem *m_item;
        
    private slots:
        void configChanged() {
            KConfig config("kremotecontrolrc");
            KConfigGroup globalGroup(&config, "Global");
            if(!globalGroup.readEntry("ShowTrayIcon", true)){
                quit();
            }
        }
};

int main( int argc, char **argv ) {

    KAboutData aboutData("irkick", 0, ki18n("Remote Control"), KDEUTILS_VERSION_STRING, ki18n("The KDE Infrared Remote Control Server"), KAboutData::License_GPL, ki18n("(c) 2003, Gav Wood"), ki18n("Control your desktop with your remote."), "http://utils.kde.org/projects/kdelirc");
    aboutData.addAuthor(ki18n("Michael Zanetti"), ki18n("Maintainer"), "michael_zanetti@gmx.net");
    aboutData.addAuthor(ki18n("Frank Scheffold"), ki18n("KDeveloper"), "fscheffold@googlemail.com");
    aboutData.addCredit(ki18n("Gav Wood"), ki18n("Original KDELirc Author"), "gav@kde.org");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions( options );
    KUniqueApplication::addCmdLineOptions();

    if(!KrcdNotifierApp::start()) {
        return 0;
    }

    KrcdNotifierApp app;
    app.disableSessionManagement();

    return app.exec();
    kDebug() << "exiting...";
}
