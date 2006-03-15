kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )

add_subdirectory(irkick)
add_subdirectory(kcmlirc)


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#
#KDE_LANG = en
#KDE_DOCS = AUTO
#SUBDIRS = $(AUTODIRS)
#
