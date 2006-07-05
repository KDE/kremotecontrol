



########### install files ###############

install( FILES profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml  DESTINATION ${DATA_INSTALL_DIR}/profiles)




#original Makefile.am contents follow:

#profiledata_DATA = profile.dtd noatun.profile.xml klauncher.profile.xml konqueror.profile.xml
#profiledatadir = $(kde_datadir)/profiles
#
#EXTRA_DIST = $(profiledata_DATA)
#
