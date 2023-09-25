//
//  FKPnmUtil.h
//  ESImageProcessor
//
//  Created by S.Kenichi on 6/20/17.
//  Copyright (c) 2017 EPSON. All rights reserved.
//
#pragma once
#include "FKDestination.h"

class CFKPnmEncodeUtil
{
public:
	CFKPnmEncodeUtil();
	virtual ~CFKPnmEncodeUtil();

	BOOL StartEncodingWithPath(const ESString& strPath, const ESImageInfo& pImageInfo,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError);
	BOOL StartEncodingWithData(IFKDestination* pDest, const ESImageInfo& pImageInfo,ESNumber nResolutionForMetadata, ENUM_FK_ERROR_CODE& eError);
	BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cSource, IFKDestination* pDest,  ENUM_FK_ERROR_CODE& eError);

	BOOL FinalizeEncodingAndReturnError(IFKDestination* pDest, ENUM_FK_ERROR_CODE& eError);

private:
	int WriteDataAtPosition(IFKDestination* pDest, const uint8_t* pSrc, uint32_t position, uint32_t length);
	void InitPnmInfo(const ESImageInfo& imageInfo);
	void InitPnmFileHeader(const ESImageInfo& imageInfo);

	ESString		m_pnmHeader;
	PNMINFO			m_stPnmInfo;
	void Destroy(bool bDeleteFile = false);
	uint32_t		m_rowBytes;
	uint32_t		m_currentLine;
	FILE*				m_pTarget;
	ESString		m_strTargetPath;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_cData;
	ESString		m_strImageJson;
};

class CFKPnmUtil
{
public:
	static uint32_t GetStride(const PNMINFO& pnmInfo);
	static BOOL CheckImageInfo(const ESImageInfo& imageInfo);
	static long long GetFileSize(ESString name);
	static void FlipBits(ES_CMN_FUNCS::BUFFER::IESBuffer&  cSource);
};
