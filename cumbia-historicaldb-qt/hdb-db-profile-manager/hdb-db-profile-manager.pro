isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
    include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
}else {
    message("hdb-db-profile-manager: missing file $${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri")
    message("hdb-db-profile-manager: cumbia-qtcontrols library must be installed under INSTALL_ROOT")
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

PROFILES_DIR=$$(HOME)/.config/cumbia-hdb
DEFINES += PROFILES_DIR=\"\\\"$${PROFILES_DIR}\\\"\"
message("hdb-db-profile-manager: profiles dir: $${PROFILES_DIR}")

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
