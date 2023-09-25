//
//  FKWriter
//  ESImageFormatKit
//
//  Created by Nakazawa Tuichi on 2014/12/18.
//  Copyright (c) 2013 EPSON All rights reserved.
//
#include "stdafx.h"

#include "FKWriter.h"
#include "FileFormatKitErrors.h"
#include "../Writer/FKJpegWriter.h"
#include "../Writer/FKPngWriter.h"
#include "../Writer/FKTiffWriter.h"
#include "../Writer/FKBmpWriter.h"
#ifdef WIN32
#include "../Writer/FKPdfWriter.h"
#else
#include "../Writer/FKPnmWriter.h"
#endif
//#include "FKErrorUtil.h"

CFKWriter::CFKWriter()
{
    m_eState			= kFKWriterStateNotInitialized;

    m_pCFKDestination	= NULL;
    m_uPages			= 0;

}

CFKWriter::~CFKWriter()
{

	if( m_pCFKDestination ){
		m_pCFKDestination->Destroy();
		m_pCFKDestination = NULL;
	}
}

CFKWriter* CFKWriter::CreateFKWriterWithType(FKWriterType eWriterType, FKWriterInitDict& dictOpetion){

	CFKWriter* pcFKWriter = NULL;

	if (eWriterType == kFKWriterTypeJpeg) {
		pcFKWriter = new (std::nothrow) CFKJpegWriter();
		pcFKWriter->Initialize();
	}else if(eWriterType == kFKWriterTypePDF) {
	#ifdef WIN32
		pcFKWriter = new (std::nothrow) CFKPdfWriter();
		pcFKWriter = ((CFKPdfWriter*)pcFKWriter)->InitWithOption( dictOpetion );
	#endif
	}else if(eWriterType == kFKWriterTypePng) {
		pcFKWriter = new (std::nothrow) CFKPngWriter();
		pcFKWriter->Initialize();
	}else if(eWriterType == kFKWriterTypeTiff) {
		pcFKWriter = new (std::nothrow) CFKTiffWriter();
		pcFKWriter->Initialize();
	}else if (eWriterType == kFKWriterTypeBmp) {
		pcFKWriter = new (std::nothrow) CFKBmpWriter();
		pcFKWriter->Initialize();
  }else if (eWriterType == kFKWriterTypePnm) {
    pcFKWriter = new (std::nothrow) CFKPnmWriter();
    pcFKWriter->Initialize();
	}
	else{
		assert(false);
		return NULL;
	}

	return pcFKWriter;
}

ESNumber CFKWriter::GetPageCount()
{
	if( !IsMultiPageSupport() )
	{
		if (m_uPages > 1) {
			return 1;
		}else{
			return 0;
		}
	}
	return m_uPages;
}


BOOL CFKWriter::CheckWriterState( FKWriterState ePreferedState, ENUM_FK_ERROR_CODE& eError ) const
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;

    if (ePreferedState != m_eState) {
        errorCode = kFKInconsistentError;
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsistent"));
    }

BAIL:;

    eError = errorCode;

    return errorCode == kFKNoError;
}


BOOL CFKWriter::InitializeDestination( IFKDestination* pDestination, ENUM_FK_ERROR_CODE& eError )
{
	return InitializeDestinationAsJson( pDestination, NULL, eError );
}

BOOL CFKWriter::InitializeDestinationAsJson( IFKDestination* pDestination, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;
	FKWriterInitializeDestDict dictWriter;

    //ESTrace(@"writer:%@ option:%@", self, option);	// TBD:未実装 EsTrace実装待ち

	if ( CheckWriterState( kFKWriterStateNotInitialized, eError ) == FALSE ) {
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
    }

	m_pCFKDestination = pDestination;

	if(pszOptionAsJson){
		ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(pszOptionAsJson),dictWriter);
	}
	if ( InitializeWriterWithDestination( (CFKDestination*)pDestination, &dictWriter, eError ) == FALSE ) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("InitializeWriterWithDestination fails"));
	}

    m_eState = kFKWriterStateNotOpened;
    m_uPages = 0;

BAIL:;

    eError = errorCode;

    return errorCode == kFKNoError;
}

// should be overriedden
BOOL CFKWriter::InitializeWriterWithDestination( CFKDestination* pCKDestination,
													FKWriterInitializeDestDict* pOption,
													ENUM_FK_ERROR_CODE& eError)
{
	assert(false);
	return TRUE;
}

BOOL CFKWriter::OpenPageWithImageInfo( ES_CHAR_CPTR pszImageInfoAsJson, ENUM_FK_ERROR_CODE& eError )
{
	return OpenPageWithImageInfoAsJson( pszImageInfoAsJson, NULL, eError );
}

