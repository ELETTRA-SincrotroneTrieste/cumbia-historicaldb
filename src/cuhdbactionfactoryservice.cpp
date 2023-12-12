#include "cuhdbactionfactoryservice.h"
#include "cuhdbactionfactoryi.h"
#include "hdb_source.h"
#include "cuhdbactioni.h"
#include <map>
#include <cumacros.h>

/*! @private
 */
class CuHDBActionFactoryServicePrivate
{
public:
    std::list<CuHdbActionI * > actions;
};

/*! \brief the class constructor
 *
 */
CuHdbActionFactoryService::CuHdbActionFactoryService()
{
    d = new CuHDBActionFactoryServicePrivate;
}

CuHdbActionFactoryService::~CuHdbActionFactoryService()
{
    pdelete("~CuActionFactoryService %p", this);
    delete d;
}

/** \brief Create and register a new action if an action with the same source and type is not already running.
 *
 * \par Rules
 *  \li same source, different CuHdbActionI type: two distinct actions.
 *  \li Share actions of the same type among sources with the same name.
 *  \li If an action with the same source and type exists, but is stopping, it is discarded and a new
 *      action is created.
 *
 * @see findAction
 * @see unregisterAction
 */
CuHdbActionI *CuHdbActionFactoryService::registerAction(const std::string& src,
                                                       const CuHdbActionFactoryI &f,
                                                       CumbiaHdb* ct)
{
    CuHdbActionI* action = NULL;
    std::list<CuHdbActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it) // use HdbSource.src to get the full source
        if((*it)->getType() == f.getType() && (*it)->getSource().src() == src) {
            break;
        }

    if(it == d->actions.end())
    {
        action = f.create(src, ct);
        d->actions.push_back(action);
    }
    return action;
}

CuHdbActionI * CuHdbActionFactoryService::find(const string &src, CuHdbActionI::Type at)
{
    std::list<CuHdbActionI *>::const_iterator it;
    for(it = d->actions.begin(); it != d->actions.end(); ++it) {
        if((*it)->getType() == at && (*it)->getSource().src() == src)
            return (*it);
    }
    return nullptr;
}

/*! \brief return the number of registered actions
 *
 * @return the number of registered actions.
 *
 * Actions are added with registerAction and removed with unregisterAction
 */
size_t CuHdbActionFactoryService::count() const
{
    return d->actions.size();
}

/** \brief Unregister the action with the given source and type if the action's exiting() method returns true
 *
 * @param src the complete source name (no wildcards)
 * @param at the action type (Reader, Writer, AttConfig, ...)
 *
 * if an action with the given source (target) and type is found and the action is
 * exiting (CuHdbActionI::exiting returns false), it is removed
 * from the internal list of actions.
 *
 * If CuHdbActionI::exiting returns false, the action is not removed.
 *
 * \note
 * The action is <strong>not deleted</strong>.
 * CuTReader and CuTWriter auto delete themselves from within CuTReader::onResult and CuTWriter::onResult
 * when the *exiting* flag is true.
 *
 */
void CuHdbActionFactoryService::unregisterAction(const std::string &src, CuHdbActionI::Type at)
{
    std::list<CuHdbActionI *>::iterator it;
    size_t siz = d->actions.size();

    it = d->actions.begin();
    while( it != d->actions.end())  {
        if((*it)->getType() == at && (*it)->getSource().src() == src && (*it)->exiting()) {
            it = d->actions.erase(it);
        }
        else
            ++it;
    }
    if(d->actions.size() == siz) // nothing removed
        perr("CuActionFactoryService::unregisterAction: no actions unregistered");
}

/*! \brief all registered actions are <strong>deleted</strong> and removed from the service internal list.
 *
 * All references to the registered actions are removed from the service internal list,
 * whether or not CuHdbActionI::exiting returns true
 *
 * \note
 * The action is <strong>deleted</strong>.
 */
void CuHdbActionFactoryService::cleanup()
{
    std::list<CuHdbActionI *>::iterator it = d->actions.begin();
    while(it != d->actions.end())
    {
        delete (*it);
        it = d->actions.erase(it);
    }
}

/*! \brief returns the service name
 *
 * @return the constant string "*CuHdbActionFactoryService" identifying this type of service.
 *
 * Implements CuServiceI::getName pure virtual method
 */
std::string CuHdbActionFactoryService::getName() const
{
    return std::string("CuHdbActionFactoryService");
}

/*! \brief returns the service type
 *
 * @return the value defined in cuactionfactoryservice.h  by the *enum Type*
 * (CuHdbActionFactoryServiceType = CuServices::User + 25)
 *
 * @see Type
 *
 * Implements CuHdbActionI::getType pure virtual method
 */
CuServices::Type CuHdbActionFactoryService::getType() const
{
    return static_cast<CuServices::Type >(CuHdbActionFactoryServiceType);
}
