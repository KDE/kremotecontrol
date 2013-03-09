project(kremotecontrol)

if(NOT UNIX)
  message(FATAL_ERROR "KRemoteControl is only supposed to be built on Unix platforms.")
endif(NOT UNIX)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

# If definitions like -D_GNU_SOURCE are needed for these checks they
# should be added to _KDE4_PLATFORM_DEFINITIONS when it is originally
# defined outside this file.  Here we include these definitions in
# CMAKE_REQUIRED_DEFINITIONS so they will be included in the build of
# checks below.
set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})

macro_log_feature(QT_QTXMLPATTERNS_FOUND "QtXmlPatterns" "Qt support for XPath, XQuery, XSLT and XML Schema validation" "http://doc.trolltech.com/latest/qtxmlpatterns.html" TRUE "" "Required to build kremotecontrol.")

macro_display_feature_log()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(libkremotecontrol)
add_subdirectory(kcmremotecontrol)
add_subdirectory(profiles)
add_subdirectory(kded)
add_subdirectory(krcdnotifieritem)
add_subdirectory(dataengine)

if(KDE4_BUILD_TESTS)
  add_subdirectory(tests)
endif(KDE4_BUILD_TESTS)

add_subdirectory(doc)
