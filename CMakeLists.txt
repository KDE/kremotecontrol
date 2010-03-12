project(kdelirc)

######### to be removed on inclusion in kdeutils - begin ####################
find_package( KDE4 REQUIRED )
include( KDE4Defaults )
include( MacroLibrary )
add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
######### to be removed on inclusion in kdeutils - end ####################

add_subdirectory(libkremotecontrol)
add_subdirectory(kcmremotecontrol)
add_subdirectory(profiles)
add_subdirectory(kded)
add_subdirectory(krcdnotifieritem)
add_subdirectory(dataengine)
add_subdirectory(doc)

if(KDE4_BUILD_TESTS)
  add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)
