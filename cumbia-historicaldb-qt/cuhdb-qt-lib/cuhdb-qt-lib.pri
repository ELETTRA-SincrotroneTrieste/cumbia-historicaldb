isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}


exists($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri) {
	message("cuhdb-qt-lib: using cumbia-qtcontrols installation under $${INSTALL_ROOT}")
	include($${INSTALL_ROOT}/include/cumbia-qtcontrols/cumbia-qtcontrols.pri)
} else {
	error("cuhdb-qt-lib: did not find cumbia-qtcontrols installation under $${INSTALL_ROOT}")
}

INCLUDEDIR = $${INSTALL_ROOT}/include/cuhdb-qt-lib
SHAREDIR = $${INSTALL_ROOT}/share
DOCDIR = $${SHAREDIR}/doc/cuhdb-qt-lib
LIBDIR = $${INSTALL_ROOT}/lib

CONFIG += link_pkgconfig
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
