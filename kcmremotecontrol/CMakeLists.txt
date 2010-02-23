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

set(kcm_remotecontrol_PART_SRCS
   kcmremotecontrol.cpp
   addaction.cpp
   editactioncontainer.cpp
   editdbusaction.cpp
   editprofileaction.cpp
   model.cpp
   modedialog.cpp
   selectprofile.cpp
   )

qt4_add_dbus_adaptor(kcm_remotecontrol_PART_SRCS org.kde.kcmremotecontrol.xml kcmremotecontrol.h KCMRemoteControl)

kde4_add_ui_files(kcm_remotecontrol_PART_SRCS modedialog.ui addaction.ui editactioncontainer.ui editdbusaction.ui configurationwidget.ui )

kde4_add_plugin(kcm_remotecontrol ${kcm_remotecontrol_PART_SRCS})

target_link_libraries(kcm_remotecontrol ${KDE4_KIO_LIBS} ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} libkremotecontrol)

install(TARGETS kcm_remotecontrol DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcm_remotecontrol.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

