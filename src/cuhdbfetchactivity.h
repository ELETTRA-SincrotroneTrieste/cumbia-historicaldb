#ifndef CUHDBGENACTIVITY_H
#define CUHDBGENACTIVITY_H

#include <cuisolatedactivity.h>
#include <cuactivityevent.h>
#include <list>
#include <cuhdbactioni.h>
#include <hdb_source.h>
#include <hdbextractor.h>
#include <hdbextractorlistener.h>

class CuData;
class CuHdbActivityPrivate;

/*! \brief an activity to periodically read from Tango. Implements CuContinuousActivity
 *
 * Implementing CuActivity, the work is done in the background by the three methods
 *
 * \li init
 * \li execute
 * \li onExit
 *
 * They are called from a separate thread. In cumbia-tango, threads are grouped by device.
 *
 * Apart from the three execution hooks listed above, some other methods have been written
 * to comply with the CuActivityInterface:
 *
 * \li getType, returning CuEventActivityType
 * \li event, to receive CuActivityEvent events
 * \li matches, that returns true if the input token represents an activity like this
 * \li repeat, that indicates whether or not execute has to be called again
 *
 * \note
 * CuHdbGenActivity *must stay in execution (must not exit)* until CuTReader is stopped (CuTReader::stop).
 * For this reason, CuAUnregisterAfterExec is set to false. When the Tango device is not defined into the
 * database (in CuHdbGenActivity::execute the Tango::DeviceProxy is NULL) the repeat time is set to a negative
 * number. This suspends the continuous activity without exiting. It will be asked to exit from CuTReader::stop
 * through Cumbia::unregisterActivity.
 *
 * CuADeleteOnExit is left to true in order to let the activity be deleted by CuThread after onExit.
 *
 * This combination of the flags guarantees the availability of the CuHdbGenActivity when CuTReader::stop is called
 * (activity will be either executing the polling loop or suspended) and lets CuTReader not worry about the activity deletion.
 *
 * @see CuTReader
 * @see CuTReader::stop
 *
 */
class CuHdbFetchActivity : public CuIsolatedActivity, public HdbExtractorListener
{
public:

    /*! \brief defines the Type of the activity, returned by getType
     */
    enum Type { CuHdbFetchActivityType = CuActivity::User + 7 };

    CuHdbFetchActivity(const CuData& token, HdbXSettings *hdbxs);

    ~CuHdbFetchActivity();

    // CuActivity interface
public:
    bool matches(const CuData &token) const;

protected:
    void init();
    void execute();
    void onExit();

private:
    CuHdbActivityPrivate *d;

    void m_putInfo(CuData &res);

public:
    void event(CuActivityEvent *e);

    int getType() const;

    // HdbExtractorListener interface
public:
    void onSourceProgressUpdate(const char *name, double percent);
    void onExtractionFinished(int totalRows, double elapsed);
    void onSourceExtractionFinished(const char *name, int totalRows, double elapsed);
};

#endif // CUPOLLINGACTIVITY_H
