#include "cuhdbfetchactivity.h"
#include "hdb_source.h"
#include "cuhdbactioni.h"
#include <cumacros.h>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
#include <assert.h>
#include <hdbxsettings.h>
#include <timeinterval.h>
#include <cumbiahdbworld.h>

/* @private */
class CuHdbActivityPrivate
{
public:
    int repeat, errCnt;
    int exec_cnt;
    std::string message, label, thread_token;
    pthread_t my_thread_id, other_thread_id;
    bool exiting;
    double min, max;
    size_t size;
    Hdbextractor *hdb_extractor;
    HdbXSettings *hdbx_s;
};

/*! \brief the class constructor that sets up a Tango polling activity
 *
 * @param token a CuData that will describe this activity
 * @param df a pointer to a CuDeviceFactoryService that is used by init and onExit to create/obtain and
 *        later get rid of a Tango device, respectively
 * @param argins input arguments that can optionally be passed to Tango commands as argins
 *
 * \par notes
 * \li the default polling period is 1000 milliseconds
 * \li if the "period" key is set on the token, then it is converted to int and it will be used
 *     to set up the polling period
 * \li CuADeleteOnExit is active.
 * \li CuAUnregisterAfterExec is disabled because if the Tango device is not defined into the database
 *     the poller is not started and the activity is suspended (repeat will return -1).
 */
CuHdbFetchActivity::CuHdbFetchActivity(const CuData &token, HdbXSettings *hdbxs)
    : CuIsolatedActivity(token)
{
    d = new CuHdbActivityPrivate;
    d->repeat = -1;
    d->errCnt = 0;
    d->other_thread_id = pthread_self();
    d->exiting = false;
    d->exec_cnt = 0;
    d->label = "hdb";
    d->hdb_extractor = nullptr;
    d->hdbx_s = hdbxs;
    //  flag CuActivity::CuADeleteOnExit is true
    setFlag(CuActivity::CuAUnregisterAfterExec, true);
}

/*! \brief the class destructor
 *
 * deletes the internal data
 */
CuHdbFetchActivity::~CuHdbFetchActivity()
{
    delete d;
}


/** \brief returns true if the passed token's *device* *activity* and *period* values matche this activity token's
 *         *device* and *activity* and *period* values.
 *
 * @param token a CuData containg key/value pairs of another activity's token
 * @return true if the input token's "period" *device* and *activity* values match this token's
 * "period" *device* and *activity* values
 *
 * Two CuHdbGenActivity match if they refer to the same device and have the same period.
 *
 * @implements CuActivity::matches
 */
bool CuHdbFetchActivity::matches(const CuData &token) const
{
    const CuData& mytok = getToken();
    return token["src"] == mytok["src"];
}


/*! \brief the implementation of the CuActivity::init hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 *
 * See also CuActivity::init, execute and onExit
 *
 * @implements CuActivity::init
 *
 */
void CuHdbFetchActivity::init()
{
    auto t1 = std::chrono::steady_clock::now();
    d->my_thread_id = pthread_self();
    assert(d->other_thread_id != d->my_thread_id);
    // simulate a configuration (property type)
    CuData res = getToken();
    d->thread_token = threadToken()["thtok"].toString();
    res["type"] = "hdb";
    res["mode"] = "hdb";
    m_putInfo(res);
    auto t2 = std::chrono::steady_clock::now();
    res["elapsed_us"] = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    publishResult(res);
}

/*! \brief the implementation of the CuActivity::execute hook
 *
 * This is repeatedly called in the CuActivity's thread of execution, according to the
 * period chosen for the reader.
 *
 *
 * See also CuActivity::execute and CuEventActivity
 *
 * @implements CuActivity::execute
 */
