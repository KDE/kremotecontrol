
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
   )

set(kded_kremotecontrol_SRCS
   kremotecontroldaemon.cpp
   )

set(krcd_xml org.kde.krcd.xml)


qt4_add_dbus_adaptor( kded_kremotecontrol_SRCS org.kde.krcd.xml  kremotecontroldaemon.h KRemoteControlDaemon)


kde4_add_plugin(kded_kremotecontrol ${kded_kremotecontrol_SRCS})
target_link_libraries(kded_kremotecontrol
   ${KDE4_KDECORE_LIBS}
   ${KDE4_KDEUI_LIBS}
   ${KDE4_SOLID_LIBS}
   ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY}
   libkremotecontrol
)

install(TARGETS kded_kremotecontrol DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kremotecontrol.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install( FILES krcd.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/krcd )
