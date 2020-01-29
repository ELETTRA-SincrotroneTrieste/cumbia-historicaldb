#include "cuhdbactionreader.h"
#include "cumbiahdb.h"
#include "cuhdbactionfactoryservice.h"

#include <cudatalistener.h>
#include <cuserviceprovider.h>
#include <cumacros.h>
#include <set>
#include <cuthreadfactoryimpl_i.h>
#include <cuthreadseventbridgefactory_i.h>
#include <cuactivitymanager.h>
#include <cuhdbfetchactivity.h>

#include "cuhdbactionreader.h"
#include <cumacros.h>
#include <hdbxsettings.h>

class CuHdbActionReaderPrivate
{
public:
    std::set<CuDataListener *> listeners;
    HdbSource hdb_src;
    CumbiaHdb *cumbia_hdb;
    bool exit;
    CuHdbFetchActivity *hdb_fetch_a;
    CuData options, value_d;
    HdbXSettings *hdbx_settings;
};

CuHdbActionReader::CuHdbActionReader(const HdbSource& src, CumbiaHdb *ct) : CuHdbActionI()
{
    d = new CuHdbActionReaderPrivate;
    d->hdb_src = src;
    d->cumbia_hdb = ct;
    d->hdbx_settings = nullptr;
    d->exit = false;  // set to true by stop
}

CuHdbActionReader::~CuHdbActionReader()
{
    pdelete("~CuHdbActionReader %p\n", this);
    delete d;
}

/*! \brief progress notification callback
 *
 * @param step the completed steps in the background
 * @param total the total number of steps
 * @param data CuData with data from the background activity
 *
 * The current implementation does nothing
 */
void CuHdbActionReader::onProgress(int step, int total, const CuData &data) {
    (void) step;  (void) total;  (void) data;
}

void CuHdbActionReader::onResult(const std::vector<CuData> &datalist) {
    (void) datalist;
}

void CuHdbActionReader::setOptions(const CuData &options) {
    d->options = options;
}

/*
 * \brief delivers to the main thread the result of a task executed in background.
 *
 * See  \ref md_lib_cudata_for_tango
 *
 * The d->exit flag is true only if the CuHdbActionReader::stop has been called. (data listener destroyed
 * or reader disconnected ("unset source") )
 * Only in this case CuHdbActionReader auto deletes itself when data["exit"] is true.
 * data["exit"] true is not enough to dispose CuHdbActionReader because CuHdbActionReader handles two types of
 * activities (polling and event).
 *
 * If the error flag is set by the CuEventActivity because subscribe_event failed, the poller is started
 * and the error *is not* notified to the listener(s)
 *
 */
void CuHdbActionReader::onResult(const CuData &data)
{
    bool a_exit = data["exit"].toBool(); // activity exit flag
    // iterator can be invalidated if listener's onUpdate unsets source: use a copy
    std::set<CuDataListener *> lis_copy = d->listeners;
    std::set<CuDataListener *>::iterator it;
    // if it's just subscribe_event failure, do not notify listeners
    for(it = lis_copy.begin(); it != lis_copy.end(); ++it) {
        (*it)->onUpdate(data);
    }

    /* remove last listener and delete this
     * - if d->exit is set to true (CuHdbActionReader has been stop()ped )
     */
    if(d->exit && a_exit)
    {
        CuHdbActionFactoryService * af = static_cast<CuHdbActionFactoryService *>(d->cumbia_hdb->getServiceProvider()
                                                                                  ->get(static_cast<CuServices::Type>(CuHdbActionFactoryService::CuHdbActionFactoryServiceType)));
        af->unregisterAction(d->hdb_src.getName(), getType());
        d->listeners.clear();
        delete this;
    }
}

/*! \brief returns the CuData storing the token that identifies this action
 *
 * @return a CuData with the following key/value pairs
 *
 * \li "source" --> the name of the source (string)
 * \li "type" --> a constant string: "reader"
 */
CuData CuHdbActionReader::getToken() const
{
    CuData da("source", d->hdb_src.getName());
    da["type"] = std::string("hdb");
    return da;
}

/*! returns the *tango* source (TSource)
 *
 * @return a TSource object that describes the Tango source
 */
