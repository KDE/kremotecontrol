INCLUDES    = -I../irkick -I$(srcdir)/../irkick -I$(srcdir)/../kdelirc $(all_includes)
METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kcm_kcmlirc.la

noinst_HEADERS = kcmlirc.h addaction.h addactionbase.h
kcm_kcmlirc_la_SOURCES = selectprofile.ui newmode.ui kcmlircbase.ui kcmlirc.cpp addactionbase.ui addaction.cpp kcmlirc.skel editactionbase.ui editmodebase.ui editmode.cpp editaction.cpp modeslist.cpp
kcm_kcmlirc_la_LIBADD = $(LIB_KDEUI) $(LIB_KFILE) ../kdelirc/libkdelirc.la
kcm_kcmlirc_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined

xdg_apps_DATA = kcmlirc.desktop

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kcmlirc.pot
