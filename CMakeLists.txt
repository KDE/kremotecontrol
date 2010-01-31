project(kdelirc)

#add_subdirectory( kdelirc )
add_subdirectory( libkremotecontrol )
add_subdirectory( kcmlirc )
#add_subdirectory( irkick )
add_subdirectory( profiles )
add_subdirectory(kded)


if(KDE4_BUILD_TESTS)
  add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)

