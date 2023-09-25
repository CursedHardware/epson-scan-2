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

enum ActionType : ESNumber
{
    kActionTypeGetDocumentLocation,
    kActionTypeJudgeBlankPage,
    kActionTypeJudgeColorType,
    kActionTypeRemovePunchHole
};

class DTR : public Filter
{
public:

    DTR(std::shared_ptr<KeyMgr> keyMgr,
          std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), 
                                                  actionType_(kActionTypeGetDocumentLocation),
                                                  isBlankPage_(false),
                                                  isColor_(true),
                                                  autoColorPixel_(kSDIAutoColorPixelTypeMono)
    {   

    }

    void SetActionType(ActionType type)
    {
        actionType_ = type;
    }

    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override;

    bool IsBlankPage()
    {
        return isBlankPage_;
    }

    bool IsColorPage()
    {
        return isColor_;
    }

    SDIAutoColorPixelType GetAutoColorPixelType()
    {
        return autoColorPixel_;
    }


    std::string Name() override
    {
        return "DTR";
    }

    static bool IsDTRAvailable();

private:
	static SDIError DoAutoCrop(ESImageInfo& imageInfo, 
				               ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                               std::shared_ptr<ModelInfo> modelInfo,
                               std::string workFolder
                               );

    static bool JudgeBlankPage(ESImageInfo& imageInfo, 
				               ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                               std::shared_ptr<ModelInfo> modelInfo,
                               ESNumber skipLevel,
                               std::string workFolder);


    static bool JudgeColorType(ESImageInfo& imageInfo, 
				               ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                               std::shared_ptr<ModelInfo> modelInfo,
                               std::shared_ptr<KeyMgr> keyMgr,
                               std::string workFolder,
                               SDIAutoColorPixelType &autoColorPiexelType
                               );

    static SDIError DoRemovePunchHole(ESImageInfo& imageInfo, 
				                      ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                                      std::shared_ptr<ModelInfo> modelInfo,
                                      ESNumber removeArea,
                                      std::string workFolder);
                                      
    ActionType actionType_;
    bool       isBlankPage_;
    bool       isColor_;
    SDIAutoColorPixelType autoColorPixel_;
};
}