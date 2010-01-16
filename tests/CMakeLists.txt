ENABLE_TESTING() 

include_directories(${KDE4_INCLUDES}	${QT_INCLUDES})


set(kdelirctest_SRCS kdelirctest.cpp)
kde4_add_unit_test(kdelirctest TESTNAME kdelirctest ${kdelirctest_SRCS})

target_link_libraries(
  kdelirctest
  kdelirc_shared
  ${QT_QTTEST_LIBRARY}
  ${QT_QTCORE_LIBRARY}
  ${KDE4_KDECORE_LIBS}
)
