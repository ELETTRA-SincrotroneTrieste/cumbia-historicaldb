#ifndef CUMBIAHdbWORLD_H
#define CUMBIAHdbWORLD_H

#include <vector>
#include <string>

class XVariant;
class CuData;
class Result;

class CumbiaHdbWorld
{
public:
    CumbiaHdbWorld();

    void setSrcPatterns(const std::vector<std::string> &pat_regex);

    std::vector<std::string> srcPatterns() const;

    bool source_valid(const std::string& s);

    void extract_data(const std::vector<XVariant> &dbdata, CuData& res) const;

    void extract_data(Result *res, double elapsed, CuData &da);

    std::string getDbProfilesDir() const;

    size_t rowCount(const CuData& da) const;

    size_t slice(const CuData& result, std::vector<std::vector<std::string> > &slices) const;

    std::string getValue(const CuData &result,
                         const std::vector<std::vector<std::string> > & slices,
                         size_t row,
                         const char *column_name) const;

private:
    std::vector<std::string> m_src_patterns;
};

#endif // CUMBIAHdbWORLD_H
