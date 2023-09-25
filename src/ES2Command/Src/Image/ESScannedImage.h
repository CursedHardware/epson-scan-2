////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScannedImage.h
//!
//! @brif     スキャン画像クラスの定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Include/ES2Command/IESScannedImage.h"

class CESFile;

class CESScannedImage : public IESScannedImage
{
public:
	// コンストラクタ
	CESScannedImage();
	// デストラクタ
	virtual ~CESScannedImage();

	// インスタンスを削除する
	virtual void DeleteInstance() override;

	// インスタンスの管理を呼び出し側に移動します。
	// 呼び出し側では、IESScannerDelegate::DidCompleteScanningWithError以降で
	// DeleteInstance()を呼び出しインスタンスを削除してください
	virtual void RelegationOwner() override;
	virtual bool IsRelegationOwner() override;

	// Delegateをセットする
	virtual ESErrorCode SetDelegate( IESScannedImageDelegate* pDelegate ) override;

	// 設定値を初期化する
	virtual ESErrorCode Init( 
									ESNumber nWidth, 
									ESNumber nHeight, 
									ESNumber nBytesPerRow, 
									ESNumber nBitsPerSample, 
									ESNumber nSamplesPerPixel,
									ESString strWorkFolder
									);
	
	// 
	virtual void Open() override;
	virtual void Close() override;
	virtual void CloseWithWidth( ESNumber nWidth, ESNumber nHeight ) override;
	virtual bool IsClosed() override;
	virtual void Abort() override;

	//
	virtual void SeekToOffset( UInt32 un32Offset ) override;

	//
	virtual ESErrorCode WriteData( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cImageData );
	//
	virtual void ReadDataOfLength( UInt32 un32Length, ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData ) override;
	//
	virtual void ReadDataToEnd( ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData ) override;

	//
	virtual UInt32 GetPaperSerialNumber() override;
	virtual void SetPaperSerialNumber( UInt32 un32Number );

	//
	virtual UInt32 GetSerialNumber() override;
	virtual void SetSerialNumber( UInt32 un32Number );
	
	//
	virtual ENUM_ES_SURFACE_TYPE GetSurfaceType() override;
	virtual void SetSurfaceType( ENUM_ES_SURFACE_TYPE eSurfaceType );

	//
	virtual ENUM_ES_DOCUMENT_TYPE GetDocumentType() override;
	virtual void SetDocumentType( ENUM_ES_DOCUMENT_TYPE eDocumentType );

	//
	virtual bool IsProgressAvailable() override;
	virtual ESNumber GetProgressInPercent() override;

	//
	virtual ENUM_ES_DIGITAL_ICE_IMAGE_TYPE GetDigitalICEImageType() override;
	virtual void SetDigitalICEImageType( ENUM_ES_DIGITAL_ICE_IMAGE_TYPE eDigitalICEImageType );

	//
	virtual ESImageFormat GetImageFormat() override;
	virtual void SetImageFormat( ESImageFormat eImageFormat );

	virtual ENUM_ES_STORAGE_TYPE GetStorageType() override;
	virtual void SetStorageType( ENUM_ES_STORAGE_TYPE eStorageType ) override;

	//
	virtual bool IsAutoCropped() override;
	virtual void SetAutoCropped( bool bAutoCropped );

	//
	virtual bool IsUpsideDown() override;
	virtual void SetUpsideDown( bool bIsUpsideDown );

	//
	virtual bool IsDoubleFeed() override;
	virtual void SetDoubleFeed(bool bIsDoubleFeed);

	virtual bool IsAutoDetectMonoPage() override;
	virtual void SetAutoDetectMonoPage(bool bValue) ;
	virtual bool IsAutoDetectBlankPage() override;
	virtual void SetAutoDetectBlankPage(bool bValue) ;

	virtual ST_ES_SIZE_UN32 GetExpectedImageSize() override;	// スキャン開始時に期待される画像サイズ。
	virtual ST_ES_SIZE_UN32 GetImageSize() override;			// 実際にスキャンされた画像サイズ。close時には確定する。
	virtual void SetImageSize( const ST_ES_SIZE_UN32& size );

	virtual ESNumber GetWidth() override;
	virtual ESNumber GetHeight() override;
	virtual ESNumber GetBitsPerPixel() override;
	virtual ESNumber GetSamplesPerPixel() override;
	virtual ESNumber GetBitsPerSample() override;
	virtual ESNumber GetBytesPerRow() override;

protected:
	IESScannedImageDelegate*	m_pDelegate;

	ENUM_ES_SURFACE_TYPE			m_eSurfaceType;
	ENUM_ES_DOCUMENT_TYPE			m_eDocumentType;
	ENUM_ES_DIGITAL_ICE_IMAGE_TYPE	m_eDigitalICEImageType;
	ESImageFormat			m_eImageFormat;				// 画像フォーマットの種類( Raw/Jpeg など )
	ENUM_ES_STORAGE_TYPE			m_eStorageType;

	UInt32							m_un32SerialNumber;			// この画像データのデータブロック番号
	UInt32							m_un32PaperSerialNumber;	// この画像のページ番号

	ESNumber						m_nExpectedWidth;
	ESNumber						m_nExpectedHeight;

	ESNumber						m_nWidth;
	ESNumber						m_nHeight;
	ESNumber						m_nBytesPerRow;
	ESNumber						m_nBitsPerSample;
	ESNumber						m_nSamplesPerPixel;

	SInt64							m_nProcessedLength;

	bool							m_bClosed;
	CESFile*						m_pFile;
	ESString						m_strFilePath;
	ESString						m_strDumpLogFileName;

	bool							m_bIsRelegationOwner;
	bool							m_bAutoCropped;
	bool							m_bIsUpsideDown;
	bool							m_bIsDoubleFeed;

	bool							m_bIsAutoDetectMonoPage;
	bool							m_bIsAutoDetectBlankPage;
};

template< typename T >
void DeleteScanedImage( T** ppImage )
{
	if ( ppImage != NULL && *ppImage != NULL ){
		if ( !(*ppImage)->IsRelegationOwner() ){
			(*ppImage)->DeleteInstance();
			*ppImage = NULL;
		}
	}
}
