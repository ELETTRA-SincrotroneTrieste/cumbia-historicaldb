#include "cumbiahdb.h"
#include "cuhdb_config.h"
#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiahdbworld.h>
#include <cuhdbactionfactoryservice.h>
#include <cuhdbactionfactoryi.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>
#include <dbsettings.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> // strerror

class CumbiaHdbPrivate {
public:
    CuData db_conn_params;
    DbSettings* hdbx_settings;
};

CumbiaHdb::CumbiaHdb(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb)
{
    d = new CumbiaHdbPrivate;
    d->hdbx_settings = nullptr;
    m_threadsEventBridgeFactory = teb;
    m_threadFactoryImplI = tfi;
    m_init();
}

CumbiaHdb::~CumbiaHdb()
{
    pdelete("~CumbiaHdb %p", this);
    /* all registered services are unregistered and deleted by cumbia destructor after threads have joined */
    if(m_threadsEventBridgeFactory)
        delete m_threadsEventBridgeFactory;
    if(m_threadFactoryImplI)
        delete m_threadFactoryImplI;
    if(d->hdbx_settings)
        delete d->hdbx_settings;
}

void CumbiaHdb::m_init()
{
    getServiceProvider()->registerService(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType),
                                          new CuHdbActionFactoryService());

}

void CumbiaHdb::addAction(const std::string &source, CuDataListener *l, const CuHdbActionFactoryI &f)
{
    CuHdbActionFactoryService *af =
            static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));

    CuHdbActionI *a = af->find(source, f.getType());
    if(!a) {
        a = af->registerAction(source, f, this);
        if(!d->hdbx_settings)
            setDbProfile(PROFILE_DEFAULT_NAME);
        a->setHdbXSettings(d->hdbx_settings);
        a->start();
    }
    a->addDataListener(l);
}

void CumbiaHdb::unlinkListener(const std::string &source, CuHdbActionI::Type t, CuDataListener *l)
{
    CuHdbActionFactoryService *af =
            static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));
    CuHdbActionI * action = af->find(source, t);
    if(action) {
        action->removeDataListener(l);
    }
}

CuHdbActionI *CumbiaHdb::findAction(const std::string &source, CuHdbActionI::Type t) const
{
    CuHdbActionFactoryService *af =
            static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));
    return af->find(source, t);
}

CuThreadFactoryImplI *CumbiaHdb::getThreadFactoryImpl() const
{
    return m_threadFactoryImplI;
}

CuThreadsEventBridgeFactory_I *CumbiaHdb::getThreadEventsBridgeFactory() const
{
    return m_threadsEventBridgeFactory;
}

void CumbiaHdb::setDbProfile(const string &db_profile_name) {
    CumbiaHdbWorld world;
    std::string fnam =  world.getDbProfilesDir() + "/" + db_profile_name + "." + std::string(PROFILES_EXTENSION);
    DbSettings* hdbxs = new DbSettings();
    hdbxs->loadFromFile(fnam.c_str());
    if(hdbxs->hasError()) {
        perr("CumbiaHdb.setDbProfile: failed to open profile \"%s\": %s", db_profile_name.c_str(),
             hdbxs->getError().c_str());
        delete hdbxs;
    }
    else
        setHdbXSettings(hdbxs);
}

/*!
 * \brief Set custom HdbXSettings
 * \param hdb_settings a pointer to a previously allocated HdbXSettings object
 *
 * \par note
 * CumbiaHdb takes the ownership of hdb_settings, that is destroyed either after a new call to
 * setHdbXSettings or within CumbiaHdb class destructor.
 */
void CumbiaHdb::setHdbXSettings(DbSettings *hdb_settings) {
    if(d->hdbx_settings)
        delete d->hdbx_settings;
    d->hdbx_settings = hdb_settings;
}

DbSettings *CumbiaHdb::hdbXSettings() const {
    return d->hdbx_settings;
}

CuData CumbiaHdb::getDbParams() const
{
    return d->db_conn_params;
}

int CumbiaHdb::getType() const
{
    return CumbiaHdbType;
}
