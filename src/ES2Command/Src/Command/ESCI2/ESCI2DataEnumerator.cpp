////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2DataEnumerator.cpp
//!
//! @brif     ESCI2コマンド受信データーの型解析クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ESCI2DataEnumerator.h"


CESCI2DataEnumerator::CESCI2DataEnumerator( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& data )
 :	m_pDataSource( NULL ),
    m_cBufferStream( &data )
{
}

CESCI2DataEnumerator::~CESCI2DataEnumerator(void)
{
}

void CESCI2DataEnumerator::SetDataSource( IESCI2DataEnumeratorDataSource* pDataSource )
{
	m_pDataSource = pDataSource;
}


UInt32 CESCI2DataEnumerator::Read( PESByte pBuffer, UInt32 un32Length )
{
	if ( pBuffer == NULL ){
		return 0;
	}
	memset( pBuffer, 0, un32Length );

	UInt32 un32Read = m_cBufferStream.Read( pBuffer, un32Length );
	if ( ( un32Read < un32Length ) && ( m_pDataSource != NULL ) ){
		un32Read += m_pDataSource->ReadFromEnumerator( this, pBuffer + un32Read, un32Length - un32Read );
	}
	return un32Read;
}

/**
 ESC/I-2データの先頭から解析を開始し、呼び出すたびにプリミティブ（分割不可能な最小のデータ）を一つ返す。
 データの終端に到達した場合はnilを返す。
    
    '#ADF'          -> "#ADF"   (NSString)
    'd005'          -> 5        (NSNumber),
    'i-000200'      -> -200     (NSNumber),
    'x00000FF'      -> 255      (NSNumber)
    'h005DS-50   '  -> "DS-50"  (NSData)
 
 @return ESStringA, ESNumber, ESByteDataのいずれかのインスタンス。あるいはnil。
 */
ESAny CESCI2DataEnumerator::Nextdata()
{
	char szFourBytes[5] = {0};
	int res = Read( (LPBYTE)szFourBytes, 4 );
	if ( res <= 0 ){
		return nullptr;
	}

	char chDataType  = szFourBytes[0];
	switch ( chDataType ){
		case 'd': 
			{
				char buf[4] = {0};
				memcpy(buf, szFourBytes + 1, 3);

				ESNumber nValue = (ESNumber)strtol( buf, NULL, 10 );
				return nValue;
			}
			break;
		case 'i': 
			{
				char buf[8] = {0};
				memcpy(buf, szFourBytes + 1, 3);

				res = Read( (PESByte)buf+3, 4 );
				if ( res < 4 ){
					return nullptr;
				}

				ESNumber nValue = (ESNumber)strtol( buf, NULL, 10 );
				return nValue;
			}
			break;
		case 'x': 
			{
				char buf[8] = {0};
				memcpy(buf, szFourBytes + 1, 3);

				res = Read( (PESByte)buf+3, 4 );
				if ( res < 4 ){
					return nullptr;
				}

				ESNumber nValue = (ESNumber)strtol( buf, NULL, 16 );
				return nValue;
			}
			break;
		case 'h': 
			{
				char buf[4] = {0};
				memcpy(buf, szFourBytes + 1, 3);

				long lDataSize = strtol( buf, NULL, 16 );
				long lAlignedLength = ((lDataSize + 3) / 4) * 4;

				if ( lAlignedLength <= 0 ){
					return nullptr;
				}
				ESByteData data(lAlignedLength, 0);
				res = Read( &data[0], (UInt32)lAlignedLength );
				if ( res < lAlignedLength ){
					return nullptr;
				}
				data.resize(lDataSize);
				return data;
			}
			break;
		default: 
			{
				ESStringA strValue = szFourBytes;
				return strValue;
			}
			break;
	}
}
