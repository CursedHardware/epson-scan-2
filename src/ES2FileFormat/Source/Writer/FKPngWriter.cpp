//
//  FKJpegWriter.m
//  ESImageFormatKit
//
//  Created by T.Kondo on 2015/2/25.
//  Copyright (c) 2013-2015 EPSON All rights reserved.
//
#include "stdafx.h"
#include "ESImageInfo.h"
#include "FileFormatKitErrors.h"
#include "FKPngWriter.h"
#include "../PngUtil/FKPngEncodeUtil.h"
#include "ESAnyCastUtils.h"
#include "DbgLog.h"

CFKPngWriter::CFKPngWriter()
{
	m_pCFKPngEncodeUtil = NULL;

}

CFKPngWriter::~CFKPngWriter()
{
	delete m_pCFKPngEncodeUtil;
	m_pCFKPngEncodeUtil = NULL;
}

BOOL CFKPngWriter::Initialize()
{
	m_pCFKPngEncodeUtil = safeNew CFKPngEncodeUtil();
	if (m_pCFKPngEncodeUtil == NULL){
		return FALSE;
	}
	return TRUE;
}

BOOL CFKPngWriter::InitializeWriterWithDestination(
	CFKDestination* pCFKDestination,
	FKWriterInitializeDestDict* pOption,
	ENUM_FK_ERROR_CODE& eError)
{
	UNREFERENCED_PARAMETER(pOption);

	eError = kFKNoError;
	ES_ErrorBailWithAction(m_pCFKPngEncodeUtil != NULL, BAIL, this, eError = kFKInconsistentError, L"not initialized");
	ES_ErrorBailWithAction((pCFKDestination->GetFKDestinationType() == kFKDestinationTypePath), BAIL, this, eError = kFKUnsupportedDestination, L"unsupported dest");


BAIL:;


	return eError == kFKNoError;
}


BOOL CFKPngWriter::OpenWriterPageWithImageInfo(
	ESImageInfo* pImageInfo,
	FKWriterPageOpenDict* pOption,
	ENUM_FK_ERROR_CODE& eError)

{
	ESString strPath = (LPTSTR)(((GetCFKDestination()->GetSource())).GetBufferPtr());
	ES_ErrorBail(pOption,BAIL,  this, ES_STRING("pOption is null"));
	ES_ErrorBail(pImageInfo,BAIL,  this, ES_STRING("pImageInfo is null"));


	if (!this->m_pCFKPngEncodeUtil->StartEncodingWithPath(strPath,
														*pImageInfo,
														eError)){
		//		eError = kFKUnknownError;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("startWithPath fails"));
	}
	m_eState = kFKWriterStateProcessing;
BAIL:;


	return eError == kFKNoError;
}
BOOL CFKPngWriter::CloseWriterPageWithOption(FKWriterPageCloseDict* pOption, ENUM_FK_ERROR_CODE& eError)
{
	UNREFERENCED_PARAMETER(pOption);
	eError = kFKNoError;

	return eError == kFKNoError;
}

BOOL CFKPngWriter::AppendSourceToDestination( CFKSource* pSource, ENUM_FK_ERROR_CODE& eError )
{
	return AppendConstSourceToDestination(pSource,eError);
}

BOOL CFKPngWriter::AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError)
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;
	ES_ErrorBailWithAction((pSource->GetFKSourceType() == kFKSourceTypeData), BAIL, this, errorCode = kFKUnsupportedDestination, ES_STRING("unsupported dest"));

	{

		BOOL bSuccess = m_pCFKPngEncodeUtil->WriteScanlinesWithData(pSource->GetConstSource(), eError);
		ES_ErrorBailWithAction(bSuccess, BAIL, this, errorCode = kFKParameterError, ES_STRING("writeScanLines fails"));
	}
BAIL:;
	eError = errorCode;

	return errorCode == kFKNoError;

}

BOOL CFKPngWriter::IsMultiPageSupport()
{
	return FALSE;
}

BOOL CFKPngWriter::FinalizeWriterWithOption(FKWriterFinalizeDestDict* pOption,
	ENUM_FK_ERROR_CODE& eError)
{
	UNREFERENCED_PARAMETER(pOption);

	m_eState = kFKWriterStateNotOpened;
	if (!m_pCFKPngEncodeUtil->FinalizeEncodingAndReturnError(eError)) {
		m_eState = kFKWriterStateNotOpened;
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("finalize fails"));
	}


	return TRUE;
BAIL:;

	return FALSE;
}
