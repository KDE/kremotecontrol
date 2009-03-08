project(klirc)

find_package(KDE4 REQUIRED)
#find_package(QT REQUIRED)

#add_definitions( -DQT3_SUPPORT ${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( ${QT4_DEFINITIONS} ${KDE4_DEFINITIONS})


set(kdelirc_shared_SRCS
	${CMAKE_SOURCE_DIR}/kdelirc/iraction.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/arguments.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/iractions.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/prototype.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/modes.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/mode.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/profileserver.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/remoteserver.cpp
	${CMAKE_SOURCE_DIR}/kdelirc/dbusqueryinterface.cpp
)


add_subdirectory( icons )
add_subdirectory( irkick )
add_subdirectory( kcmlirc )
add_subdirectory( profiles )
add_subdirectory( remotes )




