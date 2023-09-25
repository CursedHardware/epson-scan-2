//
//  FKJpegUtil.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 13/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once
#define HAVE_BOOLEAN
typedef int boolean;
#include "jpeglib.h"
#include "FileFormatKitErrors.h"
#include "jpegUserDefines.h"

//---------------------------------------------------------------------------------------
//  JPEG解凍のためのユーティリティクラス　libJpegとのインターフェイス
//--------------------------------------------------------------------------------------
class CFKJpegDecodeUtil
{
private:
	struct jpeg_decompress_struct	m_stCinfo;
	struct my_error_mgr			m_stJerr;

	BOOL					m_bHeaderRead;
    BOOL					m_bStart;

public:
	CFKJpegDecodeUtil();
	~CFKJpegDecodeUtil();

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer	m_cJpegData;

	jpeg_decompress_struct* GetJpegDecompressInfo() {
		return &m_stCinfo;
	}

	BOOL IsStart() {
		return m_bStart;
	}

	BOOL StartDecodingAndReturnError( ENUM_FK_ERROR_CODE& eError );

	void AppendData( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cData );

	BOOL DecodedDataAndReturnError(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData, ENUM_FK_ERROR_CODE& eError ,ESNumber nValidHeight,BOOL bReadOnlyValidData);

	BOOL FinalizeDecodingAndReturnError( ENUM_FK_ERROR_CODE& eError );

	BOOL DecodeJpegDataWithResize(const ES_CMN_FUNCS::BUFFER::IESBuffer& cSrcJpegData,ES_CMN_FUNCS::BUFFER::IESBuffer& cDestResizedRawData, ST_ES_SIZE_UN32& stRequestSize,ESString& outImageInfoAsJsonn);


private:
	ENUM_FK_ERROR_CODE jpegStartDecompress();
	BOOL scanJpegLines(ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cDestData,ESNumber nValidHeight,BOOL bReadOnlyValidData,ENUM_FK_ERROR_CODE& eError);
};
