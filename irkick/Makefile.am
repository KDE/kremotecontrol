kdedservicesdir = $(kde_servicesdir)/kded
kdedservices_DATA = irkick.desktop

lib_LTLIBRARIES = libkded_irkick.la

libkded_irkick_la_SOURCES = irkick.cpp irkick.skel irkick.stub klircclient.cpp
libkded_irkick_la_LDFLAGS = $(all_libraries) -module -avoid-version
libkded_irkick_la_LIBADD = $(LIB_KDECORE) ../kdelirc/libkdelirc.la

noinst_HEADERS = irkick.h klircclient.h irkick_stub.h

INCLUDES = $(all_includes) -I$(srcdir)/../kdelirc

KDE_ICON = AUTO
METASOURCES = AUTO
