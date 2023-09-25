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

enum ActionTypeDDE : ESNumber
{
    kActionTypeADE,
    kActionTypeBin
};

class DDEFilter : public Filter
{
public:

    DDEFilter(std::shared_ptr<KeyMgr> keyMgr,
          std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), 
                                                  actionType_(kActionTypeADE)
    {   

    }

    void SetActionType(ActionTypeDDE type)
    {
        actionType_ = type;
    }

    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override;


    std::string Name() override
    {
        return "TextEnhanceFilter";
    }

    static bool IsDDEAvailable();

private:

    ActionTypeDDE actionType_;
};
}