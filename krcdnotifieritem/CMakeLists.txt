add_subdirectory( icons ) 

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
#    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${QT_INCLUDES}
    ${KDE4WORKSPACE_INCLUDE_DIR} )

set(krcdnotifieritem_SRCS
    main.cpp
    krcdnotifieritem.cpp 
)

#qt4_add_dbus_adaptor(krcdnotifieritem_SRCS org.kde.krcdnotifieritem.xml krcdnotifieritem.h KrcdNotifierItem) 

kde4_add_executable( krcdnotifieritem ${krcdnotifieritem_SRCS})

target_link_libraries(krcdnotifieritem  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} 
                                      ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} libkremotecontrol)

install(TARGETS krcdnotifieritem ${INSTALL_TARGETS_DEFAULT_ARGS})
install( PROGRAMS krcdnotifieritem.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
