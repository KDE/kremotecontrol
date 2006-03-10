kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/remotes FILES RM-0010.remote.xml cimr100.remote.xml sherwood.remote.xml                    hauppauge.remote.xml sonytv.remote.xml remote.dtd )

kde4_footer()



#original Makefile.am contents follow:

#remotedata_DATA = RM-0010.remote.xml cimr100.remote.xml sherwood.remote.xml \
#                  hauppauge.remote.xml sonytv.remote.xml remote.dtd
#remotedatadir = $(kde_datadir)/remotes
#
#EXTRA_DIST = $(remotedata_DATA)
#
