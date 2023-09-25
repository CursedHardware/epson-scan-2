//----------------------------------------=----------------------------------------┐
/*!@file
@brief		画像情報格納ディクショナリ　アクセッサクラス
			ScannerDriverEngine,FileFormatKit,ImageProcessorで利用する
@par		Copyright:
			Copyright (C) SEIKO EPSON CORPORATION 2014. All rights reserver. <BR>
@date		2015.3.13
*/
#pragma once

#include <assert.h>
#include "ESCommonTypedef.h"
#include "../Include/IESBuffer.h"
#include "utils/ESAnyCastUtils.h"
#include "../CommonUtility/utils/ESJsonUtils.h"

typedef ESFloatMultiArray  ESProfileMatrix;// 3x3 Array eg. @[@[1,0,0], @[0,1,0], @[0,0,1]]
typedef ESFloatMultiArray  ESGrayReproductionMatrix;// 3x3 Array eg. @[@[1,0,0], @[0,1,0], @[0,0,1]]

typedef ESDictionary        ESGammaTable;
typedef ESDictionary		ESMutableGammaTable;
STATIC_CONST_KEY kESGammaTableMaxKey         = ES_STRING("gammaTabeType");
typedef enum
{
    kESGammaTableMax256 = 256,
} ESGammaTableMax;

STATIC_CONST_KEY kESGammaTableRedChannelKey   = ES_STRING("redChannel");	//ESIndexArray
STATIC_CONST_KEY kESGammaTableGreenChannelKey = ES_STRING("greenChannel");	//ESIndexArray
STATIC_CONST_KEY kESGammaTableBlueChannelKey  = ES_STRING("blueChannel");	//ESIndexArray
STATIC_CONST_KEY kESGammaTableMonoChannelKey  = ES_STRING("monoChannel");	//ESIndexArray

STATIC_CONST_KEY kESImageInfoArray = ES_STRING("imageInfos");	//ImageInfoArray��JSON�ɕێ������ێg�p

static ESIndexArray getESGammaTableRBytes(const ESGammaTable& gammaTable)
{
	return SAFE_ANY_VALUE_FROM_DICT(gammaTable,kESGammaTableRedChannelKey,ESIndexArray,ESIndexArray());
}
static ESIndexArray getESGammaTableGBytes(const ESGammaTable& gammaTable)
{
	return SAFE_ANY_VALUE_FROM_DICT(gammaTable,kESGammaTableGreenChannelKey,ESIndexArray,ESIndexArray());
}
static ESIndexArray getESGammaTableBBytes(const ESGammaTable& gammaTable)
{

	return SAFE_ANY_VALUE_FROM_DICT(gammaTable,kESGammaTableBlueChannelKey,ESIndexArray,ESIndexArray());
}
static ESIndexArray getESGammaTableMonoBytes(const ESGammaTable& gammaTable)
{
	return SAFE_ANY_VALUE_FROM_DICT(gammaTable,kESGammaTableMonoChannelKey,ESIndexArray,ESIndexArray());
}
static void getESGammaTableRGBData(ES_CMN_FUNCS::BUFFER::IESBuffer& cBufDest,const ESGammaTable& dictGammaTable){

	cBufDest.FreeBuffer();

	ESIndexArray arR = getESGammaTableRBytes(dictGammaTable);
	ESIndexArray arG = getESGammaTableGBytes(dictGammaTable);
	ESIndexArray arB = getESGammaTableBBytes(dictGammaTable);

	if(arR.size() + arG.size() + arB.size() != kESGammaTableMax256*3){
		//invalid table size.
		return;
	}
	if(!cBufDest.AllocBuffer( kESGammaTableMax256*3)){
		return;
	}
	UInt8* pTable = cBufDest.GetBufferPtr();

	for(size_t nLoop=0;nLoop<arR.size();nLoop++){
		UInt8 un8Data = arR[nLoop];
		*pTable = un8Data;
		pTable++;
	}

	for(size_t nLoop=0;nLoop<arG.size();nLoop++){
		UInt8 un8Data = arG[nLoop];
		*pTable = un8Data;
		pTable++;
	}

	for(size_t nLoop=0;nLoop<arB.size();nLoop++){
		UInt8 un8Data = arB[nLoop];
		*pTable = un8Data;
		pTable++;
	}

}

typedef std::map<ESString,ESNumber> ESImageInfo;

typedef std::deque<ESImageInfo> ESImageInfoArray;

namespace ES_IMAGE_INFO{

