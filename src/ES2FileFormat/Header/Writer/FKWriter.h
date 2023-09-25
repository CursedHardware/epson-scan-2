//
//  FKWriter.h
//  ESImageFormatKit
//
//  Created by Nakazawa Tuichi on 2014/12/18.
//  Copyright (c) 2013 EPSON All rights reserved.
//
#pragma once

#include "FKWriterKeys.h"
#include "FKSource.h"
#include "FKDestination.h"
#include "FileFormatKit.h"

/*
 FKWriterクラスの内部状態を示す。
*/
enum FKWriterState
{
    kFKWriterStateNotInitialized = 0,
    kFKWriterStateNotOpened      = 1,
    kFKWriterStateProcessing     = 2,
};


/**
  RAWデータの各ファイル形式での書き込み（エンコード）のためのクラス
*/
class CFKWriter : public IFKWriter
{
protected:
    FKWriterState		m_eState;			         // FKWriterState state_;

    ESImageInfo			m_dictEsImageInfo;	   // ESImageInfo*    imageInfo_;
    IFKDestination*		m_pCFKDestination;  // FKDestination*  destination_;
    ESNumber				m_uPages;			        // uint32_t        pages_;

public:
	CFKWriter();
	virtual ~CFKWriter();
	//@property (readonly) ESImageInfo*    imageInfo;
	//@property (readonly) FKDestination*  destination;
	//@property (readonly) BOOL            multiPageSupport;
	//@property (readonly) uint32_t        pages;

	static CFKWriter* CreateFKWriterWithType(FKWriterType eType,ESDictionary& dictOpetion);

	void Destroy(){
		delete this;
	}

	ESImageInfo& GetImageInfo()
	{
		return m_dictEsImageInfo;
	}

	IFKDestination* GetCFKDestination()
	{
		return m_pCFKDestination;
	}

	virtual BOOL IsMultiPageSupport() = 0;


	ESNumber GetPageCount();
	//ESNumber GetPageCount()
	//{
	//	return m_uPages;
	//}


	/**
  出力先を指定して出力を開始する。
  出力先は「ファイル」、「メモリ」、「なし」から指定できる。
 「なし」を指定した場合には出力はFKDestinationDelegateのみで取得できる。

 サポートしている主力先はファイル保存形式(FKwriterType)によって異なる。
  kFKWriterTypeJpeg  : ファイル、メモリ、なし
  kFKWriterTypePDF   : ファイル
  kFKWriterTypeTiff  : ファイル
  kFKWriterTypePng   : ファイル

 オプションをFKWriterInitializeDestDictで指定できる
	*/
	//- (BOOL)initializeDestination:(FKDestination*)destination error:(NSError**)outError;
	//- (BOOL)initializeDestination:(FKDestination*)destination option:(FKWriterInitializeDestDict*)option error:(NSError**)outError;
	BOOL InitializeDestination( IFKDestination* pDestination, ENUM_FK_ERROR_CODE& eError );
	BOOL InitializeDestinationAsJson( IFKDestination* pDestination, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );

  /**
　　	   FKWriterInitializeOptionDictの設定可能なキ一覧を返す。
	*/
	//-  (NSArray*)availableInitializeOptionKeys;

	/**
	  ページ画像の入力を開始する
	  オプションをFKWriterPageOpenDictで指定できる
	 */
	//- (BOOL)openPageWithImageInfo:(ESImageInfo*)imageInfo
	//						error:(NSError**)outError;
	//- (BOOL)openPageWithImageInfo:(ESImageInfo*)imageInfo
	//					   option:(FKWriterPageOpenDict*)option
	//						error:(NSError**)outError;
	BOOL OpenPageWithImageInfo( ES_CHAR_CPTR pszImageInfoAsJson, ENUM_FK_ERROR_CODE& eError );
	BOOL OpenPageWithImageInfoAsJson( ES_CHAR_CPTR pszImageInfoAsJson, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );

	/**
	 FKWriterPageOpenDictの設定可能なキ一覧を返す。
	*/
	//- (NSArray*)availablePageOpenOptionKeys;


