INCLUDES    = $(all_includes) -I../irkick
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kcm_kcmlirc.la

noinst_HEADERS = kcmlirc.h addaction.h addactionbase.h
kcm_kcmlirc_la_SOURCES = kcmlircbase.ui kcmlirc.cpp addactionbase.ui addaction.cpp kcmlirc.skel
kcm_kcmlirc_la_LIBADD = $(LIB_KDEUI)
kcm_kcmlirc_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined

kcmlirc_DATA = kcmlirc.desktop
kcmlircdir = $(kde_appsdir)/Settings/Peripherals

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcmlirc.pot
