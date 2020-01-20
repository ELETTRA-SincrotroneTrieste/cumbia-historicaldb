#include "hdb_source.h"
#include <stdio.h>
#include <algorithm>
#include <regex>

HdbSource::HdbSource()
{
    m_valid = false;
}

HdbSource::HdbSource(const std::string& s)
{
    std::string dates;
    std::string delim = ",";
    std::regex re(delim);
    std::vector<std::string> ret;
    size_t pos = s.find('(');
    if(pos != string::npos)
        dates = s.substr(pos + 1, s.rfind(')') - pos - 1);
    std::sregex_token_iterator iter(dates.begin(), dates.end(), re, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        if((*iter).length() > 0)
            ret.push_back((*iter));
    if(ret.size() == 2) {
        m_start_dt = ret[0];
        m_stop_dt = ret[1];

        if(s.find("://") != std::string::npos) {
            m_s = s.substr(s.find("://") + 3, s.find('(') - s.find("://") - 3);
            m_domain = s.substr(0, s.find("://") + 3);
        }
        else {
            m_s = s.substr(0, s.find('('));
        }
    }
    else {
        pos = s.find("find/");
        if(pos != string::npos) {
            m_find_pattern = s.substr(pos + strlen("find/"));
        }
        else {
            pos = s.find("query/");
            if(pos != string::npos) {
                m_query = s.substr(pos + strlen("query/"));
            }
            else {
                m_valid = false;
                m_error = "HdbSource: did not find either (start_date,stop_date) "
                        "or find/pattern or query/QUERY within \"" + s + "\"";
            }
        }
    }
    m_valid = m_s.size() > 0 || m_find_pattern.size() > 0 || m_query.size() > 0;
    printf("HdbSource: detected domain %s src %s %s --> %s | find pattern %s | query %s \n",
           m_domain.c_str(), m_s.c_str(), m_start_dt.c_str(), m_stop_dt.c_str(),
           m_find_pattern.c_str(), m_query.c_str());
}

HdbSource::HdbSource(const HdbSource &other)
{
    m_from(other);
}

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

void HdbSource::m_from(const HdbSource &other)
{
    m_s = other.m_s;
    m_domain = other.m_domain;
    m_start_dt = other.m_start_dt;
    m_stop_dt = other.m_stop_dt;
    m_valid = other.m_valid;
    m_error = other.m_error;
    m_find_pattern = other.m_find_pattern;
    m_query = other.m_query;
}

