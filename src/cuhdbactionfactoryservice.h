#ifndef ACTIONFACTORYSERVICE_H
#define ACTIONFACTORYSERVICE_H

#include <cuservicei.h>
#include <cuhdbactioni.h>
#include <vector>

class CuHdbActionFactoryI;
class CumbiaHdb;
class CuDataListener;

class CuHDBActionFactoryServicePrivate;

/*! \brief implements CuServiceI interface and provides a service to register, unregister
 *         and find *cumbia historical database actions*
 *
 * \note This service is internally used by CumbiaHdb and clients of the library should
 *       rarely need to use it.
 *
 * This class is a cumbia *service* (implements getName and getType). It is registered by
 * CumbiaTango at construction time through the cumbia service provider, namely CuServiceProvider:
 *
 * \code
   getServiceProvider()->registerService(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType), new CuActionFactoryService());
 * \endcode
 *
 * A reference to the service can be obtained through the Cumbia::getServiceProvider method:
 *
 * \code
   CuActionFactoryService *af =
            static_cast<CuActionFactoryService *>(getServiceProvider()->get(static_cast<CuServices::Type> (CuActionFactoryService::CuActionFactoryServiceType)));
 * \endcode
 *
 * Through the service provider, the service can be unregistered with CuServiceProvider::unregisterService
 *
 * Once the service is registered, new CuHDBActionI can be added through
 * registerAction, removed with unregisterAction and searched with find and findActive.
 * The count method returns the number of registered actions.
 *
 * Tango sources (for readers) and targets (for writers) with the same name and type, share the same
 * CuHDBActionI. See the registerAction documentation for further details.
 *
 */
class CuHdbActionFactoryService : public CuServiceI
{
public:
    enum Type { CuHdbActionFactoryServiceType = CuServices::User + 35 };

    CuHdbActionFactoryService();

    virtual ~CuHdbActionFactoryService();

    CuHdbActionI* registerAction(const std::string& src, const CuHdbActionFactoryI& f, CumbiaHdb *ct);

    CuHdbActionI *findActive(const std::string &name, CuHdbActionI::Type at);

    std::vector<CuHdbActionI *> find(const std::string &name, CuHdbActionI::Type at);

    size_t count() const;

    void unregisterAction(const std::string& src, CuHdbActionI::Type at);

    void cleanup();

    // CuServiceI interface
public:
    std::string getName() const;

    CuServices::Type getType() const;

private:
    CuHDBActionFactoryServicePrivate *d;
};

#endif // DEVICEFACTORYSERVICE_H
