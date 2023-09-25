#pragma once
#include <utils/ESBuffer.h>
#include "ESCommonTypedef.h"
#include "ESImageInfo.h"
#include "EpsonScan2.h"
#include "Filter.hpp"
#include "KeyMgr.hpp"
#include "ModelInfo.hpp"

namespace epsonscan
{

enum EIPRotateType : ESNumber
{
	kEIPRotateType90  = 90,
	kEIPRotateType180 = 180,
	kEIPRotateType270 = 270
};

//---------------------------------------------------------------------------------------
// 90単位の画像回転を行うユーティリティクラス
//---------------------------------------------------------------------------------------
class ProcOrientation : public Filter
{
public:

           ProcOrientation(std::shared_ptr<KeyMgr> keyMgr,
                     	   std::shared_ptr<ModelInfo> modelInfo) : Filter(keyMgr, modelInfo)
            {   
            }

            virtual void DoProcess(ESImageInfo& imageInfo, ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf) override
            {
					if(orientation_ != kSDIOrientationNone)
					{
						if(RotateImage(imageInfo, inDataBuf, orientation_) == false)
						{
							throw std::bad_alloc();
						}
					}
            }

			void SetOrientation(SDIOrientation orientation)
			{
				orientation_ = orientation;
			}

private:

	bool RotateImage(ESImageInfo& imageInfo, 
					 ES_CMN_FUNCS::BUFFER::CESHeapBuffer& inDataBuf,
					 SDIOrientation orientation
					 )
	{
		ES_CMN_FUNCS::BUFFER::CESHeapBuffer dest;
		SDIError error = kSDIErrorNone;

		switch(orientation)
		{
			case kSDIOrientation90:
			{
				RotateImageOnMem(dest, kEIPRotateType90 , imageInfo, inDataBuf , error);
				ESNumber widthBefore = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
				imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] =   ES_IMAGE_INFO::GetESImageHeight(imageInfo);
				imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] =   widthBefore;
			}
			break;

			case kSDIOrientation180:
			{
				RotateImageOnMem(dest, kEIPRotateType180 , imageInfo, inDataBuf , error);
			}
			break;

			case kSDIOrientation270:
			{
				RotateImageOnMem(dest, kEIPRotateType270 , imageInfo, inDataBuf , error);
				ESNumber widthBefore = ES_IMAGE_INFO::GetESImageWidth(imageInfo);
				imageInfo[ES_IMAGE_INFO::kESImageInfoWidthKey] =   ES_IMAGE_INFO::GetESImageHeight(imageInfo);
				imageInfo[ES_IMAGE_INFO::kESImageInfoHeightKey] =   widthBefore;
			}
			break;

			case kSDIOrientationNone:
			break;
		}

		inDataBuf.FreeBuffer();
		inDataBuf.Attach(dest);

		return error == kSDIErrorNone;
	}

	std::string Name() override
    {
    	 return "ProcOrientation";
    }

	//---------------------------------------------------------------------------------------
    // 90単位の画像回転を行う。出力画像バッファーを生成する。
	// バッファーの複製が不要な場合には入力バッファーをdetachする
   //---------------------------------------------------------------------------------------
	static void  RotateImageOnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
								 EIPRotateType eRotateType,
								 const ESImageInfo& dictImageInfo,
							  	 ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
								 SDIError& eOutError);

	//---------------------------------------------------------------------------------------
    // 90度の回転を行う。
   //---------------------------------------------------------------------------------------
	static void  RotateImageBy90OnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
  		  							   const ESImageInfo& dictImageInfo,
								  	   ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
									   BOOL bClockwise,
									   SDIError& eOutError);

	//---------------------------------------------------------------------------------------
    // 180度の回転を行う。
   //---------------------------------------------------------------------------------------
	static void  RotateImageB1800OnMem(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,
			  					      const ESImageInfo& dictImageInfo,
								 	  ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cInData,
									  SDIError& eOutError);

	SDIOrientation orientation_ = kSDIOrientationNone;
};

}
