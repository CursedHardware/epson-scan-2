////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2DataConstructor.cpp
//!
//! @brif     ESCI2コマンド送信データー作成クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ESCI2DataConstructor.h"


CESCI2DataConstructor::CESCI2DataConstructor()
{
}

CESCI2DataConstructor::~CESCI2DataConstructor()
{
}

void CESCI2DataConstructor::Init()
{
	m_data.clear();
}

//=========================================================================
//!
//! 文字列からESC/I-2データのバイト列に変換し、インスタンス変数にアペンドする。
//! 
//! @param strFourChar  変換したい文字列。文字列はASCIIで4文字でなければならない。
//!
//=========================================================================
void CESCI2DataConstructor::AppendFourCharString( const ESStringA& strFourChar )
{
	char buf[5] = {0};
	memcpy_s( buf, sizeof(buf), strFourChar.c_str(), std::min<size_t>(strFourChar.length(), 4));

	m_data.insert( m_data.end(), buf, buf+4 );
}

//=========================================================================
//!
//! 数値からESC/I-2データの符号なし10進数表現のバイト列に変換し、インスタンス変数にアペンドする。
//! 
//! @param usDecimal  変換したい数値。符号なし整数で999以下の値でなければならない。
//!
//=========================================================================
void CESCI2DataConstructor::AppendDecimal( unsigned short usDecimal )
{
	assert(usDecimal <= 999);

	char buf[5] = {0};
	sprintf_s(buf, sizeof(buf), "d%03u", usDecimal);

	m_data.insert( m_data.end(), buf, buf+4 );
}

//=========================================================================
//!
//! 数値からESC/I-2データの符号付き10進数表現のバイト列に変換し、インスタンス変数にアペンドする。
//! 
//! @param nInteger  変換したい数値。符号付き整数で-999999から9999999の範囲の値でなければならない。
//!
//=========================================================================
void CESCI2DataConstructor::AppendInteger( int nInteger )
{
	assert(-999999 <= nInteger && nInteger <= 9999999);

	char buf[9] = {0};
	sprintf_s(buf, sizeof(buf), "i%07d", nInteger);

	m_data.insert( m_data.end(), buf, buf+8 );
}

//=========================================================================
//!
//! 数値からESC/I-2データの16進数表現のバイト列に変換し、インスタンス変数にアペンドする。
//! 
//! @param nHex  変換したい数値。整数で0xFFFFFFF以下の値でなければならない。
//!
//=========================================================================
void CESCI2DataConstructor::AppendHex( unsigned long nHex )
{
	assert(nHex <= 0xFFFFFFF);

	char buf[9] = {0};
	sprintf_s(buf, sizeof(buf), "x%07lX", nHex);

	m_data.insert( m_data.end(), buf, buf+8 );
}

//=========================================================================
//!
//! バイト列からESC/I-2データのバイト列に変換し、インスタンス変数にアペンドする。
//! 
//! @param data  変換したいバイト列。サイズは0xFFF以下でなければならない。
//!
//=========================================================================
void CESCI2DataConstructor::AppendData( const ESByteData& data )
{
	UInt32 un32ength = (UInt32)data.size();
    assert(un32ength <= 0xFFF);

	char buf[5] = {0};
	sprintf_s(buf, sizeof(buf), "h%03X", (unsigned int)un32ength);
	m_data.insert( m_data.end(), buf, buf+4 );

	// アライメント調整
	ESByteData alignedData( data.begin(), data.end() );
	int nAlignedLength = (int)( (alignedData.size() + 3)/ 4  ) * 4 ;
	alignedData.resize( nAlignedLength );

	m_data.insert( m_data.end(), alignedData.begin(), alignedData.end() );
}

