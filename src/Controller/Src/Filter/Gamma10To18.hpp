#pragma once
#include <utils/ESBuffer.h>
#include <ES2Command/ES2CommandAPI.h>
#include <math.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "ModelInfo.hpp"
#include "Filter.hpp"
#include "Utility.hpp"
#include "Error.hpp"
#include "AutoSize.hpp"
#include "ColorType.hpp"

namespace epsonscan
{

#define INVERSEGAMMAREVISION(x,y)	(255.*pow(((x)/255.), y))

class Gamma10To18 : public Filter
{
public:

    Gamma10To18(std::shared_ptr<KeyMgr> keyMgr,
                     std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
    {   

    }

    virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
    {
        ColorType* colorType =(ColorType*)(keyMgr_->GetKeyInstance(kSDIColorTypeKey).get());   
        AutoSize* autoSize =(AutoSize*)(keyMgr_->GetKeyInstance(kSDIAutoSizeKey).get());   

        if (colorType->GetScanGammaMode() == kESGammaMode10 && colorType->GetSaveGammaMode() == kESGammaMode18)
        {
            uint8_t rTable[256];
            uint8_t gTable[256];
            uint8_t bTable[256];

            ESNumber nImageWidth      = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
            ESNumber nImggeHeight     = ES_IMAGE_INFO::GetESImageHeight(imageInfo);
            ESNumber nSamplesPerPixel = ES_IMAGE_INFO::GetESImageSamplesPerPixel(imageInfo);
            ESNumber nBytesPerRow     = ES_IMAGE_INFO::GetESImageBytesPerRow(imageInfo);

            // processr inverse reduce brightness
            if (autoSize->IsAdvancedSotwareAutoCropEnabled())
            {
			    ESNumber reduceBrightness = 11111;
			    modelInfo_->GetValue(kPrvHRD_ReduceBirightness, reduceBrightness);

                SDIFunctionalUnit functionalUnit = (SDIFunctionalUnit)keyMgr_->GetValueInt(kSDIFunctionalUnitKey);
                if (functionalUnit == kSDIFunctionalUnitFlatbed)
                {
                    for (int32_t i = 0; i < 256; i++) {
                        double dNewValue = pow((double)reduceBrightness / 10000, 1.0) * i;
                        int32_t    nClipNewValue = (std::min)((std::max)(int32_t(dNewValue), 0), 255);
                        rTable[i] = nClipNewValue;
                        gTable[i] = nClipNewValue;
                        bTable[i] = nClipNewValue;
                    }
                }else{
                    for (int32_t i = 0; i < 256; i++) {
                        rTable[i] = i;
                        gTable[i] = i;
                        bTable[i] = i;
                    }
                }
            }   

            UInt8* pInBytes = inDataBuf.GetBufferPtr();
    	    ES_IMAGE_INFO::ESImageColorType eColorType = ES_IMAGE_INFO::GetESImageColorType(imageInfo);

            for (ESNumber i = 0; i < nImggeHeight; i++) {
                for( ESNumber j = 0; j < nImageWidth; j++ ) {    

                    uint32_t nIndex =  nBytesPerRow * i + j * nSamplesPerPixel;
                
                    if( eColorType == ES_IMAGE_INFO::kESImageColorTypeRGB ) {
                        pInBytes[nIndex]     = rTable[pInBytes[nIndex]];
                        pInBytes[nIndex + 1] = gTable[pInBytes[nIndex + 1]];
                        pInBytes[nIndex + 2] = bTable[pInBytes[nIndex + 2]];
                    }else if( eColorType == ES_IMAGE_INFO::kESImageColorTypeGray ) {
                        pInBytes[nIndex]     = gTable[pInBytes[nIndex]];
                    }else{
                        assert(FALSE);
                    }
                 }
           }


            //Gamma 10 to 18
            for (int i = 0; i < 256; i++)
            {
                rTable[i] = INVERSEGAMMAREVISION(i, 1.0/1.8);
                gTable[i] = INVERSEGAMMAREVISION(i, 1.0/1.8);
                bTable[i] = INVERSEGAMMAREVISION(i, 1.0/1.8);
            }

            for (ESNumber i = 0; i < nImggeHeight; i++) {
                for( ESNumber j = 0; j < nImageWidth; j++ ) {    

                    uint32_t nIndex =  nBytesPerRow * i + j * nSamplesPerPixel;
                
                    if( eColorType == ES_IMAGE_INFO::kESImageColorTypeRGB ) {
                        pInBytes[nIndex]     = rTable[pInBytes[nIndex]];
                        pInBytes[nIndex + 1] = gTable[pInBytes[nIndex + 1]];
                        pInBytes[nIndex + 2] = bTable[pInBytes[nIndex + 2]];
                    }else if( eColorType == ES_IMAGE_INFO::kESImageColorTypeGray ) {
                        pInBytes[nIndex]     = gTable[pInBytes[nIndex]];
                    }else{
                        assert(FALSE);
                    }
                 }
           }
            
        }

    }


    std::string Name() override
    {
        return "Gamma10To18";
    }


private:
                                      
};

}