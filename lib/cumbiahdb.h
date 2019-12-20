#ifndef CUMBIAHDB_H
#define CUMBIAHDB_H

#include <cumbia.h>
#include <string>
#include <cuhdbactionreader.h>
#include <cuhdbactionfactoryi.h>

class CuThreadFactoryImplI;
class CuThreadsEventBridgeFactory_I;
class CuDataListener;
class CumbiaHdbPrivate;
class HdbXSettings;

/*!
 * \mainpage
 *
 * \section introduction Introduction
 *
 * The CumbiaHdb module
 */
class CumbiaHdb : public Cumbia
{

public:
    enum Type { CumbiaHdbType = Cumbia::CumbiaUserType + 14 };
    enum DbType { Hdb, Hdbpp };

    CumbiaHdb(CuThreadFactoryImplI *tfi, CuThreadsEventBridgeFactory_I *teb);

    ~CumbiaHdb();

    CuThreadFactoryImplI* getThreadFactoryImpl() const;

    CuThreadsEventBridgeFactory_I* getThreadEventsBridgeFactory() const;

    void setDbProfile(const std::string& db_profile_name);
    void setHdbXSettings(HdbXSettings *hdb_settings);
    HdbXSettings *hdbXSettings() const;


    CuData getDbParams() const;

    virtual int getType() const;

    void addAction(const std::string &source, CuDataListener *l, const CuHdbActionFactoryI &f);
    void unlinkListener(const string &source, CuHdbActionI::Type t, CuDataListener *l);
    CuHdbActionI *findAction(const std::string &source, CuHdbActionI::Type t) const;
private:

    void m_init();
    CuThreadsEventBridgeFactory_I *m_threadsEventBridgeFactory;
    CuThreadFactoryImplI *m_threadFactoryImplI;
    CumbiaHdbPrivate *d;
};

#endif // CUMBIAHDB_H
