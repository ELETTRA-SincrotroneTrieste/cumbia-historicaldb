#include "cumbiahdbworld.h"
#include <xvariant.h>
#include <result.h>
#include <cudata.h>
#include <algorithm> // std::find
// to get home dir
#include <pwd.h>
#include <unistd.h>
#include <string.h> // strlen
#include <regex> // source valid
#include "hdb_source.h"
#include "cuhdb_config.h"
#include "hdb_source.h" // for source regex

CumbiaHdbWorld::CumbiaHdbWorld()
{
    m_src_patterns.push_back("hdb://.+");
    m_src_patterns.push_back("hdb++://.+");
    m_src_patterns.push_back("hdbpp://.+");
    m_src_patterns.push_back(HdbSource::hdb_source_regexp());
}

void CumbiaHdbWorld::setSrcPatterns(const std::vector<std::string> &pat_regex)
{
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaHdbWorld::srcPatterns() const
{
    return m_src_patterns;
}

bool CumbiaHdbWorld::source_valid(const std::string &s) const
{
    HdbSource hs(s);
    return hs.isValid();
}

void CumbiaHdbWorld::extract_data(const std::vector<XVariant> &dbdata, CuData &res) const
{
    // xvariant.h: enum DataType { TypeInvalid = -1, Int, UInt, Double, Boolean, String };
    // lib/cpp/server/idl/tango.h:  enum AttrQuality { ATTR_VALID, ATTR_INVALID, ATTR_ALARM, ATTR_CHANGING, ATTR_WARNING /*, __max_AttrQuality=0xffffffff */ };
    std::vector<double>  timestamps_us, nulls_timestamps_us;
    struct timeval tv;
    double ts;
    std::vector<double > dvalues, dwvalues;
    std::vector<long int> livalues, liwvalues;
    std::vector<unsigned long int> ulivalues, uliwvalues;
    std::vector<bool> bvalues, bwvalues;
    std::vector<std::string> svalues, swvalues, errors;
    XVariant::DataType datatyp = XVariant::TypeInvalid;
    XVariant::DataFormat datafmt = XVariant::FormatInvalid;
    XVariant::Writable writable = XVariant::WritableInvalid;

    for(size_t i = 0; i < dbdata.size(); i++) {
        const XVariant& v = dbdata[i];
        // timestamps
        tv = v.getTimevalTimestamp();

        ts = static_cast<time_t>(tv.tv_sec);
        ts += tv.tv_usec * 1e-6;
        // values
        if(v.hasErrorDesc()) {
            nulls_timestamps_us.push_back(ts);
            errors.push_back(std::string(v.getError()));
            res["notes_time_scale_us"] = nulls_timestamps_us;
            res["notes"] = errors;
        }
        else if(!v.isNull()) {
            // no error
            timestamps_us.push_back(ts);
            res["time_scale_us"] = timestamps_us;
            datafmt = v.getFormat();
            datatyp = v.getType();
            writable = v.getWritable();
            if(dbdata.size() == 1) {
                // one single value: make result compatible with a single read
                res["timestamp_ms"] = static_cast<long int>(ts * 1000.0);
                res["timestamp_us"] = ts;
            }
            if(datafmt == XVariant::Scalar) {
                res["data_format_str"] = "scalar";
                res["write_mode_str"] = "ro";
                if(datatyp == XVariant::Double) {
                    dvalues.push_back(v.toDouble());
                    res["value"] = dvalues;
                    res["data_type_str"] = "double";
                }
                else if(datatyp == XVariant::Int) {
                    livalues.push_back(v.toLongInt());
                    res["value"] = livalues;
                    res["data_type_str"] = "int";
                }
                else if(datatyp == XVariant::UInt) {
                    ulivalues.push_back(v.toULongInt());
                    res["value"] = ulivalues;
                    res["data_type_str"] = "uint";
                }
                else if(datatyp == XVariant::Boolean) {
                    bvalues.push_back(v.toBool());
                    res["value"] = bvalues;
                    res["data_type_str"] = "bool";
                }
                else if(datatyp == XVariant::String) {
                    svalues.push_back(v.toString());
                    res["value"] = svalues;
                    res["data_type_str"] = "string";
                }
                else {
                    res["err"] = true;
                    res["msg"] = "CumbiaHdbWorld::extract_data: unsupported data type from db";
                }
                if(writable == XVariant::RW) {
                    res["write_mode_str"] = "rw";
                    // extract "write" value
                    if(datatyp == XVariant::Double) {
                        dwvalues.push_back(v.toDouble(false));
                        res["w_value"] = dwvalues;
                    }
                    else if(datatyp == XVariant::Int) {
                        liwvalues.push_back(v.toLongInt(false));
                        res["w_value"] = liwvalues;
                    }
                    else if(datatyp == XVariant::UInt) {
                        uliwvalues.push_back(v.toULongInt(false));
                        res["w_value"] = uliwvalues;
                    }
                    else if(datatyp == XVariant::Boolean) {
                        bwvalues.push_back(v.toBool(false));
                        res["w_value"] = bwvalues;
                    }
                    else if(datatyp == XVariant::String) {
                        swvalues.push_back(v.toString(false));
                        res["w_value"] = swvalues;
                    }
                    else {
                        res["err"] = true;
                        res["msg"] = "CumbiaHdbWorld::extract_data: unsupported data type from db";
                    }
                } // v.getWritable == RW
            } // scalar
            else if(v.getFormat() == XVariant::Vector) {
                datafmt = XVariant::Vector;
                datatyp = v.getType();
                writable = v.getWritable();
                res["data_format_str"] = "vector";
                res["write_mode_str"] = "ro";
                if(datatyp == XVariant::Double) {
                    const std::vector<double> dv = v.toDoubleVector();
                    dvalues.insert(dvalues.end(), dv.begin(), dv.end());
                    res["data_type_str"] = "double";
                }
                else if(datatyp == XVariant::Int) {
                    const std::vector<long int> iv = v.toLongIntVector();
                    livalues.insert(livalues.end(), iv.begin(), iv.end());
                    res["data_type_str"] = "int";
                }
                else if(datatyp == XVariant::UInt) {
                    const std::vector<unsigned long int> uiv = v.toULongIntVector();
                    ulivalues.insert(ulivalues.end(), uiv.begin(), uiv.end());
                    res["data_type_str"] = "uint";
                }
                else if(datatyp == XVariant::Boolean) {
                    const std::vector<bool> biv = v.toBoolVector();
                    bvalues.insert(bvalues.end(), biv.begin(), biv.end());
                    res["data_type_str"] = "bool";
                }
                else if(datatyp == XVariant::String) {
                    const std::vector<std::string> svals = v.toStringVector();
                    svalues.insert(svalues.end(), svals.begin(), svals.end());
                    res["data_type_str"] = "string";
                }
                else {
                    res["err"] = true;
                    res["msg"] = "CumbiaHdbWorld::extract_data: unsupported data type from db";
                }
                if(writable == XVariant::RW) {
                    res["write_mode_str"] = "rw";
                    // extract "write" value
                    if(v.getType() == XVariant::Double) {
                        const std::vector<double> dv = v.toDoubleVector(false);
                        dwvalues.insert(dwvalues.end(), dv.begin(), dv.end());
                    }
                    else if(v.getType() == XVariant::Int) {
                        const std::vector<long int> iv = v.toLongIntVector(false);
                        liwvalues.insert(liwvalues.end(), iv.begin(), iv.end());
                    }
                    else if(v.getType() == XVariant::UInt) {
                        const std::vector<unsigned long int> uiv = v.toULongIntVector(false);
                        uliwvalues.insert(uliwvalues.end(), uiv.begin(), uiv.end());
                    }
                    else if(v.getType() == XVariant::Boolean) {
                        const std::vector<bool> biv = v.toBoolVector(false);
                        bwvalues.insert(bwvalues.end(), biv.begin(), biv.end());
                    }
                    else if(v.getType() == XVariant::String) {
                        const std::vector<std::string> svals = v.toStringVector(false);
                        swvalues.insert(swvalues.end(), svals.begin(), svals.end());
                    }
                }
            } // vector
            else {
                res["err"] = true;
                res["msg"] = "CumbiaHdbWorld::extract_data: only scalar and vector data is supported for the moment";
            }
        } // else if(!v.isNull())
    } // for(size_t i = 0; i < dbdata.size(); i++)

    res["has_null"] = static_cast<bool>(nulls_timestamps_us.size() > 0);
    res["null_cnt"] = static_cast<int>(nulls_timestamps_us.size());
    res["valid_cnt"] = static_cast<int>(timestamps_us.size());

    if(nulls_timestamps_us.size() > 0) {
        // valid data mixed with nulls
        res["quality"] = 4; // warning
        res["quality_color"] = std::string("yellow");
        res["quality_string"] = "warning";
    }
    else {
        res["quality"] = 0;
        res["quality_color"] = "white";
        res["quality_string"] = "valid";
    }

    if(datafmt == XVariant::Vector) {
        if(datatyp == XVariant::Double) {
            res["value"] = dvalues;
            if(writable == XVariant::RW)
                res["w_value"] = dwvalues;
        }
        else if(datatyp == XVariant::Int) {
            res["value"] = livalues;
            if(writable == XVariant::RW)
                res["w_value"] = liwvalues;
        }
        else if(datatyp == XVariant::UInt) {
            res["value"] = ulivalues;
            if(writable == XVariant::RW)
                res["w_value"] = uliwvalues;
        }
        else if(datatyp == XVariant::Boolean) {
            res["value"] = bvalues;
            if(writable == XVariant::RW)
                res["w_value"] = bwvalues;
        }
        else if(datatyp == XVariant::String) {
            res["value"] = svalues;
            if(writable == XVariant::RW)
                res["w_value"] = swvalues;
        }
        else {
            res["err"] = true;
            res["msg"] = "CumbiaHdbWorld::extract_data: unsupported spectrum data type from db";
        }
    }
}
/*!  \brief extract data from a hdbextractor Result and store it into a
 *   to a CuData bundle passed as input reference.
 *
 * \param res a Result from DbSchema::query
 * \param elapsed the time taken by the query. Will be stored under the "elapsed" key
 * \param da a reference to CuData that will contain the keys listed below in case of
 *        success.
 *
 * \par CuData bundle data *keys*
 *
 * The CuData input argument will be filled with the following keys if the DbSchema::query call
 * was successful:
 * - row_count - the number of rows fetched from the database. Use CuVariant::toULongInt (aka size_t) for conversion
 * - column_count - the number of columns selected by the query. Use CuVariant::toULongInt (aka size_t)  for conversion
 * - query - the text of the full query submitted
 * - results - a vector of strings (convert with CuData::toStringVector) containing *all the rows
 *             in sequence*, one behind the next.
 * - elapsed - a double representing the elapsed time in seconds.microseconds
 *
 * \par Access a field in the CuData "results" value
 * - let *data* be the name of the CuData input parameter to extract_data
 * - check with data["row_count"] the number of fetched rows (use toULongInt() for conversion)
 * - let size_t row be less than data["row_count"].toULongInt()
 * - let size_t col be less than data["column_count"].toULongInt()
 *
 * \code
 * size_t row = 10;
 * size_t col = 0;
 * size_t columns = da["column_count"].toULongInt();
 * // rows are enqueued in da["results"] one behind the next
 * size_t column_offset = row * columns;
 * const std::vector<std::string> values = da["results"].toStringVector();
 * std::string myvalue = values[column_offset + col];
 *
 * \endcode
*/
void CumbiaHdbWorld::extract_data(Result *res, double elapsed, CuData& da) {
    da["elapsed"] = elapsed;
    if(res) {
        int r = 0;
        da["columns"] = res->getColumnList();
        da["column_count"] = res->getColumnList().size();
        std::vector<std::string> results;
        while(res && res->next() > 0)
        {
            ++r;
            Row *row = res->getCurrentRow();
            for(int i = 0; row != NULL && i < row->getFieldCount(); i++) {
                results.push_back(std::string(row->getField(i)));
            }
        }
        da["results"] = results;
        da["row_count"] = results.size();
    }
}

std::string CumbiaHdbWorld::getDbProfilesDir() const {
    size_t pos;
    const char* hofind = "$${HOME_DIR}";
    std::string prd = std::string(PROFILES_DIR);
    pos = prd.find(hofind);
    if(pos != std::string::npos && getenv("HOME") != nullptr) {
        // getpwuid return value may point to a static area, and may be overwritten
        // by subsequent calls. (Do not pass the returned  pointer to free)
        struct passwd *pw = getpwuid(getuid());
        prd.replace(pos, strlen(hofind), std::string(pw->pw_dir));
    }
    return prd;
}

/*!
 * \brief Returns the row count of the result stored in the input parameter
 * \param da CuData obtained from a generic query expressed with a source like
 *        *hdb://query/SELECT name from table WHERE field='abc'*
 * \return the row count detected in the input parameter or 0 if the argument does
 *         not represent a valid result from a generic hdb query or if there are no
 *         rows
 */
size_t CumbiaHdbWorld::rowCount(const CuData &da) const
{
    if(da.containsKey("query") && da.containsKey("row_count"))
        return static_cast<size_t>(da["row_count"].toULongInt());
    return 0;
}

/*!
 * \brief Returns a CuData representing the row r in the result passed as argument
 * \param result CuData obtained from a generic query expressed with a source like
 *        *hdb://query/SELECT name from table WHERE field='abc'*
 * \param slices: a reference to a vector of vector of strings that will contains the
 *        slices
 * \return the number of slices or 0 if the result is invalid or empty
 *
 */
size_t CumbiaHdbWorld::slice(const CuData &result,
                             std::vector<std::vector<std::string> > & slices) const
{
    size_t rc = result["row_count"].toULongInt();
    size_t cc = result["column_count"].toULongInt();
    bool err = !result.containsKey("query") || cc <= 0 || result["err"].toBool();
    if(!err) {
        const std::vector<std::string>& rows = result["results"].toStringVector();
        for(size_t r = 0; r < rc; r++)
            slices.push_back(std::vector<std::string>(rows.begin() + cc * r, rows.begin() + cc * r + cc));
    }
    return rc;
}

/*!
 * \brief get the value under row, column from a custom query to the
 *        database, after the result has been sliced with CumbiaHdbWorld::slice
 * \param result the result from a DbSchema::query (hdbextractor lib)
 * \param slices vector of rows (one row is a vector of strings) previously sliced
 *        with CumbiaHdbWorld::slice
 * \param row the row you want to access
 * \param column_name the name of the column you want to access
 *
 * \return the value, as string, under row, column
 *
 * \note
 * The input argument must be a valid result from a DbSchema::query call.
 * A valid result will contain the following keys:
 * - row_count
 * - column_count
 * - query
 * - results
 *
 * as compiled by CumbiaHdbWorld::extract_data(Result *res, double elapsed, CuData& da)
 *
 * Check the result size greater than zero from CumbiaHdbWorld::slice before calling getValue
 */
std::string CumbiaHdbWorld::getValue(const CuData &result,
                                     const std::vector<std::vector<std::string> > & slices,
                                     size_t row,
                                     const char *column_name) const
{
    std::string value;
    int idx;
    if(!result["err"].toBool() && result.containsKey("columns")) {
        const std::vector<std::string> &cols = result["columns"].toStringVector();
        std::vector<std::string>::const_iterator it = find(cols.begin(), cols.end(), std::string(column_name));
        if(it != cols.end()) {
            idx = distance(cols.begin(), it);
            if(idx > -1 && row < slices.size() && static_cast<int>(slices[row].size()) > idx) {
                value = slices[row][idx];
            }
        }
    }
    return value;
}

bool CumbiaHdbWorld::isHdbpp(const std::string& dbnam) const {
    return dbnam == std::string("hdbpp");
}

std::string CumbiaHdbWorld::hdbppConfQuery(const std::string& att_name) const {
    return std::string("SELECT label,unit,standard_unit,display_unit,format,description,archive_rel_change,"
                       "archive_abs_change,archive_period FROM att_parameter,att_conf WHERE "
                       "att_parameter.att_conf_id=att_conf.att_conf_id AND "
                       "att_conf.att_name='" + att_name + "' ORDER BY recv_time DESC LIMIT 1");
}
