#pragma once

#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "Utility.hpp"

namespace epsonscan
{

extern void BuildGammaTable(int				brightness,
		    				int				contrast,
						    int				gamma,	//gamma x 100 (0.5->50)
						    unsigned char* GammaR,
                            unsigned char* GammaG,
						    unsigned char* GammaB);

class Lut : public Filter
{
public:
           Lut(std::shared_ptr<KeyMgr> keyMgr,
                      std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo), autoJudgedColor_(true)
            {   

            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override;

            static bool isFitPluginAvailable();

            bool isSKipLut();

            void SetAutoJudgedColor(bool autoJudgedColor)
            {
                autoJudgedColor_ = autoJudgedColor;
            }

protected:


            std::string Name() override
            {
                return "Lut";
            }

private:
    bool autoJudgedColor_;

	static void ProcessLUTUsingFit(ESImageInfo& imageInfo, 
					ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
					ESNumber brightness, 
					ESNumber contrast,
                    ESNumber gamma, 	//gamma x 100 (0.5->50)
                    SDIBackgroundRemoval bgRemoval,
                    std::string workFolder
                    );

	static void ProcessLUT(ESImageInfo& imageInfo, 
					ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                    std::shared_ptr<KeyMgr> keyMgr,
                    std::shared_ptr<ModelInfo> modelInfo,
                    bool autoJudgedColor
                    );

      static void ProcessLUT(ESImageInfo& imageInfo, 
					ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
                    unsigned char* gammaR,
                    unsigned char* gammaG,
                    unsigned char* gammaB 
                    );
};

}