//
//  FKImageReader.h
//  ESImageFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once

#include "FileFormatKit.h"
#include "FKReaderKeys.h"
#include "FKSource.h"
#include "FKDestination.h"



//---------------------------------------------------------------------------------------
//  RAW形式での各ファイルの読み込みのためのインターフェイス
//--------------------------------------------------------------------------------------
class CFKReader : public IFKReader
{
protected:
	BOOL	m_bOpen;
	ESNumber	m_nValidImageHeight;
	BOOL	m_bReadOnlyValidData;

	IFKDestination* m_piFkDestination;
	ESImageInfo	    m_esImageInfo;

public:
	CFKReader();
	virtual ~CFKReader();

	static CFKReader* CreateFKReaderWithType(FKReaderType eType);

	void  SetValidImageHeight(ESNumber nHeight,BOOL bReadOnlyValidData) override
	{
		m_nValidImageHeight = nHeight;
		m_bReadOnlyValidData = bReadOnlyValidData;
	}


	//---------------------------------------------------------------------------------------
	// 出力先オブジェクト
	//--------------------------------------------------------------------------------------
	IFKDestination* GetCFkDestionation()
	{
		return m_piFkDestination;
	}

	ESImageInfo& GetImageInfo(void);

	virtual void GetImageInfoAsJson(IESResultString& strJson) override;

	//---------------------------------------------------------------------------------------
	// 画像の読み込みを開始する。
	//
  //
  // 出力先を指定して出力を開始する。
  // 出力先は「ファイル」、「メモリ」、「なし」から指定できる。
  // FKDestinationのsourceにて出力先に応じた出力オブジェクトを取得できる。
  //
  //	 「なし」を指定した場合には出力はFKDestinationDelegateのみで取得できる。
  //サポートしている出力先はファイル保存形式(FKwriterType)によって異なる。
	// kFKWriterTypeJpeg  : メモリ, なし
  // kFKWriterTypeTiff  : メモリ, ファイル, なし
  //
  // オプションをFKReaderOpenOptionDictで指定できる。
  //--------------------------------------------------------------------------------------

	BOOL OpenWithDestination( IFKDestination* pcDestination, ENUM_FK_ERROR_CODE& eError );
	BOOL OpenWithDestinationAsJson( IFKDestination* pcDestination, ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );


	//---------------------------------------------------------------------------------------
	//  処理対象のデータを追加する
  //  一度に画像データ全てを追加する場合にはisFull=YESとする。
	// 入力元は「ファイル」「メモリ」から選択できる。
	//  FKSourceのsourceにて入力元に応じた入力オブジェクトを設定する。
 	//  また、一度に一ページ分の画像データを渡す『一括渡し』と部分画像データずつ渡す『ブロック渡し」が可能であり、isFullフラグによって区別する。
	// @caution 現在、ファイル形式でのブロック渡しはサポートしていない
  //
	// サポートしている入力元はファイル形式(FKReaderType)によって異なる。
  //
  // kFKReaderTypeJpeg : メモリ
  //  kFKReaderTypeTiff : ファイル
	//---------------------------------------------------------------------------------------
	virtual BOOL AppendSource(IFKSource* source, ENUM_FK_ERROR_CODE& eError);

	//---------------------------------------------------------------------------------------
	//  画像の読み込みを終了する。
	//
	// オプションをFKReaderCloseOptionDictで指定できる。
	//---------------------------------------------------------------------------------------
	BOOL CloseAndReturnError( ENUM_FK_ERROR_CODE& eError );
	BOOL CloseWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError );


	//---------------------------------------------------------------------------------------
	//	  サブクラスにてオーバライドする
	//---------------------------------------------------------------------------------------
	virtual BOOL OpenReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL AppendSourceToReader( IFKSource* pcSource, ENUM_FK_ERROR_CODE& eError ) = 0;
	virtual BOOL CloseReaderWithOptionAsJson( ES_CHAR_CPTR pszOptionAsJson, ENUM_FK_ERROR_CODE& eError ) = 0;


	void Destroy(){
		delete this;
	}

};
