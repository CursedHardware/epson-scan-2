//
//  FileFormatKit.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once
#include "ESBuffer.h"
#include "ESCommonTypedef.h"
#include "FileFormatKitErrors.h"
#include "FKReaderKeys.h"
#include "FKWriterKeys.h"
#include "ESResultString.h"
//---------------------------------------------------------------------------------------
// FKWriter/FKReaderの入力先
//---------------------------------------------------------------------------------------

typedef enum : uint32_t
{
	kFKSourceTypePath = 1,
	kFKSourceTypeData,
}FKSourceType;

//---------------------------------------------------------------------------------------
// FKWriter/FKReaderの出力先
//---------------------------------------------------------------------------------------
typedef enum : uint32_t
{
	kFKDestinationTypePath = 1,///< ファイル出力
	kFKDestinationTypeData,    ///< NSData出力
	kFKDestinationTypeEmpty,  ////< 出力先なし(FKDestinationDelegateのみでデータを受け取り)
}FKDestinationType;

//---------------------------------------------------------------------------------------
// FKReaderの入力元フォーマット
//---------------------------------------------------------------------------------------
typedef enum : uint32_t
{
	kFKReaderTypeJpeg	= 1,
	kFKReaderTypeTiff	= 2,
	kFKReaderTypeBmp	= 3,
#ifndef WIN32
	kFKReaderTypePnm	= 4,
#endif
}FKReaderType;

//---------------------------------------------------------------------------------------
//  FKWriterの出力先フォーマット
//---------------------------------------------------------------------------------------
typedef enum : uint32_t
{
	kFKWriterTypeJpeg	= 0,
	kFKWriterTypePDF	= 1,
	kFKWriterTypeTiff	= 2,
	kFKWriterTypePng	= 3,
	kFKWriterTypeBmp	= 4,
#ifndef WIN32
	kFKWriterTypePnm	= 5,
	kFKWriterTypeOFD	= 6
#endif
}FKWriterType;


class IFKSource
{
protected:
	virtual ~IFKSource(){}
public:
	virtual FKSourceType GetFKSourceType() const = 0;
	virtual BOOL IsFull() const = 0;
	virtual ES_CMN_FUNCS::BUFFER::IESBuffer& GetSource() = 0;
	virtual void Destroy() = 0;

	virtual const ES_CMN_FUNCS::BUFFER::IESBuffer& GetConstSource() const = 0;

};

class IFKDestination;

class IFKDestinationDelegate
{
protected:
	virtual ~IFKDestinationDelegate(){}
public:
	virtual void CALLBACK DestinationDidBeginReceiving( IFKDestination* pcDestination, ES_CHAR_CPTR pszImageInfoAsJson ) = 0;
	virtual void CALLBACK DestinationDidEndReceiving( IFKDestination* pcDestination, ES_CHAR_CPTR pszImageInfoAsJson ) = 0;
	virtual void CALLBACK DestinationDidReceiveImageData( IFKDestination* pcDestination, ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ES_CHAR_CPTR pszImageInfoAsJson ) = 0;
};


class IFKDestination
{
protected:
	virtual ~IFKDestination(){}
public:
	virtual FKDestinationType GetFKDestinationType() = 0;
	virtual ES_CMN_FUNCS::BUFFER::IESBuffer& GetSource() = 0;
	virtual void SetDelegate(IFKDestinationDelegate* piDelegate) = 0;

	virtual void SetUserDataAsJson(ES_CHAR_CPTR pszUserDataAsJson) = 0;
	virtual void GetUserDataAsJson(IESResultString& strUserDataAsJson) = 0;

//	virtual IFKDestination* CreateInstanceWithType( FKDestinationType eType, ESAny vpSource ) = 0;
	virtual void NotifyDidBeginReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson ) = 0;
	virtual void NotifyDidEndReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson ) = 0;
	virtual void NotifyReceiveImageData(ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ES_CHAR_CPTR pszImageInfoAsJson ) = 0;

	virtual void Destroy()=0;

};

class IFKReader
{
protected:
	virtual ~IFKReader(){}
public:
	virtual void  SetValidImageHeight(ESNumber nHeight,BOOL bReadOnlyValidData) = 0;

	virtual IFKDestination* GetCFkDestionation() = 0;

	virtual void GetImageInfoAsJson(IESResultString& sttrImageInfoAsJson) = 0;

	virtual BOOL OpenWithDestination( IFKDestination* pcDestination, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL OpenWithDestinationAsJson( IFKDestination* pcDestination, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;


	virtual BOOL CloseAndReturnError( ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL CloseWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;


	virtual BOOL AppendSource(IFKSource* source, ENUM_FK_ERROR_CODE& eError) = 0;

	virtual BOOL OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;


	virtual void Destroy() = 0;
};

class IFKWriter
{
protected:
	virtual ~IFKWriter(){}
public:
	//virtual ESImageInfo* GetImageInfo() = NULL;	// TBD:必要なら実装

	virtual IFKDestination* GetCFKDestination() = 0;

	virtual BOOL IsMultiPageSupport() = 0;

	virtual ESNumber GetPageCount() = 0;

	virtual BOOL InitializeDestination( IFKDestination* pDestination, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL InitializeDestinationAsJson( IFKDestination* pDestination, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

	virtual BOOL OpenPageWithImageInfo( ES_CHAR_CPTR pszImageInfoAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL OpenPageWithImageInfoAsJson( ES_CHAR_CPTR pszImageInfoAsJson, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

	virtual BOOL ClosePageAndReturnError( ENUM_FK_ERROR_CODE& eError) = 0;
	virtual BOOL ClosePageAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

	virtual BOOL FinalizeAndReturnError( ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL FinalizeAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;

	virtual BOOL AppendSource( IFKSource* pSource, ENUM_FK_ERROR_CODE& eError ) = 0;
	/**
	所定のバイト数の書き込みが可能かどうかをチェックする
	書き込みができない場合にはNOが返る
 （ファイル形式によってサイズ上限があるため）
	*/
	virtual BOOL CheckFileFormatMaxbytes(ESNumber nNextImageSize) = 0;

	virtual void Destroy()=0;

	virtual BOOL AppendConstSource(const IFKSource* pSource, ENUM_FK_ERROR_CODE& eError ) = 0;
};
