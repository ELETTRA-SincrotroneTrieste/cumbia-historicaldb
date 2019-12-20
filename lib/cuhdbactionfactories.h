#ifndef CUHdbACTIONFACTORIES_H
#define CUHdbACTIONFACTORIES_H

#include <cuhdbactioni.h>
#include <cuhdbactionfactoryi.h>

class CuHdbActionReaderFactory : public CuHdbActionFactoryI
{
public:
    CuHdbActionReaderFactory();

    void setOptions(const CuData &o);

    virtual ~CuHdbActionReaderFactory();

    // CuTangoActionFactoryI interface
public:
    CuHdbActionI *create(const std::string &s, CumbiaHdb *c_hdb) const;

    CuHdbActionI::Type getType() const;

    bool isShareable() const;

private:
    CuData options;

};

#endif // CUHdbACTIONFACTORIES_H
