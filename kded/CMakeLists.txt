
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
   ${KDE4WORKSPACE_INCLUDE_DIR}
   )

set(kded_kremotecontroldaemon_SRCS
   kremotecontroldaemon.cpp
   )

set(krcd_xml org.kde.krcd.xml)


qt4_add_dbus_adaptor( kded_kremotecontroldaemon_SRCS org.kde.krcd.xml kremotecontroldaemon.h KRemoteControlDaemon)


kde4_add_plugin(kded_kremotecontroldaemon ${kded_kremotecontroldaemon_SRCS})
target_link_libraries(kded_kremotecontroldaemon
   ${KDE4_KDECORE_LIBS}
   ${KDE4_KDEUI_LIBS}
   libkremotecontrol
)

install(TARGETS kded_kremotecontroldaemon DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kremotecontroldaemon.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES kremotecontroldaemon.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kremotecontroldaemon )
