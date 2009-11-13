
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${KDE4_INCLUDES}
   ${QT_INCLUDES}
   ${KDE4WORKSPACE_INCLUDE_DIR} )


set(kdelirc_shared_LIB_SRCS
   iraction.cpp 
   iractions.cpp 
   prototype.cpp 
   modes.cpp 
   arguments.cpp 
   mode.cpp 
   profileserver.cpp
)

kde4_add_library(kdelirc_shared SHARED ${kdelirc_shared_LIB_SRCS})
set_target_properties(kdelirc_shared PROPERTIES VERSION 1.0.0 SOVERSION 1)

target_link_libraries(kdelirc_shared ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4WORKSPACE_SOLIDCONTROL_LIBRARY})


install(TARGETS kdelirc_shared ${INSTALL_TARGETS_DEFAULT_ARGS})
