
include_directories( 
	${CMAKE_SOURCE_DIR}/kdelirc/irkick/
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/ )


########### next target ###############

set(kcm_kcmlirc_PART_SRCS ${libkdelirc_SRCS}
   kcmlirc.cpp 
   addaction.cpp 
   editmode.cpp 
   editaction.cpp 
   modeslist.cpp
   newmodedialog.cpp )

#kde4_add_dcop_stubs(kcm_kcmlirc_PART_SRCS ${CMAKE_SOURCE_DIR}/kdelirc/irkick/irkick.h)

#kde4_add_dcop_skels(kcm_kcmlirc_PART_SRCS kcmlirc.h ${CMAKE_SOURCE_DIR}/kdelirc/irkick/irkick.h )

kde4_add_ui_files(kcm_kcmlirc_PART_SRCS newmode.ui)

kde4_add_ui3_files(kcm_kcmlirc_PART_SRCS 
   selectprofile.ui 
   kcmlircbase.ui 
   addactionbase.ui 
   editactionbase.ui 
   editmodebase.ui )

kde4_add_plugin(kcm_kcmlirc ${kcm_kcmlirc_PART_SRCS})


target_link_libraries(kcm_kcmlirc  ${KDE4_KIO_LIBS} )

install(TARGETS kcm_kcmlirc  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcmlirc.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})




#original Makefile.am contents follow:

#INCLUDES    = -I../irkick -I$(srcdir)/../irkick -I$(srcdir)/../kdelirc $(all_includes)
#METASOURCES = AUTO
#
## Install this plugin in the KDE modules directory
#kde_module_LTLIBRARIES = kcm_kcmlirc.la
#
#noinst_HEADERS = kcmlirc.h addaction.h addactionbase.h
#kcm_kcmlirc_la_SOURCES = selectprofile.ui newmode.ui kcmlircbase.ui kcmlirc.cpp addactionbase.ui addaction.cpp kcmlirc.skel editactionbase.ui editmodebase.ui editmode.cpp editaction.cpp modeslist.cpp
#kcm_kcmlirc_la_LIBADD = $(LIB_KDEUI) $(LIB_KFILE) ../kdelirc/libkdelirc.la
#kcm_kcmlirc_la_LDFLAGS = -module -avoid-version $(all_libraries) -no-undefined
#
#xdg_apps_DATA = kcmlirc.desktop
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kcmlirc.pot
