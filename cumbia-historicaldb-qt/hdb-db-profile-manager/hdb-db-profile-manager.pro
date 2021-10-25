isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

# INSTALL_ROOT is used to install the target
# prefix is used within DEFINES +=
#
# cumbia installation script uses a temporary INSTALL_ROOT during build
# and then files are copied into the destination prefix. That's where
# configuration files must be found by the application when the script
# installs everything at destination
#
isEmpty(prefix) {
    prefix = $${INSTALL_ROOT}
}


exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
    include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
}else {
    message("hdb-db-profile-manager: missing file $${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri")
    message("hdb-db-profile-manager: cumbia-qtcontrols library must be installed under INSTALL_ROOT")
}

# PKG_CONFIG += cumbia-hdb below is enough but if hdbextractor++ is missing
# the error message says "package cumbia-hdb is required", which is not correct
# So, output a more specific error if hdbextractor++ is not found
!packagesExist(hdbextractor++) {
    error("hdb-db-profile-manager hdbextractor++ library is required")
}

packagesExist(cumbia-hdb) {
    PKGCONFIG += cumbia-hdb
}else{
    error("hdb-db-profile-manager: package cumbia-hdb is required")
}

QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

CONFIG -= QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        profile.cpp

inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin

INSTALLS += inst

DISTFILES += \
    dbtemplate.dat

RESOURCES += \
    hdb-db-profile-manager.qrc

HEADERS += \
    profile.h
