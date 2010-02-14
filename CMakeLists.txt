project(kdelirc)

add_subdirectory( libkremotecontrol )
add_subdirectory( kcmlirc )
add_subdirectory( profiles )
add_subdirectory(kded)
add_subdirectory(dataengine)
add_subdirectory(applet)

if(KDE4_BUILD_TESTS)
  add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)

