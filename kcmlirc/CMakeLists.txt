
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




