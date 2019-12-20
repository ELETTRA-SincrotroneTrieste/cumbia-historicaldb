#-------------------------------------------------
#
# Project created by QtCreator 2017-12-27T15:40:52
#
#-------------------------------------------------
include(../cuhdb-qt-lib/cuhdb-qt-lib.pri)

TARGET = cuhdb-qt-plugin
TEMPLATE = lib
CONFIG += plugin debug

SOURCES += \
    cuhdb-qt-plugin.cpp

HEADERS += \
    cuhdb-qt-plugin.h

DISTFILES += cuhdb-qt-plugin.json

inc.files += $${HEADERS}

unix {
    target.path = $${DEFINES_CUMBIA_QTCONTROLS_PLUGIN_DIR}
    INSTALLS += target
}

LIBS += -lcumbia-hdb -lcuhdb-qt

message("cuhdb-qt-plugin: plugin installation dir:  $${DEFINES_CUMBIA_QTCONTROLS_PLUGIN_DIR}")

# qumbia-plugins.pri defines default INSTALLS for target inc and doc
# doc commands, target.path and inc.path are defined there as well.
