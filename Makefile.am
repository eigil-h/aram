AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS = $(gtkmm_CFLAGS) $(jack2_CFLAGS) -std=c++0x -pthread
bin_PROGRAMS = warsaw
warsaw_SOURCES = src/main/cc/main.cc src/main/cc/application.cc src/main/cc/gui/window.cc src/main/cc/audio/audiosystem.cc src/main/cc/audio/jackclient.cc
warsaw_LDADD = $(gtkmm_LIBS) $(jack2_LIBS)
warsaw_LDFLAGS = -pthread
## dist_noinst_SCRIPTS = autogen.sh
