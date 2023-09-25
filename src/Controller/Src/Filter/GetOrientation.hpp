#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "Utility.hpp"
//#include "Saving.hpp"

namespace epsonscan
{
    class GetOrientation : public Filter
    {
    public:

        GetOrientation(std::shared_ptr<KeyMgr> keyMgr,
                        std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
        {   

        }

        virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override;
        
        std::string Name() override
        {
            return "GetOrientation";
        }

        static bool IsPluginAvailable();

        SDIOrientation GetJudgeResult()
        {
            return judgeResult_;
        }

    private:
        SDIOrientation judgeResult_ = kSDIOrientationNone;
    };
}