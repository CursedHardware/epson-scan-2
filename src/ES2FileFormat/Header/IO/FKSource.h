//
//  FKSource.h
//  FileFormatKit
//
//  Created by Nakazawa Yuichi on 5/12/14.
//  Copyright (c) 2014 EPSON All rights reserved.
//
#pragma once
//---------------------------------------------------------------------------------------
// FKWriter/FKReaderの入力オブジェクト
//---------------------------------------------------------------------------------------

#include "FileFormatKit.h"

typedef std::vector< ESDictionary >	FKDictArray;

//---------------------------------------------------------------------------------------
// FKWriter/FKReaderの入力オブジェクト
//---------------------------------------------------------------------------------------
class CFKSource : public IFKSource
{
private:
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer m_cSource;
	FKSourceType m_eType;		// FKSourceのsourceの種別
	BOOL         m_bFull;

public:
	CFKSource();
	virtual ~CFKSource();

	//---------------------------------------------------------------------------------------
	//	FKSourceのsourceの種別
	//---------------------------------------------------------------------------------------
	FKSourceType GetFKSourceType() const override
	{
		return m_eType;
	}

	//---------------------------------------------------------------------------------------
	 // 一度に全ての画像データを入力する場合はtrueとする
	//---------------------------------------------------------------------------------------
	BOOL IsFull()  const override
	{
		return m_bFull;
	}


	ES_CMN_FUNCS::BUFFER::IESBuffer& GetSource( );


	//---------------------------------------------------------------------------------------
	// FKSourceをインスタンス化する。
	//---------------------------------------------------------------------------------------
	static IFKSource* CreateAndInitInstanceWithType(FKSourceType eSourceType, ES_CMN_FUNCS::BUFFER::IESBuffer& vpSource, BOOL bIsFull);

	void Destroy() override{
		delete this;
	}

	const ES_CMN_FUNCS::BUFFER::IESBuffer& GetConstSource( ) const override;

};
