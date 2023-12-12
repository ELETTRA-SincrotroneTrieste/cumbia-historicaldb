#include "hdb_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

// srv-tango-srf.fcs.elettra.trieste.it:20000/sl/feedback/fb_opa_energy_fel01.01/pidvalues( 2019-06-03 16:00:59)
// srv-tango-srf.fcs.elettra.trieste.it:20000/sl/feedback/fb_opa_energy_fel01.01/pidvalues( 2019-06-03 16:00:59, 2019-06-03 23:30:00)
// (hdb://){0,1}([A-Za-z0-9_\-\./\:]+)\(\s*(\d{4}[\-]\d{2}[\-]\d{2}\s+\d{2}:\d{2}:\d{2}\s*)\s*(?:,\s*(\d{4}[\-]\d{2}[\-]\d{2}\s+\d{2}:\d{2}:\d{2}\s*)\s*){0,1}\)
static const char* hdb_src_regex = "(hdb://){0,1}([A-Za-z0-9_\\-\\./\\:]+)\\(\\s*(\\d{4}[\\-]\\d{2}[\\-]\\d{2}\\s+\\d{2}:\\d{2}:\\d{2}\\s*)\\s*(?:,\\s*(\\d{4}[\\-]\\d{2}[\\-]\\d{2}\\s+\\d{2}:\\d{2}:\\d{2}\\s*)\\s*){0,1}\\)";

HdbSource::HdbSource() {
    m_valid = false;
}

/*!
 * \brief HdbSource constructor.
 *
 * \par Accepted syntax
 *
 * - [hdb://]domain/family/device/attribute(start_date, stop_date)
 *   *start_date*: YYYY-MM-dd hh:mm:ss e.g. 2019-06-21 10:00:00
 *   *stop_date*: YYYY-MM-dd hh:mm:ss e.g. 2019-06-21 10:00:00
 *   Get data within start_date and stop_date
 *
 * - [hdb://]domain/family/device/attribute(date)
 *   *date*  YYYY-MM-dd hh:mm:ss e.g. 2019-06-21 10:00:00
 *   Get the value at the given date in the past
 *
 * - hdb://find/pattern
 *   find sources matching pattern
 *
 * - hdb://query/sql_query
 *   perform the specified query to the database
 *
 * \param s a string conforming to the accepted forms
 */
HdbSource::HdbSource(const std::string& s) {
    m_full = s;
    std::string expr(hdb_src_regex);
    std::regex re(expr);
    std::smatch hdb_src_match;

    if(std::regex_match(s, hdb_src_match, re)) {
        if (hdb_src_match.size() > 4) {
            std::sub_match src = hdb_src_match[2];
            m_s = src.str();
            m_domain = hdb_src_match[1].str();
            m_start_dt = hdb_src_match[3].str();
            m_stop_dt = hdb_src_match[4].str();
            if(m_stop_dt.size() == 0)
                m_stop_dt = m_start_dt;
        }
    }
    else {
        size_t pos = s.find("find/");
        if(pos != string::npos) {
            m_find_pattern = s.substr(pos + strlen("find/"));
            m_s = s;
        }
        else {
            pos = s.find("query/");
            if(pos != string::npos) {
                m_query = s.substr(pos + strlen("query/"));
                m_s = s;
            }
            else {
                m_valid = false;
                m_error = "HdbSource: did not find either (start_date,stop_date) "
                        "or find/pattern or query/QUERY within \"" + s + "\"";
            }
        }
    }
    m_valid = m_s.size() > 0 || m_find_pattern.size() > 0 || m_query.size() > 0;
}

HdbSource::HdbSource(const HdbSource &other)
{
    m_from(other);
}

string HdbSource::src() const {
    return m_full;
}

/*!
 * \brief get the complete source, including the domain, "hdb://", if specified
 * \return the complete source passed to the HdbSource constructor
 */
string HdbSource::getName() const
{
    return m_s;
}

string HdbSource::getDomain() const
{
    return m_domain;
}

HdbSource &HdbSource::operator=(const HdbSource &other)
{
    if(this != &other) {
        m_from(other);
    }
    return *this;
}

bool HdbSource::operator ==(const HdbSource &other) const
{
    return m_s == other.m_s && m_domain == other.m_domain
            && m_start_dt == other.m_start_dt && other.m_stop_dt == m_stop_dt;
}

bool HdbSource::isValid() const
{
    return m_valid;
}

string HdbSource::error() const {
    return m_error;
}

string HdbSource::start_date() const
{
    return m_start_dt;
}

string HdbSource::stop_date() const {
    return m_stop_dt;
}

string HdbSource::find_pattern() const {
    return m_find_pattern;
}

string HdbSource::query() const {
    return m_query;
}

HdbSource::Type HdbSource::getType() const
{
    if(m_find_pattern.size() > 0)
        return HdbSource::FindSources;
    else if(m_start_dt.size() > 0 && m_stop_dt.size() > 0)
        return HdbSource::DataFetch;
    else if(m_query.size() > 0)
        return HdbSource::Query;
    return HdbSource::Invalid;
}

const char* HdbSource::hdb_source_regexp() {
    return hdb_src_regex;
}

void HdbSource::m_from(const HdbSource &other)
{
    m_full = other.m_full;
    m_s = other.m_s;
    m_domain = other.m_domain;
    m_start_dt = other.m_start_dt;
    m_stop_dt = other.m_stop_dt;
    m_valid = other.m_valid;
    m_error = other.m_error;
    m_find_pattern = other.m_find_pattern;
    m_query = other.m_query;
}