HdbSource CuHdbActionReader::getSource() const
{
    return d->hdb_src;
}

CuHdbActionI::Type CuHdbActionReader::getType() const
{
    return CuHdbActionI::HdbFetch;
}

/** \brief Send data with parameters to configure the reader.
 *
 * @param data a CuData bundle with the settings to apply to the reader.
 *
 * \par Valid keys
 * \li "period": integer. Change the polling period, if the refresh mode is CuHdbActionReader::PolledRefresh
 * \li "refresh_mode". A CuHdbActionReader::RefreshMode value to change the current refresh mode.
 * \li "read" (value is irrelevant). If the read mode is CuHdbActionReader::PolledRefresh, a read will be
 *     performed.
 *
 * @see getData
 *
 */
void CuHdbActionReader::sendData(const CuData &data)
{

}

/** \brief Get parameters from the reader.
 *
 * @param d_inout a reference to a CuData bundle containing the parameter names
 *        as keys. getData will associate the values to the keys.
 *        Unrecognized keys are ignored.
 *
 * \par Valid keys
 * \li "period": returns an int with the polling period
 * \li "refresh_mode": returns a CuHdbActionReader::RefreshMode that can be converted to int
 * \li "mode": returns a string representation of the CuHdbActionReader::RefreshMode
 *
 * @see sendData
 */
void CuHdbActionReader::getData(CuData &d_inout) const
{
    //    if(d_inout.containsKey("mode"))
    //        d_inout["mode"] = refreshModeStr();
}

/*
 * main thread
 */
void CuHdbActionReader::stop()
{
    if(!d->exit) {
        d->exit = true;
        m_stopHdbGenActivity();
    }
}

void CuHdbActionReader::addDataListener(CuDataListener *l)
{
    std::set<CuDataListener *>::iterator it = d->listeners.begin();
    d->listeners.insert(it, l);
}

void CuHdbActionReader::removeDataListener(CuDataListener *l)
{
    d->listeners.erase(l);
    if(d->listeners.size() == 0)
        stop();
}

size_t CuHdbActionReader::dataListenersCount()
{
    return d->listeners.size();
}

bool CuHdbActionReader::exiting() const
{
    return d->exit;
}

void CuHdbActionReader::setHdbXSettings(HdbXSettings *hdbs) {
    d->hdbx_settings = hdbs;
}

void CuHdbActionReader::start()
{
    m_startFetchHdbActivity();
}

void CuHdbActionReader::m_startFetchHdbActivity()
{
    CuData at(d->cumbia_hdb->getDbParams());
    at["src"] =  d->hdb_src.getName();
    if(d->hdb_src.getType() == HdbSource::DataFetch) {
        at["start_date"] = d->hdb_src.start_date();
        at["stop_date"] = d->hdb_src.stop_date();
        at["fetch_property"] = d->options.containsKey("fetch_property") && d->options["fetch_property"].toBool();
        at["no_fetch_errors"] = d->options.containsKey("no_fetch_errors") && d->options["no_fetch_errors"].toBool();
    }
    else if(d->hdb_src.getType() == HdbSource::FindSources) {
        at["find_pattern"] = d->hdb_src.find_pattern();
    }
    else if(d->hdb_src.getType() == HdbSource::Query) {
        at["query"] = d->hdb_src.query();
    }
    at["activity"] = "hdb";
    if(d->options.containsKey("label"))
        at["label"] = d->options["label"].toString();
    CuData tt; // thread token
    tt["token"] = "hdbextractor";
    d->hdb_fetch_a = new CuHdbFetchActivity(at, d->hdbx_settings);
    const CuThreadsEventBridgeFactory_I &bf = *(d->cumbia_hdb->getThreadEventsBridgeFactory());
    const CuThreadFactoryImplI &fi = *(d->cumbia_hdb->getThreadFactoryImpl());
    d->cumbia_hdb->registerActivity(d->hdb_fetch_a, this, tt, fi, bf);
}

void CuHdbActionReader::m_stopHdbGenActivity()
{
    d->cumbia_hdb->unregisterActivity(d->hdb_fetch_a);
    d->hdb_fetch_a = NULL; // not safe to dereference henceforth
}

