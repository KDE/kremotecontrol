ENABLE_TESTING() 

include(KDE4Defaults)

include_directories(
  ${KDE4_INCLUDES}	${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol/
  ${CMAKE_CURRENT_BINARY_DIR}/../libkremotecontrol/
)


set(kdelirctest_SRCS kdelirctest.cpp profiletest.cpp)
kde4_add_unit_test(kdelirctest TESTNAME kdelirctest ${kdelirctest_SRCS})

target_link_libraries(
  kdelirctest  
  libkremotecontrol
  ${QT_QTTEST_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${KDE4_KDECORE_LIBS}  
)

