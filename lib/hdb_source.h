#ifndef HdbSOURCE_H
#define HdbSOURCE_H

#include <string>
#include <list>
#include <cuvariant.h>

using namespace std;

class HdbSource
{
public:
    enum Type { Cmd = 0, Attr };

    HdbSource();
    HdbSource(const std::string &s);
    HdbSource(const HdbSource& other);

    string getName() const;
    string getDomain() const;

    HdbSource & operator=(const HdbSource& other);

    bool operator ==(const HdbSource &other) const;

    bool isValid() const;

    std::string error() const;

    std::string start_date() const;
    std::string stop_date() const;

private:
    string m_s, m_domain;
    std::string m_error;
    std::string m_start_dt, m_stop_dt;
    bool m_valid;
    void m_from(const HdbSource &other);
};

#endif // TSOURCE_H
