kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/profiles FILES profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml )

kde4_footer()



#original Makefile.am contents follow:

#profiledata_DATA = profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml
#profiledatadir = $(kde_datadir)/profiles
#
#EXTRA_DIST = $(profiledata_DATA)
#
