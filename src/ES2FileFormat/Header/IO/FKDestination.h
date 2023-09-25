//
//  FKImageDestination.h
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once

#include "FileFormatKit.h"

class CFKDestination;

//---------------------------------------------------------------------------------------
// FKWriter/FKReaderの出力先オブジェクト
//---------------------------------------------------------------------------------------

class CFKDestination : public IFKDestination
{
private:
	FKDestinationType m_eFKDestinationType;

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_cSource;
	ESImageInfo m_dictImageInfo;;

	IFKDestinationDelegate* m_pcDelegate;

public:
	CFKDestination();
	~CFKDestination();

	//---------------------------------------------------------------------------------------
  //	 データの保存オブジェクト
  //---------------------------------------------------------------------------------------
	FKDestinationType GetFKDestinationType()
	{
		return m_eFKDestinationType;
	}

	//---------------------------------------------------------------------------------------
  //データの保存オブジェクト　具体形式はFKDestinationTypeで決まる
  //---------------------------------------------------------------------------------------
	ES_CMN_FUNCS::BUFFER::IESBuffer& GetSource();

	//---------------------------------------------------------------------------------------
	//  データ入力通知量のデリゲート
	//---------------------------------------------------------------------------------------
	void SetDelegate(IFKDestinationDelegate* piDelegate){
		m_pcDelegate = piDelegate ;
	}

 	//---------------------------------------------------------------------------------------
	// ユーザーがセットできる任意のハンドル
	//---------------------------------------------------------------------------------------
	void SetUserData(const ESImageInfo& dictImageInfo){
		m_dictImageInfo = dictImageInfo;
	}


	ESImageInfo GetUserData(void){
		return m_dictImageInfo;
	}

	virtual void SetUserDataAsJson(ES_CHAR_CPTR pszUserDataAsJson) override;
	virtual void GetUserDataAsJson(IESResultString& pszUserDataAsJson) override;


	//---------------------------------------------------------------------------------------
	//  FKDestinationをインスタンス化する。
	//  sourceには
	//			kFKDestinationTypePathの場合にIESBufferにパスの文字列を格納して渡す
	//			kFKDestinationTypeDataの場合にIESBufferを渡す。内部メモリ領域を所有権移動する
	//---------------------------------------------------------------------------------------
	static IFKDestination* CreateAndInitInstanceWithType( FKDestinationType eType, ES_CMN_FUNCS::BUFFER::IESBuffer& cSource );


	void NotifyDidBeginReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson );
	void NotifyDidEndReceivingForImageInfo( ES_CHAR_CPTR pszImageInfoAsJson );
	void NotifyReceiveImageData( ES_CMN_FUNCS::BUFFER::IESBuffer& cData, ES_CHAR_CPTR pszImageInfoAsJson ) override;

	void Destroy(){
		delete this;
	}

};
