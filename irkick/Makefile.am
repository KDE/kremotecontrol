SUBDIRS = icons

bin_PROGRAMS =
lib_LTLIBRARIES =
noinst_LTLIBRARIES = libirkick_common.la
kdeinit_LTLIBRARIES = irkick.la

CLEANFILES = dummy.cpp

libirkick_common_la_LIBADD = $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KSYCOCA) ../kdelirc/libkdelirc.la
libirkick_common_la_SOURCES = main.cpp irkick.cpp irkick.skel irkick.stub klircclient.cpp

libirkick_common_la_LDFLAGS = $(all_libraries)

irkick_la_LIBADD = libirkick_common.la
irkick_la_SOURCES = dummy.cpp
irkick_la_LDFLAGS = $(all_libraries) -module -avoid-version

xdg_apps_DATA = irkick.desktop

autostartdir = $(datadir)/autostart
autostart_DATA = irkick.desktop

messages:
	$(XGETTEXT) *.cpp -o $(podir)/irkick.pot

dummy.cpp:
	echo > dummy.cpp

#kdedservicesdir = $(kde_servicesdir)/kded
#kdedservices_DATA = irkick.desktop
#lib_LTLIBRARIES = libkded_irkick.la
#libkded_irkick_la_SOURCES = irkick.cpp irkick.skel irkick.stub klircclient.cpp
#libkded_irkick_la_LDFLAGS = $(all_libraries) -module -avoid-version
#libkded_irkick_la_LIBADD = $(LIB_KDECORE) ../kdelirc/libkdelirc.la

noinst_HEADERS = irkick.h klircclient.h irkick_stub.h

INCLUDES = -I$(srcdir)/../kdelirc $(all_includes)

KDE_ICON = AUTO
METASOURCES = AUTO
