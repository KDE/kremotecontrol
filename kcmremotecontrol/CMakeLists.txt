add_subdirectory( icons )

include_directories( 
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
   ${CMAKE_CURRENT_BINARY_DIR}/../libkremotecontrol
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
)

########### next target ###############

set(kcm_remotecontrol_PART_SRCS
   kcmremotecontrol.cpp
   addaction.cpp
   editactioncontainer.cpp
   editdbusaction.cpp
   editprofileaction.cpp
   editkeypressaction.cpp
   model.cpp
   modedialog.cpp
   selectprofile.cpp
   )

kde4_add_ui_files(kcm_remotecontrol_PART_SRCS
   modedialog.ui
   addaction.ui
   editactioncontainer.ui
   editdbusaction.ui
   editkeypressaction.ui
   configurationwidget.ui
)

kde4_add_plugin(kcm_remotecontrol ${kcm_remotecontrol_PART_SRCS})

target_link_libraries(kcm_remotecontrol ${KDE4_KIO_LIBS} libkremotecontrol)

install(TARGETS kcm_remotecontrol DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcm_remotecontrol.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

