
add_subdirectory( icons )

include_directories(
   ${CMAKE_SOURCE_DIR}/libkremotecontrol/
   ../libkremotecontrol/
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
   ${KDE4WORKSPACE_INCLUDE_DIR} )

########### next target ###############


set(libirkick_common_SRCS
        main.cpp
        irkick.cpp
        klircclient.cpp
)


set(irkick_KDEINIT_SRCS ${libirkick_common_SRCS})

qt4_add_dbus_adaptor(irkick_KDEINIT_SRCS org.kde.irkick.xml irkick.h KRemoteControlDaemon)

kde4_add_kdeinit_executable( irkick ${irkick_KDEINIT_SRCS})

target_link_libraries(kdeinit_irkick  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS}
                                      ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} kdelirc_shared)
install(TARGETS kdeinit_irkick ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(irkick kdeinit_irkick)
install(TARGETS irkick ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES irkick.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES irkick.desktop  DESTINATION ${AUTOSTART_INSTALL_DIR})
install( FILES irkick.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/irkick )

kde4_install_icons( ${ICON_INSTALL_DIR} )
