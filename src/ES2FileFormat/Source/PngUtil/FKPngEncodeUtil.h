//
//  FKPngEncodeUtil.h
//  ESImageProcessor
//
//  Created by T.Kondo
//  Copyright (c) 2015 Seiko Epson Corporation. All rights reserved.
//
#pragma once
#include "FKDestination.h"
#include "png.h"

class CFKPngEncodeUtil
{
public:
	CFKPngEncodeUtil();
	virtual ~CFKPngEncodeUtil();

	BOOL StartEncodingWithPath(const ESString& strPath, const ESImageInfo& pImageInfo, ENUM_FK_ERROR_CODE& eError);

	BOOL WriteScanlinesWithData(const ES_CMN_FUNCS::BUFFER::IESBuffer&  cData, ENUM_FK_ERROR_CODE& eError);
	
	BOOL FinalizeEncodingAndReturnError(ENUM_FK_ERROR_CODE& eError);

private:
	void DestroyPngResources(bool bDeleteFile = false);

	uint32_t		m_rowBytes;
	FILE*			m_pTarget;
	png_structp		m_pPng;
	png_infop		m_pPngInfo;
	ESString		m_strTargetPath;
	//ESImageInfo*	m_imageInfo;
};
