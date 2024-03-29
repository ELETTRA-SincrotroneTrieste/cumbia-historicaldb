#ifndef HdbSOURCE_H
#define HdbSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class HdbSource
{
public:
    enum Type { Invalid = -1, DataFetch = 0, FindSources, Query };

    HdbSource();
    HdbSource(const std::string &s);
    HdbSource(const HdbSource& other);

    std::string src() const;
    std::string getName() const;
    std::string getDomain() const;

    HdbSource & operator=(const HdbSource& other);

    bool operator ==(const HdbSource &other) const;

    bool isValid() const;

    std::string error() const;

    std::string start_date() const;
    std::string stop_date() const;

    std::string find_pattern() const;
    std::string query() const;

    Type getType() const;

    static const char *hdb_source_regexp();

private:
    string m_s, m_domain, m_find_pattern, m_query, m_full;
    std::string m_error;
    std::string m_start_dt, m_stop_dt;
    bool m_valid;
    void m_from(const HdbSource &other);
};

#endif // TSOURCE_H
