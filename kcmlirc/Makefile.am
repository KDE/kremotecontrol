INCLUDES    = $(all_includes) -I$(srcdir)/../irkick -I../irkick -I$(srcdir)/../kdelirc
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kcm_kcmlirc.la

noinst_HEADERS = kcmlirc.h addaction.h addactionbase.h
kcm_kcmlirc_la_SOURCES = newmode.ui kcmlircbase.ui kcmlirc.cpp addactionbase.ui addaction.cpp kcmlirc.skel
kcm_kcmlirc_la_LIBADD = $(LIB_KDEUI) ../kdelirc/libkdelirc.la
kcm_kcmlirc_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined

kcmlirc_DATA = kcmlirc.desktop
kcmlircdir = $(kde_appsdir)/Settings/Peripherals

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmlirc.pot
