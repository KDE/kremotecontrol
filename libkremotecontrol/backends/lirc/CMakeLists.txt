include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BUILD_DIR}
)

set(kremotecontrol_lirc_PART_SRCS
    lircclient.cpp
    lircremotecontrol.cpp
    lircremotecontrolmanager.cpp
)

kde4_add_library(kremotecontrol_lirc_static STATIC ${kremotecontrol_lirc_PART_SRCS})
target_link_libraries(kremotecontrol_lirc_static ${KDE4_KDECORE_LIBS} libkremotecontrol)

kde4_add_plugin(kremotecontrol_lirc module.cpp ${kremotecontrol_lirc_PART_SRCS})

target_link_libraries(kremotecontrol_lirc ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} libkremotecontrol)
install(TARGETS kremotecontrol_lirc DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kremotecontrol_lirc.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kremotecontrolbackends )
