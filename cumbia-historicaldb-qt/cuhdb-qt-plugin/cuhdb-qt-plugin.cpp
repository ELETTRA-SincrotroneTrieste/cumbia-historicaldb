#include "cuhdb-qt-plugin.h"
#include <cumbiahdb.h>
#include <cumbiahdbworld.h>
#include <QtDebug>
#include <cuthreadfactoryimpl.h>
#include <qthreadseventbridgefactory.h>
#include "cuhdbreader.h"

class CuHdbQtPluginPrivate
{
public:
    CumbiaHdb *cuhdb;
    CuControlsReaderFactoryI* r_fac;
};

CuHdbQtPlugin::CuHdbQtPlugin(QObject *parent) : QObject(parent)
{
    d = new CuHdbQtPluginPrivate;
    d->cuhdb = new CumbiaHdb(new CuThreadFactoryImpl(), new QThreadsEventBridgeFactory());
}

CuHdbQtPlugin::~CuHdbQtPlugin()
{
    delete d;
    delete d->r_fac;
}

Cumbia *CuHdbQtPlugin::getCumbia() const
{
    return d->cuhdb;
}

std::vector<std::string> CuHdbQtPlugin::getSrcPatterns() const {
    CumbiaHdbWorld w;
    return w.srcPatterns();
}

CuControlsReaderFactoryI *CuHdbQtPlugin::getReaderFactory() const {
    if(!d->r_fac)
        d->r_fac = new CuHdbReaderFactory();
    return d->r_fac;
}

void CuHdbQtPlugin::setDbProfile(const QString &profile_name) {
    d->cuhdb->setDbProfile(profile_name.toStdString());
}


#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cuhdb-qt-plugin, CuHdbQtPlugin)
#endif // QT_VERSION < 0x050000
