
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
   ${KDE4WORKSPACE_INCLUDE_DIR} )


set(libkremotecontrol_LIB_SRCS
   remote.cpp
   mode.cpp
   action.cpp
   prototype.cpp
   argument.cpp
   dbusaction.cpp
   modeswitchaction.cpp
   profileaction.cpp
   profileactiontemplate.cpp
   profile.cpp
   profileserver.cpp
   executionengine.cpp
   actionexecutor.cpp
   dbusactionexecutor.cpp
   dbusinterface.cpp
   remote.cpp
   remotelist.cpp
)

kde4_add_library(libkremotecontrol SHARED ${libkremotecontrol_LIB_SRCS})
set_target_properties(libkremotecontrol PROPERTIES VERSION 1.0.0 SOVERSION 1)

target_link_libraries(libkremotecontrol ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY} ${QT_QTXMLPATTERNS_LIBRARY})


install(TARGETS libkremotecontrol ${INSTALL_TARGETS_DEFAULT_ARGS})
