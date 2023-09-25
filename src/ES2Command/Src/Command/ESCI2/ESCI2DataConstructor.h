////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2DataConstructor.h
//!
//! @brif     ESCI2コマンド送信データー作成クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 

class CESCI2DataConstructor 
{
public:
	CESCI2DataConstructor();
	~CESCI2DataConstructor();

	void Init();

	void AppendFourCharString( const ESStringA& strFourChar );
	void AppendDecimal( unsigned short usDecimal );
	void AppendInteger( int nInteger );
	void AppendHex( unsigned long nHex );
	void AppendData( const ESByteData& data );

	ESByteData& GetData()
	{
		return m_data;
	}

	ESByteData* GetDataPtr()
	{
		return &m_data;
	}

protected:
	ESByteData m_data;
};
