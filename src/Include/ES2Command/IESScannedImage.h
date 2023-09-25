////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     IESScannedImage.h
//!
//! @brif     スキャン画像 インターフェースの定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../es2CommnoType.h"
#include "../IESBuffer.h"
#include "ESCommandProperties.h"

typedef enum {
    kESSurfaceTypeFrontSide   = 0,
    kESSurfaceTypeBackSide    = 1,
} ENUM_ES_SURFACE_TYPE;


typedef enum {
    kESDocumentTypeStandard         = 0,
    kESDocumentTypeCarrierSheet     = 1,
} ENUM_ES_DOCUMENT_TYPE;

typedef enum {
    kESDigitalICEImageTypeNone            = 0,
    kESDigitalICEImageTypeTransparent     = 1,
    kESDigitalICEImageTypeTransparentIR   = 2,
    kESDigitalICEImageTypeFlatbedLamp1    = 3,
    kESDigitalICEImageTypeFlatbedLamp2    = 4,
} ENUM_ES_DIGITAL_ICE_IMAGE_TYPE;

typedef enum {
    kESStorageTypeFile  = 0,//ファイル形式でデータの蓄積を行う
    kESStorageTypeNone  = 1,// データの蓄積を行わない
} ENUM_ES_STORAGE_TYPE;


class IESScannedImage;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// スキャン画像への変更を通知するDelegate
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IESScannedImageDelegate
{
public:
	virtual void CALLBACK ScannedImageDidBeginReceiving( IESScannedImage* pScannedImage ) = 0;
	virtual void CALLBACK ScannedImageDidEndReceiving( IESScannedImage* pScannedImage ) = 0;
	virtual void CALLBACK ScannedImageDidAbortReceiving(IESScannedImage* pScannedImage) = 0;
	virtual void CALLBACK ScannedImageDidReceiveImageData( IESScannedImage* pScannedImage, ES_CMN_FUNCS::BUFFER::IESBuffer* pDidReceiveImageData ) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//　スキャン画像 インターフェース
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IESScannedImage {
public:

	// インスタンスを削除する
	virtual void DeleteInstance() = 0;

	// インスタンスの管理を呼び出し側に移動します。
	// 呼び出し側では、IESScannerDelegate::DidCompleteScanningWithError以降で
	// DeleteInstance()を呼び出しインスタンスを削除してください
	virtual void RelegationOwner() = 0;
	virtual bool IsRelegationOwner() = 0;

	//
	// Delegateをセットする
	//
	virtual ESErrorCode SetDelegate( IESScannedImageDelegate* pDelegate ) = 0;

	// 
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual void CloseWithWidth( ESNumber nWidth, ESNumber nHeight ) = 0;
	virtual bool IsClosed() = 0;
	virtual void Abort() = 0;

	//
	virtual void SeekToOffset( UInt32 un32Offset ) = 0;
	virtual void ReadDataOfLength( UInt32 un32Length, ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData ) = 0;
	virtual void ReadDataToEnd( ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData ) = 0;

	//
	virtual UInt32 GetSerialNumber() = 0;
	virtual UInt32 GetPaperSerialNumber() = 0;			// The paired frontside and backside images will have the same paperSerialNumber.
	virtual ENUM_ES_SURFACE_TYPE GetSurfaceType() = 0;
	virtual ENUM_ES_DOCUMENT_TYPE GetDocumentType() =0;
	virtual bool IsProgressAvailable() = 0;
	virtual ESNumber GetProgressInPercent() = 0;		// Returns negative value if progress is unavailable.
	virtual bool IsAutoCropped() = 0;
	virtual bool IsUpsideDown() = 0;
	virtual bool IsDoubleFeed() = 0;			// 重送検知された画像かどうかを示す
	virtual bool IsAutoDetectMonoPage() = 0;
	
	virtual bool IsAutoDetectBlankPage() = 0;

	//
	virtual ESImageFormat GetImageFormat() = 0;
	virtual ENUM_ES_DIGITAL_ICE_IMAGE_TYPE GetDigitalICEImageType() = 0;
	virtual ENUM_ES_STORAGE_TYPE GetStorageType() = 0;
	virtual void SetStorageType( ENUM_ES_STORAGE_TYPE eStorageType ) = 0;

	virtual ST_ES_SIZE_UN32 GetExpectedImageSize() = 0;	// スキャン開始時に期待される画像サイズ。
	virtual ST_ES_SIZE_UN32 GetImageSize() = 0;			// 実際にスキャンされた画像サイズ。close時には確定する。

	//
	virtual ESNumber GetWidth() = 0;
	virtual ESNumber GetHeight() = 0;
	virtual ESNumber GetBitsPerPixel() = 0;
	virtual ESNumber GetSamplesPerPixel() = 0;
	virtual ESNumber GetBitsPerSample() = 0;
	virtual ESNumber GetBytesPerRow() = 0;
};