	// 画像情報の基本表現
	STATIC_CONST_KEY kESImageInfoWidthKey				= ES_STRING("width");				//ESNumber
	STATIC_CONST_KEY kESImageInfoHeightKey			= ES_STRING("height");				//ESNumber
	STATIC_CONST_KEY kESImageInfoSamplesPerPixelKey	= ES_STRING("samplesPerPixel");	//ESNumber
	STATIC_CONST_KEY kESImageInfoBitsPerSampleKey		= ES_STRING("bitsPerSample");		//ESNumber
	STATIC_CONST_KEY kESImageInfoResolutionXKey		= ES_STRING("resolutionX");		//ESNumber 主走査方向解像度
	STATIC_CONST_KEY kESImageInfoResolutionYKey		= ES_STRING("resolutionY");		//ESNumber 副走査方向解像度
	STATIC_CONST_KEY kESImageInfoOutputResolutionKey  = ES_STRING("outputResolution");   //ESNumber 出力解像度

	// 表面、裏面の情報
	STATIC_CONST_KEY kESImageInfoSurfaceTypeKey		= ES_STRING("SurfaceType");		//ESNumber


	ESNumber GetImageInfoValueForKey(const ESImageInfo& info,ESString strKey);

	void GetImageInfoAsDict(ESDictionary& dict,const ESImageInfo& imageInfo);
	void GetImageInfoAsJson(ESString& strImageInfoAsJson,const ESImageInfo& srcImageInfo);
	void GetImageInfoArrayAsJson(ESString& strImageInfoArrayAsJson,ESImageInfoArray& srcImageInfoArray);

	void MakeImageInfoFromDict(ESImageInfo& imageInfoDest,ESDictionary dictImageInfo);
	void MakeImageInfoFromJson(ESImageInfo& imageInfoDest,ESString strImageInfoAsJson);
	void MakeImageInfoArrayAsDicArray(ESDicArray& arDicImageInfos,const ESImageInfoArray& arImageInfos);
	void MakeImageInfoArrayFromDicArray(ESImageInfoArray& arDestImageInfos, const ESDicArray& arDicImageInfos);
	void MakeImageInfoArrayFromJson(ESImageInfoArray& arDestImageInfos,const ESString& strImageInfosAsJson);

	enum ESImageInfoSurfaceType{
		kESImageInfoSurfaceTypeFrontSide = 0,
		kESImageInfoSurfaceTypeBackSide
	};

	// デジタルアイスの原稿種
	STATIC_CONST_KEY kESImageInfoDigitalICETypeKey			= ES_STRING("DigitalICEType");//ESNumber

	typedef enum
	{
		kESImageInfoDigitalICETypeNone,
		kESImageInfoDigitalICETypeTransparent,
		kESImageInfoDigitalICETypeTransparentIR,
		kESImageInfoDigitalICETypeFlatbedLamp1,
		kESImageInfoDigitalICETypeFlatbedLamp2
	} ESImageInfoDigitalICEType;

	// スキャンを一意に識別する番号
	STATIC_CONST_KEY kESImageInfoScanCountKey			= ES_STRING("scanCount");//ESNumber

	// 一つのスキャンでページを一意に識別する番号
	STATIC_CONST_KEY kESImageInfoPageCountKey			= ES_STRING("pageCount");//ESNumber

	// 一つのスキャンで用紙を一意に識別する番号(両面スキャンの場合に表裏で同じ値)
	STATIC_CONST_KEY kESImageInfoPaperCountKey		= ES_STRING("paperCount");//ESNumber

	// 画像のエンコード方式
	STATIC_CONST_KEY kESImageInfoDataTypeKey			= ES_STRING("dataType");//ESNumber


	typedef enum {
		kESImageDataTypeRaw = 0,
		kESImageDataTypeJpeg
	}ESImageDataType;

	// 原稿のキャリアシート情報
	STATIC_CONST_KEY kESImageInfoCarrierSheetTypeKey	= ES_STRING("carrierSheet");//ESNumber

	typedef enum
	{
		kESImageInfoCarrierSheetTypeOFF,
		kESImageInfoCarrierSheetTypeON,
	} ESImageInfoCarrierSheetType;

	/*
	 画像が期待される向きとは上下逆になっているかどうかを示す(BOOL)
	*/
	STATIC_CONST_KEY kESImageInfoUpsideDownKey = ES_STRING("upsideDown");


	/*
	 原稿がCropされているかどうかを示す情報 (BOOL)
	*/
	STATIC_CONST_KEY kESImageInfoAutoCroppedKey = ES_STRING("autoCropped");	//ESNumber

	/*
	 原稿が白紙かどうかを示す情報 (BOOL)
	*/
	STATIC_CONST_KEY kESImageInfoBlankPageKey = ES_STRING("blankpage");	//ESNumber

