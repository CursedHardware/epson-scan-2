//
//  FKTiffEncodeUtil.h
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 2013/12/05.
//  Copyright (c) 2013年 EPSON. All rights reserved.
//
#pragma once
#include "FKDestination.h"
#include "tiffio.h"

/**
  TIFF圧縮のための基底クラス
*/
class FKTiffEncodeUtil
{
public:
	FKTiffEncodeUtil();
	virtual ~FKTiffEncodeUtil();

	BOOL StartWithPath(ESString& path, ESImageInfo& imageInfo, uint32 compression, BOOL appendMode,
	ESString& iccProfilePath, ENUM_FK_ERROR_CODE& eError,uint32 un32JpegQuality);

	virtual BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError);

	virtual BOOL FinalizeAndReturnError(ENUM_FK_ERROR_CODE& eError);
protected:
	virtual uint32_t GetRowsPerStrip(uint32_t compression);
	virtual uint32_t GetRgbColorSpaceForCompression(uint32_t compression);
	virtual BOOL StartWriting();
	virtual BOOL FinalizeWriting();
private:
	BOOL WriteScanlinesWithHeight(uint32_t height, uint32_t bytesPerLibe, uint8_t* bytes);
	BOOL WriteScanlinesWithInversePixelData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError);

	BOOL OpenOutFileWithPath(ESString* outFilePath);
	BOOL OpenOutFileWithPath(ESString* outFilePath, BOOL appendMode);
	void CloseOutFile(bool bDeleteFile = false);

	uint32_t GetPlannerConfig();
	uint32_t GetOrientation();

	uint32_t GetMonoColorSpaceForCompression(uint32_t compression);

	uint32_t GetTiffColorSpace(uint32_t samplesPerPixel, uint32_t compression);
	BOOL GetTiffSetICCProfileWithPath(ESString* iccProfilePath);

  TIFFDataType m_dataType;
	uint32_t m_nBytesPerLine;
	uint32_t m_nSamplesPerPixel;
	uint32_t m_nCurrentRow;
	ESString m_strTargetPath;
	uint32_t m_nCompressionType;

  BOOL m_bStarted;

protected:
	TIFF*        m_pOutFile;
};
