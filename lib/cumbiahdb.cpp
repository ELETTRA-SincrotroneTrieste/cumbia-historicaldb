#include "cumbiahdb.h"

#include <cumacros.h>
#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumbiahdbworld.h>
#include <cuhdbactionfactoryservice.h>
#include <cuhdbactionfactoryi.h>
#include <cuthreadfactoryimpl.h>
#include <cuthreadseventbridgefactory_i.h>
#include <hdbxsettings.h>
// to get home dir
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h> // strerror

#define HOME_DB_CONFIG_DIR ".config/cumbiahdb"

class CumbiaHdbPrivate {
public:
    CuData db_conn_params;
    HdbXSettings* hdbx_settings;
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
    CumbiaHdbWorld w;
    if(w.source_valid(source))
    {
        CuHdbActionFactoryService *af =
                static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));

        CuHdbActionI *a = af->findActive(source, f.getType());
        if(!a) {
            a = af->registerAction(source, f, this);
            a->setHdbXSettings(d->hdbx_settings);
            a->start();
        }
        a->addDataListener(l);
    }
    else
        perr("CumbiaHdb.addAction: source \"%s\" is not valid, ignoring", source.c_str());
}

void CumbiaHdb::unlinkListener(const string &source, CuHdbActionI::Type t, CuDataListener *l)
{
    CuHdbActionFactoryService *af =
            static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));
    std::vector<CuHdbActionI *> actions = af->find(source, t);
    for(size_t i = 0; i < actions.size(); i++) {
        actions[i]->removeDataListener(l); /* when no more listeners, a stops itself */
    }
}

CuHdbActionI *CumbiaHdb::findAction(const std::string &source, CuHdbActionI::Type t) const
{
    CuHdbActionFactoryService *af =
            static_cast<CuHdbActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));
    CuHdbActionI* a = af->findActive(source, t);
    return a;
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
    // getpwuid return value may point to a static area, and may be overwritten
    // by subsequent calls. (Do not pass the returned  pointer to free)
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    std::string fnam = std::string(homedir) + "/" + std::string(HOME_DB_CONFIG_DIR) + "/" + db_profile_name + ".dat";
    HdbXSettings* hdbxs = new HdbXSettings();
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
void CumbiaHdb::setHdbXSettings(HdbXSettings *hdb_settings) {
    if(d->hdbx_settings)
        delete d->hdbx_settings;
    d->hdbx_settings = hdb_settings;
}

HdbXSettings *CumbiaHdb::hdbXSettings() const {
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
