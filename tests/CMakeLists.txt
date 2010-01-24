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
set(profiletest_SRCS  profiletest.cpp)

kde4_add_unit_test(profiletest TESTNAME profileTest ${profiletest_SRCS})

target_link_libraries(
  profiletest
  libkremotecontrol    
  ${QT_QTTEST_LIBRARY}  
)

