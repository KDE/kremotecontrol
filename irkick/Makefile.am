kdedservicesdir = $(kde_servicesdir)/kded
kdedservices_DATA = irkick.desktop

lib_LTLIBRARIES = libkded_irkick.la

libkded_irkick_la_SOURCES = irkick.cpp irkick.skel irkick.stub klircclient.cpp
libkded_irkick_la_LDFLAGS = $(all_libraries) -module -avoid-version
libkded_irkick_la_LIBADD = $(LIB_KDECORE) ../kdelirc/libkdelirc.la

## INCLUDES were found outside kdevelop specific part

noinst_HEADERS = irkick.h klircclient.h irkick_stub.h

EXTRA_DIST = main.cpp irkick.desktop hi32-app-irkick.png hi22-app-irkick.png hi16-app-irkick.png lo32-app-irkick.png lo16-app-irkick.png


KDE_ICON = AUTO

####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_irkicksdir    Where irkicks for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

# set the include path for X, qt and KDE
INCLUDES = $(all_includes) -I$(srcdir)/../kdelirc

METASOURCES = AUTO

# the library search path.

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDE´s XML GUI builing
#rcdir = $(kde_datadir)/irkick
#rc_DATA = irkickui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of

# messages:

messages:
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/irkick.pot; \
	fi

