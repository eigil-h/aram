AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
AM_CPPFLAGS = $(gtkmm_CFLAGS) $(jack2_CFLAGS) $(odb_CFLAGS) $(odbsqlite_CFLAGS) -std=c++11 -pthread
bin_PROGRAMS = aram
aram_SOURCES = src/main/cc/main.cc src/main/cc/gui/windowmanager.cc src/main/cc/gui/window.cc \
        src/main/cc/service/system.cc src/main/cc/service/audioengine.cc src/main/cc/service/jackclient.cc \
        src/main/cc/service/database.cc src/main/cc/service/buffers.cc src/main/cc/model/project.cc \
        src/main/cc/model/project-odb.cxx src/main/cc/model/audioclip.cc src/main/cc/model/audioclip-odb.cxx
aram_LDADD = $(gtkmm_LIBS) $(jack2_LIBS) $(odb_LIBS) $(odbsqlite_LIBS)
aram_LDFLAGS = -pthread

noinst_PROGRAMS = testrun
testrun_SOURCES = src/test/cc/main.cc src/test/cc/service/buffers_test.cc src/main/cc/service/buffers.cc
testrun_LDADD = $(sqlite_LIBS)
testrun_LDFLAGS = -pthread

## dist_noinst_SCRIPTS = autogen.sh
