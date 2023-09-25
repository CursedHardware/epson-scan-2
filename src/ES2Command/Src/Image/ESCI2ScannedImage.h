////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2ScannedImage.h
//!
//! @brif     ESCI/2コマンド時の、スキャン画像クラスの定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "ESScannedImage.h"

class CESCI2ScannedImage : public CESScannedImage
{
public:
	// コンストラクタ
	CESCI2ScannedImage();
	// デストラクタ
	virtual ~CESCI2ScannedImage();

	// インスタンスを削除する
	virtual void DeleteInstance() override;

	ESNumber GetStoreBufferSize();
	void SetStoreBufferSize(ESNumber nBufferSize);

	ESNumber GetDummyBytesPerLine();
	void SetDummyBytesPerLine( ESNumber nDummyBytesPerLine );

	virtual void CloseWithWidth( ESNumber nWidth, ESNumber nHeight ) override;
	virtual ESErrorCode WriteData( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cImageData ) override;

	virtual bool IsProgressAvailable() override;

protected:
	ESNumber								m_nDummyBytesPerLine;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer		m_cRemainingData;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer		m_cJpegTempBuf;
	ESNumber								m_nStoreBufferSize;
};
