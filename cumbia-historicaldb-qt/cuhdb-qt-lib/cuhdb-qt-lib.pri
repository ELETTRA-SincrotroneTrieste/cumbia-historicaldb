isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
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

packagesExist(libsqldb) {
    PKGCONFIG += libsqldb
    message("cuhdb-qt-lib: found libsqldb")
}
else {
    error("cuhdb-qt-lib: missing dependency libsqldb")
}

packagesExist(hdbextractor++) {
    PKGCONFIG += hdbextractor++
    message("cuhdb-qt-lib: found hdbextractor++")
}
else {
    error("cuhdb-qt-lib: missing dependency hdbextractor++")
}

packagesExist(cumbia-hdb) {
    PKGCONFIG += cumbia-hdb
    message("cuhdb-qt-lib: found cumbia-hdb")
}
else {
    error("cuhdb-qt-lib: missing dependency cumbia-hdb")
}


INCLUDEPATH += $${INCLUDEDIR}

LIBS += -L$${LIBDIR} -lcuhdb-qt
