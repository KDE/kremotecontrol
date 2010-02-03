add_subdirectory( icons )

include_directories( 
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
   ${CMAKE_CURRENT_BINARY_DIR}/../libkremotecontrol
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
   ${KDE4WORKSPACE_INCLUDE_DIR} )

########### next target ###############

set(kcm_lirc_PART_SRCS 
#    selectprofile.cpp
   kcmlirc.cpp
   addaction.cpp
   editactioncontainer.cpp
   editdbusaction.cpp
   model.cpp
   newmodedialog.cpp
   )

qt4_add_dbus_adaptor(kcm_lirc_PART_SRCS org.kde.kcmlirc.xml kcmlirc.h KCMLirc)

kde4_add_ui_files(kcm_lirc_PART_SRCS newmode.ui addaction.ui editactioncontainer.ui editdbusaction.ui configurationwidget.ui )

kde4_add_plugin(kcm_lirc ${kcm_lirc_PART_SRCS})

#target_link_libraries(kcm_lirc ${KDE4_KIO_LIBS} ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} kdelirc_shared)
target_link_libraries(kcm_lirc ${KDE4_KIO_LIBS} ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} libkremotecontrol)

install(TARGETS kcm_lirc DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcm_lirc.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