void CuHdbFetchActivity::execute()
{
    assert(d->my_thread_id == pthread_self());
    d->exec_cnt++;
    CuData res = getToken();
    printf("CuHdbFetchActivity.execute: data %s\n", res.toString().c_str());
    m_putInfo(res);
    if(d->hdbx_s == nullptr) {
        res["err"] = true;
        res["msg"] = "CuHdbFetchActivity: database configuration is unset";
    }
    else {
        d->hdb_extractor = new Hdbextractor(this);
        d->hdb_extractor->setHdbXSettings(d->hdbx_s);
        d->hdb_extractor->setUpdateProgressPercent(10);
        bool ok = d->hdb_extractor->connect();
        if(res.containsKey("start_date") && res.containsKey("stop_date")) {
            TimeInterval tint(res["start_date"].toString().c_str(), res["stop_date"].toString().c_str());
            if(ok)
                d->hdb_extractor->getData(res["src"].toString().c_str(), &tint);
            else {
                res["err"] = true;
                res["msg"] = std::string("CuHdbFetchActivity.execute: failed to connect to database host: "
                                         + d->hdbx_s->get("dbhost") + " user: " + d->hdbx_s->get("dbuser") +
                                         "pass: " + d->hdbx_s->get("dbpass") + ": " +
                                         std::string(d->hdb_extractor->getErrorMessage()));
                perr("CuHdbFetchActivity.execute: %s", res.toString().c_str());
            }
            res["err"] = (d->hdb_extractor->hasError());
            if(res["err"].toBool())
                res["msg"] = std::string(d->hdb_extractor->getErrorMessage());
            if(ok) {
                ok = d->hdb_extractor->findErrors(res["src"].toString().c_str(), &tint);
            }
            if(!ok) {
                res["err"] = true;
                res["msg"] = std::string(d->hdb_extractor->getErrorMessage());
            }
            if(!ok) // publish result if !ok
                publishResult(res);
        } // res.containsKeys start and stop date
        else if(res.containsKey("find_pattern")) {
            // find sources matching a given pattern
            std::list<std::string> srcs;
            ok = d->hdb_extractor->findSources(res["find_pattern"].toString().c_str(), srcs);
            if(ok) {
                res["sources"] = std::vector<std::string>(srcs.begin(), srcs.end());
                publishResult(res);   // publish result
            }
            else {
                res["err"] = true;
                res["msg"] = std::string(d->hdb_extractor->getErrorMessage());
            }
        }
        else if(res.containsKey("query")) {
            Result *result;
            double elapsed;
            ok = d->hdb_extractor->query(res["query"].toString().c_str(), result, &elapsed);
            res["err"] = !ok;
            if(!ok)
                res["msg"] = d->hdb_extractor->getErrorMessage();
            else {
                CumbiaHdbWorld w;
                // fill row_count, column_count, columns, result
                w.extract_data(result, elapsed, res);
            }
            publishResult(res);   // publish result
        }
    }

}

/*! \brief the implementation of the CuActivity::onExit hook
 *
 * This is called in the CuActivity's thread of execution.
 *
 * \li client reference counter is decreased on the TDevice (TDevice::removeRef)
 * \li CuDeviceFactoryService::removeDevice is called to remove the device from the device factory
 *     if the reference count is zero
 * \li the result of the operation is *published* to the main thread through publishResult
 *
 * See also CuActivity::onExit
 *
 * @implements CuActivity::onExit
 */
void CuHdbFetchActivity::onExit()
{
    assert(d->my_thread_id == pthread_self());
    d->exiting = true;
    CuData at = getToken(); /* activity token */
    at["exit"] = true;
    // do not publishResult because CuPoller (which is our listener) may be deleted by CuPollingService
    // from the main thread when its action list is empty (see CuPollingService::unregisterAction)
    publishResult(at);
    if(d->hdb_extractor)
        delete d->hdb_extractor;
}

void CuHdbFetchActivity::m_putInfo(CuData &res)
{
    res["mode"] = "hdb";
    res["period"] = d->repeat;
    res["thread"] = d->thread_token;
    const CuData& atok = getToken();
    if(atok.containsKey("label"))
        res["label"] = atok["label"];
    res.putTimestamp(); // timestamp_ms and timestamp_us
}

void CuHdbFetchActivity::event(CuActivityEvent *e)
{

}

/*! \brief returns the type of the polling activity
 *
 * @return the CuHdbGenActivityType value defined in the Type enum
 */
int CuHdbFetchActivity::getType() const
{
    return CuHdbFetchActivityType;
}

void CuHdbFetchActivity::onSourceProgressUpdate(const char *name, double percent) {
    CuData res = getToken();
    res["src"] = std::string(name);
    res["progress"] = percent;
    res["err"] = false;
    std::vector<XVariant> data;
    d->hdb_extractor->get(data);
    CumbiaHdbWorld wo;
    wo.extract_data(data, res);
    publishResult(res);
    printf("CuHdbFetchActivity::onSourceProgressUpdate %s %f\n", name, percent);
}

void CuHdbFetchActivity::onExtractionFinished(int totalRows, double elapsed)
{
    printf("CuHdbFetchActivity.onExtractionFinished: totalRows %d elapsed %f\n", totalRows, elapsed);
}

void CuHdbFetchActivity::onSourceExtractionFinished(const char *name, int totalRows, double elapsed)
{
    CuData res;
    res["src"] = std::string(name);
    res["progress"] = 100;
    res["elapsed"] = elapsed;
    res["err"] = false;
    std::vector<XVariant> data;
    d->hdb_extractor->get(data);
    CumbiaHdbWorld wo;
    wo.extract_data(data, res);
    publishResult(res);
    printf("CuHdbFetchActivity::onSourceExtractionFinished %s rows: %d elapsed: %f\n", name, totalRows, elapsed);
}
