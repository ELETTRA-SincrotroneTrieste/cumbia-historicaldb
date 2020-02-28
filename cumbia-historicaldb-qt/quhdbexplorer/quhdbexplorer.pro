isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT=/usr/local/cumbia-libs
}

packagesExist(cumbia-qtcontrols) {
    PKGCONFIG += cumbia-qtcontrols
} else {
    message("quhdbbrowser: package cumbia-qtcontrols not found")
}

exists($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri) {
    include($${INSTALL_ROOT}/include/qumbia-tango-controls/qumbia-tango-controls.pri)
}

exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/qutimearray3dplotplugin_i.h){
    message("quhdbexplorer: detected qutimearray3dplotplugin ")
    DEFINES += HAS_QUTIMEARRAY3D
} else {
    message("quhdbexplorer: qutimearray3dplotplugin is missing: only scalar values can be displayed")
}

include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

include(../cuhdb-qt-lib/cuhdb-qt-lib.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += debug

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../cuhdb-qt-lib/src/
LIBS += -L../cuhdb-qt-lib -lcuhdb-qt

SOURCES += \
    main.cpp \
    quhdbexplorer.cpp

HEADERS += \
    quhdbexplorer.h

DISTFILES += \
    README.md


inst.files = $${TARGET}
inst.path = $${INSTALL_ROOT}/bin

INSTALLS += inst