	/*
	 原稿がモノクロかどうかを示す情報 (BOOL)
	*/
	STATIC_CONST_KEY kESImageInfoMonoPageKey = ES_STRING("monopage");	//ESNumber

	typedef enum {
	  kESImageColorTypeRGB,
	  kESImageColorTypeRGBA,
	  kESImageColorTypeRGB16,
	  kESImageColorTypeRGBA16,
	  kESImageColorTypeRGBJpeg,
	  kESImageColorTypeGray,
	  kESImageColorTypeGray16,
	  kESImageColorTypeGrayJpeg,
	  kESImageColorTypeMono,
	  kESImageColorTypeOthers,
	}ESImageColorType;

	ESNumber GetESImageResolutionX(const ESImageInfo& imageInfo);

	ESNumber GetESImageResolutionY(const ESImageInfo& imageInfo);

	ESNumber GetESImageOutputResolution(const ESImageInfo& imageInfo);

	ESNumber GetESImageBitsPerSample(const ESImageInfo& imageInfo);

	ESNumber GetESImageSamplesPerPixel(const ESImageInfo& imageInfo);

	ESNumber GetESImageBitsPerPixel(const ESImageInfo& imageInfo);

	ESNumber GetESImageHeight(const ESImageInfo& imageInfo);

	ESNumber GetESImageWidth(const ESImageInfo& imageInfo);

	ESNumber IsESImageEmpty(const ESImageInfo& imageinfo);

	ESNumber GetESImageBytesPerRow(const ESImageInfo& imageInfo);

	ESNumber GetESImageBytesPerCol(const ESImageInfo& imageInfo);

	ESNumber GetESImagePixelsPerRow(const ESImageInfo& imageInfo);

	ESNumber GetESImageSize(const ESImageInfo& imageInfo);

	UInt32 GetESImageSizeUN(const ESImageInfo& imageInfo);

	ESNumber GetESImagePageCount(const ESImageInfo& imageInfo);

	ESNumber GetESImagePaperCount(const ESImageInfo& imageInfo);

	ESNumber GetESImageScanCount(const ESImageInfo& imageInfo);

	ESImageDataType GetESImageDataType(const ESImageInfo& imageInfo);

	ESImageInfoSurfaceType GetESImageSurfaceType(const ESImageInfo& imageInfo);

	ESImageInfoSurfaceType GetESImageOthersideSurfaceType(const ESImageInfo& imageInfo);

	ESImageInfoCarrierSheetType GetESImageCarrierSheetType(const ESImageInfo& imageInfo) ;
	BOOL GetESImageUpsideDown(const ESImageInfo& imageInfo);

	BOOL GetESImageAutoCropped(const ESImageInfo& imageInfo);

	ESImageColorType GetESImageColorType(const ESImageInfo& imageInfo);

	bool GetESImageBlankPageSkip(const ESImageInfo& imageInfo);

	bool GetESImageMonoPage(const ESImageInfo& imageInfo);

  /*
   グレー背景値の定義

   @{
       @"front" : @{
                       @"red"       : @(10),
                       @"green"     : @(50),
                       @"blue"      : @(20),
                       @"range"     : @(10),
                       @"mono"      : @(10),
                       @"monoRange" : @(10)
                    },
        @"back" : @{
                       @"red"       : @(10),
                       @"green"     : @(50),
                       @"blue"      : @(20),
                       @range"      : @(10),
                       @"mono"      : @(10),
                       @"monoRange" : @(10)
                   }
   }
   */
	typedef ESDictionary ESBackgroundColor;
	STATIC_CONST_KEY  kESBackgroundColorFrontKey     = ES_STRING("front");
	STATIC_CONST_KEY  kESBackgroundColorBackKey      = ES_STRING("back");
	STATIC_CONST_KEY  kESBackgroundColorRedKey       = ES_STRING("red");        // R
	STATIC_CONST_KEY  kESBackgroundColorGreenKey     = ES_STRING("green");      // G
	STATIC_CONST_KEY  kESBackgroundColorBlueKey      = ES_STRING("blue");       // B

	STATIC_CONST_KEY  kESBackgroundColorRedRangeKey  = ES_STRING("redRange");   // 範囲
	STATIC_CONST_KEY  kESBackgroundColorGeenRangeKey = ES_STRING("greenRange"); // 範囲
	STATIC_CONST_KEY  kESBackgroundColorBlueRangeKey = ES_STRING("blueRange");  // 範囲
	STATIC_CONST_KEY  kESBackgroundColorMonoKey      = ES_STRING("mono");       // Mono
	STATIC_CONST_KEY  kESBackgroundColorMonoRangeKey = ES_STRING("monoRange");  // 範囲

}
