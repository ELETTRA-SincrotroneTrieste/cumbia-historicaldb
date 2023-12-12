######################################################################
# Automatically generated by qmake (3.1) Mon Dec 16 13:53:21 2019
######################################################################

include(cuhdb-qt-lib.pri)

TEMPLATE = lib
TARGET = cuhdb-qt
INCLUDEPATH += .
DISTFILES += cuhdb-qt-lib.pri

DEFINES += CUMBIA_PRINTINFO

VERSION_HEX = 0x010100
VERSION = 1.1.0

DEFINES += CUHDB_QT_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    CUHDB_QT_VERSION=$${VERSION_HEX}

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += src/cuhdbreader.h \
    src/quhdbbrowser.h
SOURCES += src/cuhdbreader.cpp \
    src/quhdbbrowser.cpp

doc.commands = \
doxygen \
Doxyfile;

doc.files = doc/*
doc.path = $${DOCDIR}
QMAKE_EXTRA_TARGETS += doc

inc.files = $${HEADERS}
inc.path = $${INCLUDEDIR}

other_inst.files = $${DISTFILES}
other_inst.path = $${INCLUDEDIR}

target.path = $${LIBDIR}
INSTALLS += target inc other_inst

# generate pkg config file
CONFIG += create_pc create_prl no_install_prl

QMAKE_PKGCONFIG_NAME = cuhdb-qt-lib
QMAKE_PKGCONFIG_DESCRIPTION = Qt module for cumbia-hdb
QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
QMAKE_PKGCONFIG_LIBDIR = $${target.path}
QMAKE_PKGCONFIG_INCDIR = $${inc.path}
QMAKE_PKGCONFIG_VERSION = $${VERSION}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_REQUIRES = cumbia-qtcontrols hdbextractor++ libsqldb

LIBS -= -lcuhdb-qt
