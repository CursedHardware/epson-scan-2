////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScannedImage.cpp
//!
//! @brif     スキャン画像クラスの実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommonUtility/utils/PathUtils.h"
#include "CommonUtility/utils/ESFile.h"

#include "ESScannedImage.h"


//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESScannedImage::CESScannedImage()
{
	m_pDelegate = NULL;

	m_eSurfaceType			= kESSurfaceTypeFrontSide;
	m_eDocumentType			= kESDocumentTypeStandard;
	m_eDigitalICEImageType	= kESDigitalICEImageTypeNone;
	m_eImageFormat			= kESImageFormatRaw;
	m_eStorageType			= kESStorageTypeFile;

	m_un32SerialNumber		= 0;
	m_un32PaperSerialNumber	= 0;

	m_nExpectedWidth		= 0;
	m_nExpectedHeight		= 0;

	m_nWidth				= 0;
	m_nHeight				= 0;
	m_nBytesPerRow			= 0;
	m_nBitsPerSample		= 0;
	m_nSamplesPerPixel		= 0;
	m_nProcessedLength		= 0;

	m_bClosed = false;

	m_pFile = NULL;
	m_strFilePath = ES_STRING("");
	m_strDumpLogFileName = ES_STRING("");

	m_bIsRelegationOwner	= false;
	m_bAutoCropped			= false;
	m_bIsUpsideDown			= false;
	m_bIsDoubleFeed			= false;
	m_bIsAutoDetectMonoPage	= false;
	m_bIsAutoDetectBlankPage= false;
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESScannedImage::~CESScannedImage()
{
	if ( m_pFile != NULL ){
		delete m_pFile;
		m_pFile = NULL;
	}
	if ( !m_strFilePath.empty() && ES_CMN_FUNCS::PATH::ES_IsExistFile( m_strFilePath ) ){
		::DeleteFile( m_strFilePath.c_str() );
		m_strFilePath = ES_STRING("");
	}
}

//===============================================================
//!
//! インスタンスを削除する
//!
//===============================================================
void CESScannedImage::DeleteInstance()
{
	delete this;
}

//===============================================================
// インスタンスの管理を呼び出し側に移動します。
// 呼び出し側では、IESScannerDelegate::DidCompleteScanningWithError以降で
// DeleteInstance()を呼び出しインスタンスを削除してください
//===============================================================
void CESScannedImage::RelegationOwner()
{
	m_bIsRelegationOwner = TRUE;
}
bool CESScannedImage::IsRelegationOwner()
{
	return m_bIsRelegationOwner;
}


//===============================================================
//!
//! Delegateをセットする
//!
//===============================================================
ESErrorCode CESScannedImage::SetDelegate( IESScannedImageDelegate* pDelegate )
{
	m_pDelegate = pDelegate;
	return kESErrorNoError;
}

//===============================================================
//!
//! 設定値を初期化する
//!
//===============================================================
ESErrorCode CESScannedImage::Init( 
	ESNumber nWidth, 
	ESNumber nHeight, 
	ESNumber nBytesPerRow, 
	ESNumber nBitsPerSample, 
	ESNumber nSamplesPerPixel,
	ESString strWorkFolder )
{
	const LPCTSTR ES_TMP_PREFIX = _T("ESN");

	if ( m_pFile != NULL ){
		// 初期化済み
		return kESErrorSequenceError;
	}

	m_nExpectedWidth		= nWidth;
	m_nExpectedHeight		= nHeight;
	m_nWidth				= nWidth;
	m_nHeight				= nHeight;
	m_nBytesPerRow			= nBytesPerRow;
	m_nBitsPerSample		= nBitsPerSample;
	m_nSamplesPerPixel		= nSamplesPerPixel;

	if ( m_eStorageType == kESStorageTypeFile){
		m_pFile = CESFile::CreateTempFileInstanceWithPrefix( strWorkFolder, ES_TMP_PREFIX, CESFile::ES_OPEN_MODE_WRITE_PLUS );
		if ( m_pFile == NULL ){
			return kESErrorFileWriteError;
		}
		m_strFilePath = m_pFile->GetFileName();
	}
	return kESErrorNoError;
}

//
void CESScannedImage::Open()
{
	if ( AfxGetLog()->IsEnableDumpImage() ){
		ESstringstream ss;
		ss << L"PaperSerial_" << m_un32PaperSerialNumber << L"_Serial_" << m_un32SerialNumber;
		m_strDumpLogFileName = ss.str();
		AfxGetLog()->InitImage( m_strDumpLogFileName.c_str(), ENUM_IMAGE_TYPE::LogImageUnknown, 0, 0, 0 );
	}

	m_nProcessedLength = 0;
	if ( m_pDelegate ){
		m_pDelegate->ScannedImageDidBeginReceiving( this );
	}
}

void CESScannedImage::Close()
{
	CloseWithWidth( m_nWidth, m_nHeight );
}

void CESScannedImage::CloseWithWidth( ESNumber nWidth, ESNumber nHeight )
{
	m_nWidth  = nWidth;
	m_nHeight = nHeight;

	if ( (m_eStorageType == kESStorageTypeFile) && (m_pFile != NULL) ){
		m_pFile->SeekToHead();
	}
	m_bClosed = true;

	if ( m_pDelegate ){
		m_pDelegate->ScannedImageDidEndReceiving( this );
	}
	if ( IsEnableDumpCommand() ){
		ES_INFO_LOG( ES_STRING("Image Width             = %d"), m_nWidth );
		ES_INFO_LOG( ES_STRING("Image Height            = %d"), m_nHeight );
		ES_INFO_LOG( ES_STRING("Image Bytes Per Row     = %d"), m_nBytesPerRow );
		ES_INFO_LOG( ES_STRING("Image Bits Per Sample   = %d"), m_nBitsPerSample );
		ES_INFO_LOG( ES_STRING("Image Samples Per Pixel = %d"), m_nSamplesPerPixel );
		ES_INFO_LOG( ES_STRING("Image Processed Length  = %lld"), m_nProcessedLength );
	}
}

bool CESScannedImage::IsClosed()
{
	return m_bClosed;
}

void CESScannedImage::Abort()
{
	m_bClosed = true;
	if ( m_pDelegate ){
		m_pDelegate->ScannedImageDidAbortReceiving( this );
	}
}

//
void CESScannedImage::SeekToOffset( UInt32 un32Offset )
{
	if ( (m_eStorageType == kESStorageTypeFile) && (m_pFile != NULL) ){
		m_pFile->SeekToFileOffset( un32Offset );
	}
}

//
ESErrorCode CESScannedImage::WriteData( ES_CMN_FUNCS::BUFFER::CESHeapBuffer& cImageData )
{
	if ( !m_strDumpLogFileName.empty() ){
		AfxGetLog()->DumpImage( m_strDumpLogFileName.c_str(), ENUM_IMAGE_TYPE::LogImageUnknown, 0, cImageData );
	}

	if ( (m_eStorageType == kESStorageTypeFile) && (m_pFile != NULL) && ( cImageData.GetLength() > 0 ) ){
		if ( !m_pFile->WriteData( cImageData ) ){
			return kESErrorFileWriteError;
		}
	}
	m_nProcessedLength += cImageData.GetLength();

	if ( m_pDelegate ){
		m_pDelegate->ScannedImageDidReceiveImageData( this, &cImageData );
	}else{
		cImageData.FreeBuffer();
	}
	return kESErrorNoError;
}

//
void CESScannedImage::ReadDataOfLength( UInt32 un32Length, ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData )
{
	if ( (m_eStorageType == kESStorageTypeFile) && (m_pFile != NULL) ){
		m_pFile->ReadDataOfLength( un32Length, cReadImageData );
	}
}

//
void CESScannedImage::ReadDataToEnd( ES_CMN_FUNCS::BUFFER::IESBuffer& cReadImageData )
{
	cReadImageData.FreeBuffer();
	if ( (m_eStorageType == kESStorageTypeFile) && (m_pFile != NULL) ){
		m_pFile->ReadDataToEndOfFile( cReadImageData );
	}
}

//
UInt32 CESScannedImage::GetSerialNumber()
{
	return m_un32SerialNumber;
}

void CESScannedImage::SetSerialNumber( UInt32 un32Number )
{
	m_un32SerialNumber = un32Number;
}

//
UInt32 CESScannedImage::GetPaperSerialNumber()
{
	return m_un32PaperSerialNumber;
}

void CESScannedImage::SetPaperSerialNumber( UInt32 un32Number )
{
	m_un32PaperSerialNumber = un32Number;
}

//
ENUM_ES_SURFACE_TYPE CESScannedImage::GetSurfaceType()
{
	return m_eSurfaceType;
}
void CESScannedImage::SetSurfaceType( ENUM_ES_SURFACE_TYPE eSurfaceType )
{
	m_eSurfaceType = eSurfaceType;
}

ENUM_ES_DOCUMENT_TYPE CESScannedImage::GetDocumentType()
{
	return m_eDocumentType;
}

void CESScannedImage::SetDocumentType( ENUM_ES_DOCUMENT_TYPE eDocumentType )
{
	m_eDocumentType = eDocumentType;
}


bool CESScannedImage::IsProgressAvailable()
{
	return true;
}

ESNumber CESScannedImage::GetProgressInPercent()
{
	if ( m_eImageFormat == kESImageFormatRaw) {
		return (ESNumber)(((float) m_nProcessedLength / (float)( (SInt64)m_nBytesPerRow * m_nHeight)) * 100);
	}
	return -1;
}

//
ENUM_ES_DIGITAL_ICE_IMAGE_TYPE CESScannedImage::GetDigitalICEImageType()
{
	return m_eDigitalICEImageType;
}
void CESScannedImage::SetDigitalICEImageType( ENUM_ES_DIGITAL_ICE_IMAGE_TYPE eDigitalICEImageType )
{
	m_eDigitalICEImageType = eDigitalICEImageType;
}

//
ESImageFormat CESScannedImage::GetImageFormat()
{
	return m_eImageFormat;
}
void CESScannedImage::SetImageFormat( ESImageFormat eImageFormat )
{
	m_eImageFormat = eImageFormat;
}

//
ENUM_ES_STORAGE_TYPE CESScannedImage::GetStorageType()
{
	return m_eStorageType;
}
void CESScannedImage::SetStorageType( ENUM_ES_STORAGE_TYPE eStorageType )
{
	m_eStorageType = eStorageType;
}

//
bool CESScannedImage::IsAutoCropped()
{
	return m_bAutoCropped;
}
void CESScannedImage::SetAutoCropped( bool bAutoCropped )
{
	m_bAutoCropped = bAutoCropped;
}

//
bool CESScannedImage::IsUpsideDown()
{
	return m_bIsUpsideDown;
}
void CESScannedImage::SetUpsideDown( bool bIsUpsideDown )
{
	m_bIsUpsideDown = bIsUpsideDown;
}

ST_ES_SIZE_UN32 CESScannedImage::GetExpectedImageSize()
{
	return MakeSize<UInt32>(m_nExpectedWidth, m_nExpectedHeight);
}

//
bool CESScannedImage::IsDoubleFeed()
{
	return m_bIsDoubleFeed;
}
void CESScannedImage::SetDoubleFeed(bool bIsDoubleFeed)
{
	m_bIsDoubleFeed = bIsDoubleFeed;
}

//
bool CESScannedImage::IsAutoDetectMonoPage()
{
	return m_bIsAutoDetectMonoPage;
}
void CESScannedImage::SetAutoDetectMonoPage(bool bValue)
{
	m_bIsAutoDetectMonoPage = bValue;
}

bool CESScannedImage::IsAutoDetectBlankPage()
{
	return m_bIsAutoDetectBlankPage;
}

void CESScannedImage::SetAutoDetectBlankPage(bool bValue)
{
	m_bIsAutoDetectBlankPage = bValue;
}

//
ST_ES_SIZE_UN32 CESScannedImage::GetImageSize()
{
	return MakeSize<UInt32>(GetWidth(), GetHeight());
}
void CESScannedImage::SetImageSize( const ST_ES_SIZE_UN32& size )
{
	m_nWidth  = size.cx;
	m_nHeight = size.cy;
}

//
ESNumber CESScannedImage::GetWidth()
{
	return m_nWidth;
}
ESNumber CESScannedImage::GetHeight()
{
	return m_nHeight;
}
ESNumber CESScannedImage::GetBitsPerPixel()
{
	return GetBitsPerSample() * GetSamplesPerPixel();
}
ESNumber CESScannedImage::GetSamplesPerPixel()
{
	return m_nSamplesPerPixel;
}
ESNumber CESScannedImage::GetBitsPerSample()
{
	return m_nBitsPerSample;
}
ESNumber CESScannedImage::GetBytesPerRow()
{
	return m_nBytesPerRow;
}
