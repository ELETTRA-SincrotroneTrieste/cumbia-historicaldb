#ifndef CUMBIAHdbWORLD_H
#define CUMBIAHdbWORLD_H

#include <vector>
#include <string>

class XVariant;
class CuData;

class CumbiaHdbWorld
{
public:
    CumbiaHdbWorld();

    void setSrcPatterns(const std::vector<std::string> &pat_regex);

    std::vector<std::string> srcPatterns() const;

    bool source_valid(const std::string& s);

    void extract_data(const std::vector<XVariant> &dbdata, CuData& res) const;

    std::string getDbProfilesDir() const;

private:
    std::vector<std::string> m_src_patterns;
};

#endif // CUMBIAHdbWORLD_H