BOOL CFKWriter::OpenPageWithImageInfoAsJson( ES_CHAR_CPTR pszImageInfoAsJson, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;
	FKWriterPageOpenDict dictWriter;


    //ESTrace(@"writer:%@ option:%@", self, option	// TBD:未実装 EsTrace実装待ち

	if ( CheckWriterState( kFKWriterStateNotOpened, eError ) == FALSE ) {
		errorCode = eError;
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
    }
	if (pszOptionAsJson){
		ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(pszOptionAsJson), dictWriter);
	}
	if (pszImageInfoAsJson){

		ES_IMAGE_INFO::MakeImageInfoFromJson(m_dictEsImageInfo, pszImageInfoAsJson);
	}


	if ( OpenWriterPageWithImageInfo( &m_dictEsImageInfo, &dictWriter, eError ) == FALSE ) {
		errorCode = eError;
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("OpenWriterPageWithImageInfo fails"));
    }

    m_eState = kFKWriterStateProcessing;

BAIL:;

    eError = errorCode;

    return errorCode == kFKNoError;
}

BOOL CFKWriter::OpenWriterPageWithImageInfo( ESImageInfo* pImageInfo,
											FKWriterPageOpenDict* pOption,
											ENUM_FK_ERROR_CODE& eError)
{
    assert(false);
    return TRUE;
}

BOOL CFKWriter::ClosePageAndReturnError( ENUM_FK_ERROR_CODE& eError )
{
	return ClosePageAndReturnErrorAsJson( NULL, eError );
}

BOOL CFKWriter::ClosePageAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
    ENUM_FK_ERROR_CODE errorCode = kFKNoError;
	FKWriterPageCloseDict dictWriter;

    //ESTrace(@"writer:%@ option:%@", self, option);	// TBD:未実装 EsTrace実装待ち

	if ( CheckWriterState( kFKWriterStateProcessing, eError ) == FALSE ) {

        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
    }
	if(pszOptionAsJson){
		ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(pszOptionAsJson),dictWriter);
	}
	if ( CloseWriterPageWithOption( &dictWriter, eError ) == FALSE ) {
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("CloseWriterPageWithOption fails"));
    }

    m_eState = kFKWriterStateNotOpened;
    m_uPages ++;
BAIL:;

     errorCode = eError;

    return  errorCode == kFKNoError;
}

BOOL CFKWriter::CloseWriterPageWithOption( FKWriterPageCloseDict* pOption, ENUM_FK_ERROR_CODE& eError )
{
    assert(false);
    return TRUE;
}

BOOL CFKWriter::FinalizeAndReturnError( ENUM_FK_ERROR_CODE& eError )
{
	return FinalizeAndReturnErrorAsJson( NULL, eError );
}

BOOL CFKWriter::FinalizeAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError )
{
	FKWriterFinalizeDestDict dictWriter;

    //ESTrace(@"writer:%@ option:%@", self, option);	// TBD:未実装 EsTrace実装待ち

	if (CheckWriterState(kFKWriterStateNotOpened, eError) == FALSE) {
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
    }

	if(pszOptionAsJson){

		ES_CMN_FUNCS::JSON::JSONtoDictionary(ESString(pszOptionAsJson),dictWriter);
	}

	if ( FinalizeWriterWithOption( &dictWriter, eError ) == FALSE ) {
        ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("CloseWriterPageWithOption fails"));
    }

    m_eState = kFKWriterStateNotInitialized;
    m_uPages = 0;

BAIL:;


    return  eError == kFKNoError;

}

BOOL CFKWriter::FinalizeWriterWithOption( FKWriterFinalizeDestDict* pOption, ENUM_FK_ERROR_CODE& eError )
{
    assert(false);
    return TRUE;
}


BOOL CFKWriter::AppendSource( IFKSource* pSource, ENUM_FK_ERROR_CODE& eError )
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;

	if(CheckWriterState(kFKWriterStateProcessing, errorCode) == FALSE) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
	}

	if(AppendSourceToDestination((CFKSource*)pSource,errorCode) == FALSE) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("fails to write destination"));
	}

BAIL:;
	eError = errorCode;

	return errorCode == kFKNoError;
}

BOOL CFKWriter::AppendConstSource(const IFKSource* pSource, ENUM_FK_ERROR_CODE& eError )
{
	ENUM_FK_ERROR_CODE errorCode = kFKNoError;

	if(CheckWriterState(kFKWriterStateProcessing, errorCode) == FALSE) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("state inconsitent"));
	}

	if(AppendConstSourceToDestination((const CFKSource*)pSource,errorCode) == FALSE) {
		ES_Bail(BAIL, ENUM_LOG_LEVEL::LogLevelError, this, ES_STRING("fails to write destination"));
	}

BAIL:;
	eError = errorCode;

	return errorCode == kFKNoError;
}

BOOL CFKWriter::AppendSourceToDestination( CFKSource* pSource, ENUM_FK_ERROR_CODE& eError )
{
    assert(false);
    return FALSE;
}
BOOL CFKWriter::AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError )
{
    assert(false);
    return FALSE;
}
