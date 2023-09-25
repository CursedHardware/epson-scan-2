////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2DataEnumerator.h
//!
//! @brif     ESCI2コマンド受信データーの型解析クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommonUtility/utils/ESBuffer.h"


class CESCI2DataEnumerator;

class IESCI2DataEnumeratorDataSource
{
public:
	virtual UInt32 ReadFromEnumerator( CESCI2DataEnumerator* pSender, PESByte pBuffer, UInt32 un32Length ) = 0;
};



class CESCI2DataEnumerator
{
public:
	explicit CESCI2DataEnumerator( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& data );
	~CESCI2DataEnumerator(void);

	void SetDataSource( IESCI2DataEnumeratorDataSource* pDataSouce );

	ESAny Nextdata();

protected:
	UInt32 Read( PESByte pBuffer, UInt32 un32Length );

private:
	ES_CMN_FUNCS::BUFFER::CESIBufferStream	m_cBufferStream;
	IESCI2DataEnumeratorDataSource*			m_pDataSource;
};

