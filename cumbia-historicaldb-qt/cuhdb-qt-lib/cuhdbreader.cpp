#include "cuhdbreader.h"
#include "cumbiahdb.h"
#include "cuhdbreader.h"
#include "cuhdbactionreader.h"
#include "cuhdbactionfactories.h"
#include <cudatalistener.h>
#include <cudata.h>
#include <cumacros.h>
#include <QString>
#include <vector>
#include <string>
#include <QtDebug>

class CuHdbReaderFactoryPrivate {
public:
    CuData options;

};

CuHdbReaderFactory::CuHdbReaderFactory()
{
    d = new CuHdbReaderFactoryPrivate;
}

CuHdbReaderFactory::~CuHdbReaderFactory()
{
    delete d;
}

CuControlsReaderA *CuHdbReaderFactory::create(Cumbia *c, CuDataListener *l) const
{
    CuHdbReader *r = new CuHdbReader(c, l);
    r->setOptions(d->options);
    return r;
}

CuControlsReaderFactoryI *CuHdbReaderFactory::clone() const
{
    CuHdbReaderFactory *f = new CuHdbReaderFactory();
    f->setOptions(d->options);
    return f;
}

void CuHdbReaderFactory::setOptions(const CuData &options)
{
    d->options = options;
}

/*! @private
 */
class CuHdbReaderPrivate
{
public:
    QString source;
    CumbiaHdb *cumbia_hdb;
    CuDataListener *tlistener;
    CuData options;
};

CuHdbReader::CuHdbReader(Cumbia *cumbia_hdb, CuDataListener *tl)
    : CuControlsReaderA(cumbia_hdb, tl)
{
    assert(cumbia_hdb->getType() == CumbiaHdb::CumbiaHdbType);
    d = new CuHdbReaderPrivate;
    d->cumbia_hdb = static_cast<CumbiaHdb *>(cumbia_hdb);
    d->tlistener = tl;
}

CuHdbReader::~CuHdbReader()
{
    pdelete("CuHdbReader %p", this);
    d->tlistener->invalidate();
    unsetSource();
    delete d;
}

void CuHdbReader::setSource(const QString &s)
{
    d->source = s;
    CuHdbActionReaderFactory hdbrf;
    d->cumbia_hdb->addAction(s.toStdString(), d->tlistener, hdbrf);
}

QString CuHdbReader::source() const
{
    return d->source;
}

void CuHdbReader::unsetSource()
{
    d->cumbia_hdb->unlinkListener(d->source.toStdString(), CuHdbActionI::HdbFetch, d->tlistener);
    d->source = QString();
}

void CuHdbReader::setOptions(const CuData &o)
{
    d->options = o;
}

CuData CuHdbReader::getOptions() const
{
    return d->options;
}

void CuHdbReader::sendData(const CuData &da)
{
    CuHdbActionI *a = d->cumbia_hdb->findAction(d->source.toStdString(), CuHdbActionI::HdbFetch);
    if(a)
        a->sendData(da);
}

void CuHdbReader::getData(CuData &d_ino) const
{
    CuHdbActionI *a = d->cumbia_hdb->findAction(d->source.toStdString(), CuHdbActionI::HdbFetch);
    if(a)
        a->getData(d_ino);
}
