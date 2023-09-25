#pragma once
#include <utils/ESBuffer.h>
#include <ES2Command/ES2CommandAPI.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "Utility.hpp"
#include "Error.hpp"
#include "AutoSize.hpp"

namespace epsonscan
{

class ESIMP2Wrapper;
class ESIMP2 : public Filter
{
public:

    ESIMP2(std::shared_ptr<KeyMgr> keyMgr, std::shared_ptr<ModelInfo> modelInfo);
    virtual ~ESIMP2(void);

    void Detect(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf);

    void Crop(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf);


    void DoDeficiencyCorrection(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf);

    // Dummy function
    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
    {

    }


    std::string Name() override
    {
        return "ESIMP2";
    }

    static std::string GetESIMP2Path();

    static bool IsESIMP2Available();

    bool IsCarrerSheetCrop(ESImageInfo& imageInfo);

private:
    void* locationData_;
};

}