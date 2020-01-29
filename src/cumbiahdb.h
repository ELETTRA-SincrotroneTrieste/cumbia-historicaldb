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
 * \mainpage Cumbia hdb module
 *
 * \section introduction Introduction
 *
 * The CumbiaHdb module
 *
 * \subsection Supported databases
 *
 * \subsection Data queries
 *
 * \subsection Custom queries
 *
 * \subsection List of attributes stored into the database
 *
 *
 * \subsection CuData contents from the cumbia-hdb module
 *
 * |Key          | Type       | Value             | Description   | Notes      |
 * |-------------|------------|-------------------|---------------|------------|
 * | src         | string     | the name of the source | The complete name of the source, without *hdb://* domain prefix and start/stop date | eg: *tango://tango-host:PORT/test/device/tg_test/double_scalar*        |
 * | time_scale_us | vector<double> | vector of double: seconds.microseconds |  `data_time` columns from the hdb++ database, MySQL type DATATIME | convert with toDouble. Note for Qt: use QDateTime::fromMSecsSinceEpoch(v[i] * 1000).toString("yyyy-MM-dd hh:mm:ss.zzz")) |
 * |notes_time_scale_us | std::vector<double> | vector of double: seconds.microseconds | `data_time` columns from the hdb++ database, MySQL type DATATIME, WHERE value is NULL | Conversion notes as time_scale_us |
 * |notes             | std::vector<std::string>  | vector of strings | error messages from the database. i-th notes_time_scale_us --> i-th note | convert with toStringVector |
 * |timestamp_ms      | long int        | timestamp, in milliseconds | timestamp of the single value fetched from the database | If data size from the db is > 1, this field *is not present* |
 * |timestamp_us      | double          | timestamp, double, seconds.microseconds | same semantics as timestamp_ms | Present if data size == 1 |
 * |data_format_str   | string          | "scalar" or "vector"   | The format of data | convert with toString |
 * |write_mode_str    | string          | "ro" or "rw"           | The write mode, either read only (ro) or read write (rw) | convert with toString |
 * |data_type_str     | string          | one of "double", "int", "uint", "bool", "string" | Type of data stored in *value* | use the type to choose the appropriate conversion method for *value* and *w_value* |
 * |value             | vector of *one of* double,int,uint,bool,string | the data value | Tango "read" value from the historical database | convert with the appropriate toXXXVector according to data_type_str |
 * |w_value           | same as value | the *Tango* *write* value (set point), if write_mode_str is "rw" | use data_type_str to convert to a vector of the appropriate type | Available for read/write attributes |
 * |err               | bool          | true or false | true if an error occurred during the database extraction process | the error condition concerns only the data extraction, not possible *null* values in the database. For the quality of the data recorded into the database, refer to *notes_time_scale_us, notes, quality_color, quality* |
 * |msg               | string        | a string message | any kind of message originated from the extraction, typically an error description | use in conjunction with *err* |
 * |has_null          | bool          | true if at least one value from the database is null (error condition during the save process) | see also *null_cnt*, *valid_cnt*, *notes_time_scale_us, notes* | |
 * |null_cnt          | int           | number of null values in the result | notes_time_scale_us.size, as int | convert with toInt |
 * |valid_cnt         | int           | number of valid values in the result| time_scale_us.size | convert with toInt |
 * |start_date        | string        | lower boundary of time interval | start date time | convert with toString. Format: yyyy-MM-dd hh:mm:ss |
 * |stop_date         | string        | upper boundary of time interval | stop date time | convert with toString. Format: yyyy-MM-dd hh:mm:ss  |
 * |elapsed           | double        | elapsed time, seconds.microseconds| time spent by the database query | *optional field * |
 * |columns           | std::vector<std::string> | name of the columns involved in a *custom query* | list of columns in database result | present in *custom queries* only. Use toStringVector |
 * |column_count      | size_t aka long int | number of columns in a *custom query* result | number of columns in the result | present in *custom queries* only. Use toLongInt |
 * |results           |std::vector<std::string> | the results of the query, row after row | in *custom queries*, stores the results in a sequence of rows one after another. Use column_count to separate each row |  each row of results is inserted at the back. Use toStringVector. Use in conjunction with *row_count* |
 * |row_count         | size_T aka long int | number of results | use toLongInt |
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
