find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

include(KDE4Defaults)
include(${QT_USE_FILE})
include_directories(
  ${CMAKE_SOURCE_DIR} 
  ${CMAKE_BINARY_DIR} 
  ${KDE4_INCLUDES} 
  ${PLASMA_INCLUDE_DIR}
  ${KDE4WORKSPACE_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol
)

set(kremoteconrol_engine_SRCS
    kremotecontrolengine.cpp
)

kde4_add_plugin(plasma_engine_kremoteconrol ${kremoteconrol_engine_SRCS})

target_link_libraries(plasma_engine_kremoteconrol ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} libkremotecontrol)

install(TARGETS plasma_engine_kremoteconrol DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-engine-kremotecontrol.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

