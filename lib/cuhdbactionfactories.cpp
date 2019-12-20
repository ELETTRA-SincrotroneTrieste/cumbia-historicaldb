#include "cuhdbactionfactories.h"
#include "cuhdbactionreader.h"

class CuHdbReaderFactoryPrivate {
    public:

};

CuHdbActionReaderFactory::CuHdbActionReaderFactory()
{

}

void CuHdbActionReaderFactory::setOptions(const CuData &o)
{
    options = o;
}

CuHdbActionReaderFactory::~CuHdbActionReaderFactory()
{

}

/** \brief creates and returns a CuHdbActionReader, (that implements the CuHdbActionI interface)
 *
 * @param s a string with the name of the source
 * @param ct a pointer to CumbiaHdb
 * @return a CuHdbActionReader, that implements the CuHdbActionI interface
 *
 * The source and the CumbiaHdb reference are passed to the CuHdbActionReader
 * If options have been set, normally through CuHdbActionReaderFactory::create,
 * they are used to configure the CuHdbActionReader.
 *
 * \par options
 * The recognised options are:
 * \li "period" an integer, in milliseconds, for the polling period. Converted with: CuVariant::toInt
 * \li "refresh_mode": an integer defining the Tango refresh mode. Converted with: CuVariant::toInt
 *
 * \par note
 * Please use the CuTangoOptBuilder class rather than filling in the options manually.
 *
 * @see CuTangoOptBuilder
 */
CuHdbActionI *CuHdbActionReaderFactory::create(const std::string &s, CumbiaHdb *c_hdb) const
{    
    CuHdbActionReader* reader = new CuHdbActionReader(s, c_hdb);
    reader->setOptions(options);
    return reader;
}

CuHdbActionI::Type CuHdbActionReaderFactory::getType() const
{
    return CuHdbActionI::HdbFetch;
}


