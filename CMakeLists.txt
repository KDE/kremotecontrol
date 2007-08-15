
set(libkdelirc_SRCS
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/iraction.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/arguments.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/iractions.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/prototype.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/modes.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/mode.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/profileserver.cpp 
	${CMAKE_SOURCE_DIR}/kdelirc/kdelirc/remoteserver.cpp
)


add_subdirectory( icons )
add_subdirectory( kdelirc )
add_subdirectory( irkick )
add_subdirectory( kcmlirc )
add_subdirectory( profiles )
add_subdirectory( remotes )




