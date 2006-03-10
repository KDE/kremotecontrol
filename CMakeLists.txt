kde4_header()

add_subdirectory( icons ) 
add_subdirectory( kdelirc ) 
add_subdirectory( irkick ) 
add_subdirectory( kcmlirc ) 
add_subdirectory( profiles ) 
add_subdirectory( remotes ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#
#SUBDIRS = icons kdelirc irkick kcmlirc profiles remotes
#
