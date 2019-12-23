#include "cumbiahdbworld.h"
#include <xvariant.h>
#include <cudata.h>
// to get home dir
#include <pwd.h>
#include <unistd.h>
#include <string.h> // strlen
#include "cuhdb_config.h"

CumbiaHdbWorld::CumbiaHdbWorld()
{
    m_src_patterns.push_back("hdb://.+");
    m_src_patterns.push_back("hdb++://.+");
    m_src_patterns.push_back("hdbpp://.+");
}

void CumbiaHdbWorld::setSrcPatterns(const std::vector<std::string> &pat_regex)
{
    m_src_patterns = pat_regex;
}

std::vector<std::string> CumbiaHdbWorld::srcPatterns() const
{
    return m_src_patterns;
}

bool CumbiaHdbWorld::source_valid(const std::string &s)
{
    return true;
}

void CumbiaHdbWorld::extract_data(const std::vector<XVariant> &dbdata, CuData &res) const
{
    // xvariant.h: enum DataType { TypeInvalid = -1, Int, UInt, Double, Boolean, String };
    std::vector<double> timestamps, timestamps_ms;
    struct timeval tv;
    double ts;
    std::vector<double > dvalues;
    std::vector<long int> livalues;
    std::vector<unsigned long int> ulivalues;
    std::vector<bool> bvalues;
    std::vector<std::string> svalues;

    for(size_t i = 0; i < dbdata.size(); i++) {
        const XVariant& v = dbdata[i];
        // timestamps
        tv = v.getTimevalTimestamp();
        ts = static_cast<time_t>(tv.tv_sec);
        ts += tv.tv_usec * 10e-6;
        timestamps.push_back(ts);
        timestamps_ms.push_back(ts * 1000.0);
        res["time_scale"] = timestamps;
        res["time_scale_ms"] = timestamps_ms;
        // values
        if(v.getFormat() == XVariant::Scalar) {
            if(v.getType() == XVariant::Double) {
                dvalues.push_back(v.toDouble());
                res["value"] = dvalues;
            }
            else if(v.getType() == XVariant::Int) {
                livalues.push_back(v.toLongInt());
                res["value"] = livalues;
            }
            else if(v.getType() == XVariant::UInt) {
                ulivalues.push_back(v.toULongInt());
                res["value"] = ulivalues;
            }
            else if(v.getType() == XVariant::Boolean) {
                bvalues.push_back(v.toBool());
                res["value"] = bvalues;
            }
            else if(v.getType() == XVariant::String) {
                svalues.push_back(v.toString());
                res["value"] = svalues;
            }
            else {
                res["err"] = true;
                res["msg"] = "CumbiaHdbWorld::extract_data: unsupported data type from db";
            }
        }
        else {
            res["err"] = true;
            res["msg"] = "CumbiaHdbWorld::extract_data: only scalar data is supported for the moment";
        }
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
