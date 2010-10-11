ENABLE_TESTING() 
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
link_directories(${CMAKE_BINARY_DIR}/libkremotecontrol/ ${CMAKE_CURRENT_BINARY_DIR})


include_directories(
  ${KDE4_INCLUDES}	${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../libkremotecontrol/
  ${CMAKE_CURRENT_BINARY_DIR}/../libkremotecontrol/
)


# set(kdelirctest_SRCS kdelirctest.cpp profiletest.cpp)

# kde4_add_unit_test(kdelirctest TESTNAME kdelirctest ${kdelirctest_SRCS})
set(profileTest_SRCS  profiletest.cpp)
set(remoteTest_SRCS  remotetest.cpp)

kde4_add_unit_test(profileTest TESTNAME profileTest ${profileTest_SRCS})
kde4_add_unit_test(remoteTest TESTNAME remoteTest ${remoteTest_SRCS})

target_link_libraries(
  profileTest
  libkremotecontrol   
  ${QT_QTCORE_LIBRARY} 
  ${QT_QTTEST_LIBRARY}  
)

target_link_libraries(
  remoteTest
  libkremotecontrol    
  ${QT_QTTEST_LIBRARY}  
)