  /**
	   ページ画像の入力を終了する

	 オプションをFKWriterPageCloseDictで指定できる
	*/
	//- (BOOL)closePageAndReturnError:(NSError**)outError;
	//- (BOOL)closePageWithOption:(FKWriterPageCloseDict*)option
	//					  error:(NSError**)outError;
	BOOL ClosePageAndReturnError( ENUM_FK_ERROR_CODE& eError );
	BOOL ClosePageAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );

	/**
	 FFKWriterPageCloseDictの設定可能なキ一覧を返す。
	*/
	//- (NSArray*)availablePageCloseOptionKeys;

	/**
	 initializeDestinationで書き込みを開始し多出力先への出力を終了する。
	 オプションをFKWriterFinalizeDestDictで指定できる
	*/
	//- (BOOL)finalizeAndReturnError:(NSError**)outError;
	//- (BOOL)finalizeWithOption:(FKWriterFinalizeDestDict*)option
	//					 error:(NSError**)outError;
	BOOL FinalizeAndReturnError( ENUM_FK_ERROR_CODE& eError );
	BOOL FinalizeAndReturnErrorAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );

	/**
	  FKWriterFinalizeDestDictで設定可能な値の一覧を返す。
	*/
	//- (NSArray*)availableFinalizeOptionKeys;

	/**
	  RAW画像データを入力し、ファイル保存を開始する。
	  入力元は「ファイル」「メモリ」から選択できる。

 　	  また、一度に一ページ分の画像データを渡す『一括渡し』と部分画像データずつ渡す『ブロック渡し」が可能であり、isFullフラグによって区別する。

	  @caution 現在、ファイル形式でのブロック渡しはサポートしていない

	サポートしている入力元はファイル保存形式(FKwriterType)によって異なる。
	 kFKWriterTypeJpeg  : メモリ
	 kFKWriterTypePDF   : メモリ, ファイル
	 kFKWriterTypeTiff  : メモリ
	 kFKWriterTypePng   : メモリ
	*/
	//- (BOOL)appendSource:(FKSource*)source error:(NSError**)outError;
	virtual BOOL AppendSource(IFKSource* pSource, ENUM_FK_ERROR_CODE& eError );
	virtual BOOL AppendConstSource(const IFKSource* pSource, ENUM_FK_ERROR_CODE& eError );


protected:
	//- (BOOL)checkWriterState:(FKWriterState)preferedState error:(NSError**)outError
	BOOL CheckWriterState( FKWriterState ePreferedState, ENUM_FK_ERROR_CODE& eError ) const;


protected:

	//FKConcreteWriterProtocol.h

	virtual BOOL Initialize()=0;

//@required
	//- (BOOL)initializeWriterWithDestination:(FKDestination*)destination
    //                                 option:(FKWriterInitializeDestDict*)option
    //                                  error:(NSError**)outError;
	//
	virtual BOOL InitializeWriterWithDestination( CFKDestination* pCFKDestination,
											FKWriterInitializeDestDict* pOption,
											ENUM_FK_ERROR_CODE& eError) = 0;

	//- (BOOL)openWriterPageWithImageInfo:(ESImageInfo*)imageInfo
	//							 option:(FKWriterPageOpenDict*)option
	//							  error:(NSError**)error;
	virtual BOOL OpenWriterPageWithImageInfo(	ESImageInfo* pImageInfo,
										FKWriterPageOpenDict* pOption,
										ENUM_FK_ERROR_CODE& eError) = 0;


	//- (BOOL)closeWriterPageWithOption:(FKWriterPageCloseDict*)option
	//							error:(NSError**)outError;
	virtual BOOL CloseWriterPageWithOption( FKWriterPageCloseDict* pOption,
									ENUM_FK_ERROR_CODE& eError ) = 0;


	//- (BOOL)finalizeWriterWithOption:(FKWriterFinalizeDestDict*)option
	//							   error:(NSError **)outError;
	virtual BOOL FinalizeWriterWithOption( FKWriterFinalizeDestDict* pOption,
									ENUM_FK_ERROR_CODE& eError ) = 0;

	//- (BOOL)appendSourceToDestination:(FKSource*)source
	//							 error:(NSError**)outError
	virtual BOOL AppendSourceToDestination( CFKSource* pSource, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL AppendConstSourceToDestination(const CFKSource* pSource, ENUM_FK_ERROR_CODE& eError ) = 0;


	virtual BOOL CheckFileFormatMaxbytes(ESNumber nNextImageSize) {
		return TRUE;
	}


//@optional

};
