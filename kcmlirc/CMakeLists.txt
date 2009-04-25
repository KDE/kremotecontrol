add_subdirectory( icons )

include_directories( 
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../kdelirc
	${CMAKE_CURRENT_BINARY_DIR}/../kdelirc
	${KDE4_INCLUDES}
	${QT_INCLUDES} )

#link_directories(${KDE4_LIB_DIR} ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_BINARY_DIR}/lib)
########### next target ###############

set(kcm_lirc_PART_SRCS
   kcmlirc.cpp 
   addaction.cpp 
   editmode.cpp 
   editaction.cpp
   newmodedialog.cpp
   model.cpp 
   dbusinterface.cpp)

QT4_ADD_DBUS_ADAPTOR(kcm_lirc_PART_SRCS org.kde.kcmlirc.xml dbusinterface.h DBusInterface)

kde4_add_ui_files(kcm_lirc_PART_SRCS addactionbase.ui newmode.ui editactionbase.ui editmodebase.ui kcmlircbase.ui )

kde4_add_plugin(kcm_lirc ${kcm_lirc_PART_SRCS})

target_link_libraries(kcm_lirc ${KDE4_KIO_LIBS} kdelirc_shared)

install(TARGETS kcm_lirc DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcm_lirc.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

