project(kdelirc)

add_subdirectory(libkremotecontrol)
add_subdirectory(kcmremotecontrol)
add_subdirectory(profiles)
add_subdirectory(kded)
add_subdirectory(krcdnotifieritem)
add_subdirectory(dataengine)

if(KDE4_BUILD_TESTS)
  add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)
