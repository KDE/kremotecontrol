kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/profiles FILES profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml )

kde4_footer()



#original Makefile.am contents follow:

#profiledata_DATA = profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml
#profiledatadir = $(kde_datadir)/profiles
#
#EXTRA_DIST = $(profiledata_DATA)
#
