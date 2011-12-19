include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
   ${X11_INCLUDE_DIR}
)


set(libkremotecontrol_LIB_SRCS
   ifaces/remotecontrolinterface.cpp
   ifaces/remotecontrolmanagerinterface.cpp
#   hw/remotecontrol.cpp
#   hw/remotecontrolmanager.cpp
   remotecontrolmanager.cpp
   remotecontrol.cpp
   remotecontrolbutton.cpp

   remote.cpp
   remotelist.cpp
   mode.cpp
   prototype.cpp
   argument.cpp
   action.cpp
   dbusaction.cpp
   profileaction.cpp
   profileactiontemplate.cpp
   profile.cpp
   profileserver.cpp
   executionengine.cpp
   actionexecutor.cpp
   dbusactionexecutor.cpp
   dbusinterface.cpp
   keypressaction.cpp
   keypressactionexecutor.cpp
)

kde4_add_library(libkremotecontrol SHARED ${libkremotecontrol_LIB_SRCS})
set_target_properties(libkremotecontrol PROPERTIES VERSION 1.0.0 SOVERSION 1)

target_link_libraries(libkremotecontrol ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${QT_QTXMLPATTERNS_LIBRARY} ${QT_QTSCRIPT_LIBRARY} ${X11_X11_LIB} ${X11_XTest_LIB})


install(TARGETS libkremotecontrol ${INSTALL_TARGETS_DEFAULT_ARGS})
install( FILES kremotecontrolmanager.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR}  )

add_subdirectory(backends)
