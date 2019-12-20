isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)

INCLUDEDIR = $${INSTALL_ROOT}/include/cuhdb-qt-lib
SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cuhdb-qt-lib
LIBDIR = $${INSTALL_ROOT}/lib

packagesExist(cumbia-hdb) {
    PKGCONFIG += cumbia-hdb
}
else {
    message("cuhdb-qt-lib: missing dependency cumbia-hdb")
}

packagesExist(hdbextractor++) {
    PKGCONFIG += hdbextractor++
}
else {
    message("cuhdb-qt-lib: missing dependency hdbextractor++")
}

INCLUDEPATH += $${INCLUDEDIR}

LIBS += -L$${LIBDIR} -lcuhdb-qt
