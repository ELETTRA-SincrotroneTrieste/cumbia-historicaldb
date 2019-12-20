#ifndef CUHdbREADER_H
#define CUHdbREADER_H

#include <QString>
#include <cucontrolsfactories_i.h>
#include <cucontrolsreader_abs.h>

class CuHdbReaderFactoryPrivate;
class CuHdbReaderPrivate;

class CuHdbReaderFactory : public CuControlsReaderFactoryI
{
public:
    CuHdbReaderFactory();

    virtual ~CuHdbReaderFactory();

    CuControlsReaderA *create(Cumbia *c, CuDataListener *l) const;

    virtual void setOptions(const CuData &options);

    // CuControlsReaderFactoryI interface
public:
    CuControlsReaderFactoryI *clone() const;

private:
    CuHdbReaderFactoryPrivate *d;
};

class CuHdbReader : public CuControlsReaderA
{
public:
    CuHdbReader(Cumbia *cumbia_tango, CuDataListener *tl);

    virtual ~CuHdbReader();

    void setSource(const QString& s);

    QString source() const;

    void unsetSource() ;

    void setOptions(const CuData& o);

    CuData getOptions() const;

private:
    CuHdbReaderPrivate *d;

    // CuControlsReaderA interface
public:
    virtual void sendData(const CuData &d);
    virtual void getData(CuData &d_ino) const;
};

#endif // CUHdbREADER_H
