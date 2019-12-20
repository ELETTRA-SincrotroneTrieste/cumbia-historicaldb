#include "cuhdbactioni.h"

CuHdbActionI::CuHdbActionI()
{

}


void CuHdbActionI::onProgress(int step, int total, const CuData &data)
{
}

void CuHdbActionI::onResult(const CuData &data)
{
}

void CuHdbActionI::onResult(const std::vector<CuData> &datalist)
{
}

CuData CuHdbActionI::getToken() const
{
    return CuData();
}
