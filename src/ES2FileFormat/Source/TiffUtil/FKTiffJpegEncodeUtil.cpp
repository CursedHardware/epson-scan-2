//
//  FKTiffEncodeUtil.m
//  ESImageFormatKit
//
//  Created by Yumoto Atsushi on 2013/12/05.
//  Copyright2013 EPSON. All rights reserved.
//

#include "stdafx.h"
#include "FKTiffJpegEncodeUtil.h"
#include "FileFormatKit.h"
#include "FileFormatKitErrors.h"
#include "ESImageInfo.h"
#include "ESFile.h"

FKTiffJpegEncodeUtil::FKTiffJpegEncodeUtil()
{
}
FKTiffJpegEncodeUtil::~FKTiffJpegEncodeUtil()
{
}

uint32_t FKTiffJpegEncodeUtil::GetRowsPerStrip(uint32_t compression)
{

	uint32_t height = 0;
	if (TIFFGetField(m_pOutFile, TIFFTAG_IMAGELENGTH, &height) != 1) {
	//	ESlog(kESLogLevelError, self, @"TIFFGetField fails");
	}

	return height;
}


uint32_t FKTiffJpegEncodeUtil::GetRgbColorSpaceForCompression(uint32_t compression)
{
	UNREFERENCED_PARAMETER(compression);

	return PHOTOMETRIC_YCBCR;
}


BOOL FKTiffJpegEncodeUtil::StartWriting()
{
	m_pageBuffer = std::vector<uint8_t>();//clear.

	return TRUE;
}

BOOL FKTiffJpegEncodeUtil::WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError)
{
	eError = kFKNoError;
	try{
		uint32_t currentLen = m_pageBuffer.size();
		m_pageBuffer.resize(currentLen + cData.GetLength());
		ES_ErrorBailWithAction((m_pageBuffer.size() >= (cData.GetLength() + currentLen)), BAIL, this, eError = kFKTiffWriteScanLineError, L"WriteScanlinesWithData fails");
		memcpy((&m_pageBuffer[0] + currentLen), cData.GetConstBufferPtr(), cData.GetLength());
	}
	catch (const std::bad_alloc& e){
		UNREFERENCED_PARAMETER(e);

	}
BAIL:

	return eError == kFKNoError;
}

BOOL FKTiffJpegEncodeUtil::FinalizeWriting()
{

	ENUM_FK_ERROR_CODE  eError = kFKNoError;


	if (TIFFWriteRawStrip(m_pOutFile, 0, &m_pageBuffer[0], m_pageBuffer.size()) == 0) {
		eError = kFKTiffWriteScanLineError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("TIFFWriteRawStrip fails"));
	}

BAIL:;

	m_pageBuffer = std::vector<uint8_t>();//clear.

	return eError == kFKNoError;
}
