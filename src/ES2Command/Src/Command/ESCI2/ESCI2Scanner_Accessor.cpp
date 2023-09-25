////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCI2Scanner_Accessor.cpp
//!
//! @brif     ESCI/2コマンド用スキャナクラス - 設定値アクセス処理部の実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESCI2Scanner.h"

//=========================================================================
//!
//! 指定したキーの設定値を取得する
//!
//=========================================================================
ESErrorCode CESCI2Scanner::GetValueForKey( ES_CHAR_CPTR pszKey, ESAny& anyValue )
{
	ES_LOG_TRACE_FUNC();

	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	const ESStringArray& arKeyList = GetAllKeys();
	ESStringArray::const_iterator it = std::find( arKeyList.begin(), arKeyList.end(), pszKey );
	if ( it == arKeyList.end() ){
		ES_WARM_LOG( ES_STRING("Unsuported Key : %s"), pszKey );
		anyValue = nullptr;
		return kESErrorInvalidParameter;
	} else {
		ESDictionary dicAllValue;
		GetAllValuesForKey( pszKey, dicAllValue );
		if ( dicAllValue.empty() ){
			ES_WARM_LOG( ES_STRING("Unsuported Key : %s"), pszKey );
			anyValue = nullptr;
			return kESErrorInvalidParameter;
		}
	}

	ES_INFO_LOG( ES_STRING("Key = %s"), pszKey );
	ESErrorCode err = kESErrorNoError;
	if ( m_mapAccessor.count( pszKey ) != 0 ){
		err = m_mapAccessor[ pszKey ].GetValue( anyValue );
	} else {
		err = CESScanner::GetValueForKey( pszKey, anyValue );
	}
	return err;
}

//===============================================================
//!
//! 指定したキーの値を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetValueForKey( ES_CHAR_CPTR pszKey, ESAny anyValue )
{
	ES_LOG_TRACE_FUNC();

	if ( pszKey == NULL || pszKey[0] == ES_STRING('\0') ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}

	const ESStringArray& arKeyList = GetAllKeys();
	ESStringArray::const_iterator it = std::find( arKeyList.begin(), arKeyList.end(), pszKey );
	if ( it == arKeyList.end() ){
		ES_WARM_LOG( ES_STRING("Unsuported Key : %s"), pszKey );
		return kESErrorInvalidParameter;
	} else {
		ESDictionary dicAvailableValue;
		GetAvailableValuesForKey( pszKey, dicAvailableValue );
		if ( dicAvailableValue.empty() ){
			ES_WARM_LOG( ES_STRING("Unavailable key: %s"), pszKey );
			return kESErrorInvalidParameter;
		}
	}

	ES_INFO_LOG( ES_STRING("Key = %s"), pszKey );
	ESErrorCode err = kESErrorNoError;
	if ( m_mapAccessor.count( pszKey ) != 0 ){
		err = m_mapAccessor[ pszKey ].SetValue( anyValue );
		if ( IS_ERR_CODE( err ) ){
			ES_ERROR_LOG( ES_STRING("Failed set value for %s key."), pszKey );
		}
	} else {
		err = CESScanner::SetValueForKey( pszKey, anyValue );
	}
	return err;
}

//=========================================================================
//!
//! WorkFolderを取得する
//!
//=========================================================================
ESString CESCI2Scanner::GetWorkFolder()
{
	return CESCI2Accessor::GetWorkFolder();
}

//=========================================================================
//!
//! WorkFolderを設定する
//!
//=========================================================================
ESErrorCode CESCI2Scanner::SetWorkFolder( ESString strWorkFolder )
{
	return CESCI2Accessor::SetWorkFolder(strWorkFolder);
}

//=========================================================================
//!
//! エラーステータスを取得する。
//!
//=========================================================================
ESErrorCode CESCI2Scanner::GetErrorStatus()
{
	return CESCI2Accessor::GetErrorStatus();
}

//===============================================================
//!
//! バッファサイズを取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetBufferSize()
{
	return CESCI2Accessor::GetBufferSize();
}

//===============================================================
//!
//! バッファサイズを設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetBufferSize( ESNumber nBufferSize )
{
	return CESCI2Accessor::SetBufferSize( nBufferSize );
}

//===============================================================
//!
//! 認証機能をサポートしているか？
//!
//===============================================================
bool CESCI2Scanner::IsAuthenticationSupported()
{
	return CESCI2Accessor::IsAuthenticationSupported();
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを取得する
//!
//===============================================================
bool CESCI2Scanner::IsAuthenticationEnabled()
{
	return CESCI2Accessor::IsAuthenticationEnabled();
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetAuthenticationEnabled( bool bEnabled )
{
	return CESCI2Accessor::SetAuthenticationEnabled( bEnabled );
}

//===============================================================
//!
//! 認証ユーザー名を取得する
//!
//===============================================================
ESString CESCI2Scanner::GetAuthUserName()
{
	return CESCI2Accessor::GetAuthUserName();
}

//===============================================================
//!
//! 認証ユーザー名を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetAuthUserName( ESString strAuthUserName )
{
	return CESCI2Accessor::SetAuthUserName( strAuthUserName );
}

//===============================================================
//!
//! 認証パスワードを取得する
//!
//===============================================================
ESString CESCI2Scanner::GetAuthPassword()
{
	return CESCI2Accessor::GetAuthPassword();
}

//===============================================================
//!
//! 認証パスワードを設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetAuthPassword( ESString strAuthPassword )
{
	return CESCI2Accessor::SetAuthPassword( strAuthPassword );
}

//===============================================================
//!
//! プロダクト名を取得する
//!
//===============================================================
ESString CESCI2Scanner::GetProductName()
{
	return CESCI2Accessor::GetProductName();
}

//===============================================================
//!
//! サポートする給紙タイプを取得する
//!
//===============================================================
ESIndexSet CESCI2Scanner::GetSupportedFunctionalUnitTypes()
{
	return CESCI2Accessor::GetSupportedFunctionalUnitTypes();
}

//===============================================================
//!
//! 給紙タイプを取得する
//!
//===============================================================
ESFunctionalUnitType CESCI2Scanner::GetFunctionalUnitType()
{
	return CESCI2Accessor::GetFunctionalUnitType();
}

//===============================================================
//!
//! 給紙タイプを設定する (ESFunctionalUnitType)
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetFunctionalUnitType( ESNumber nFunctionalUnitType )
{
	return CESCI2Accessor::SetFunctionalUnitType( nFunctionalUnitType );
}

//===============================================================
//!
//! サポートする解像度を取得する (ESIndexSet or ST_ES_RANGE)
//!
//===============================================================
ESAny CESCI2Scanner::GetSupportedXResolutions()
{
	return CESCI2Accessor::GetSupportedXResolutions();
}
ESAny CESCI2Scanner::GetSupportedYResolutions()
{
	return CESCI2Accessor::GetSupportedYResolutions();
}

//===============================================================
//!
//! 解像度を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetXResolution()
{
	return CESCI2Accessor::GetXResolution();
}

ESNumber CESCI2Scanner::GetYResolution()
{
	return CESCI2Accessor::GetYResolution();
}

//===============================================================
//!
//! 解像度を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetXResolution( ESNumber xResolution )
{
	return CESCI2Accessor::SetXResolution( xResolution );
}

ESErrorCode CESCI2Scanner::SetYResolution( ESNumber yResolution )
{
	return CESCI2Accessor::SetYResolution( yResolution );
}

//===============================================================
//!
//! 光学解像度を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetOpticalResolution()
{
	return CESCI2Accessor::GetOpticalResolution();
}

//===============================================================
//!
//! 最大スキャンサイズを取得する(インチ） 
//!
//===============================================================
ST_ES_SIZE_F CESCI2Scanner::GetMaxScanSize()
{
	return CESCI2Accessor::GetMaxScanSize();
}

//===============================================================
//!
//! スキャンするエリアを取得する（インチ）
//!
//===============================================================
ST_ES_RECT_F CESCI2Scanner::GetScanArea()
{
	return CESCI2Accessor::GetScanArea();
}

//===============================================================
//!
//! スキャンするエリアを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetScanArea(ST_ES_RECT_F rcScanArea)
{
	return CESCI2Accessor::SetScanArea( rcScanArea );
}


//===============================================================
//!
//! スキャンするエリアをピクセル単位で取得する
//!
//===============================================================
ST_ES_RECT_UN32 CESCI2Scanner::GetScanAreaInPixel()
{
	return CESCI2Accessor::GetScanAreaInPixel();
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetScanAreaInPixel( ST_ES_RECT_UN32 rcUn32ScanAreaInPixel)
{
	return CESCI2Accessor::SetScanAreaInPixel( rcUn32ScanAreaInPixel );
}

//===============================================================
//!
//! スキャンするサイズを取得する（インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCI2Scanner::GetScanSize()
{
	return CESCI2Accessor::GetScanSize();
}

//===============================================================
//!
//! スキャンするサイズを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetScanSize( ST_ES_SIZE_F sizeScan )
{
	return CESCI2Accessor::SetScanSize( sizeScan );
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で取得する
//!
//===============================================================
ST_ES_SIZE_UN32 CESCI2Scanner::GetScanSizeInPixel()
{
	return CESCI2Accessor::GetScanSizeInPixel();
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan )
{
	return CESCI2Accessor::SetScanSizeInPixel( sizeScan );
}

//===============================================================
//!
//! オフセットマージン
//!
//===============================================================
ESFloat CESCI2Scanner::GetXOffsetMargin()
{
	return CESCI2Accessor::GetXOffsetMargin();
}
ESErrorCode CESCI2Scanner::SetXOffsetMargin( ESFloat fMargin )
{
	return CESCI2Accessor::SetXOffsetMargin( fMargin );
}
ESFloat CESCI2Scanner::GetYOffsetMargin()
{
	return CESCI2Accessor::GetYOffsetMargin();
}
ESErrorCode CESCI2Scanner::SetYOffsetMargin( ESFloat fMargin )
{
	return CESCI2Accessor::SetYOffsetMargin( fMargin );
}

//===============================================================
//!
//! サポートするカラーモードを取得する
//!
//===============================================================
ESIndexSet CESCI2Scanner::GetSupportedColorFormats()
{
	return CESCI2Accessor::GetSupportedColorFormats();
}

//===============================================================
//!
//! カラーフォーマットを取得する
//!
//! @return ESColorFormat, 不明時: 0
//!
//===============================================================
ESNumber CESCI2Scanner::GetColorFormat()
{
	return CESCI2Accessor::GetColorFormat();
}

//===============================================================
//!
//! カラーフォーマットを設定する(ESColorFormat)
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetColorFormat( ESNumber nColorFormat )
{
	return CESCI2Accessor::SetColorFormat( nColorFormat );
}

//===============================================================
//!
//! サンプル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetBitsPerSample()
{
	return CESCI2Accessor::GetBitsPerSample();
}

//===============================================================
//!
//! 1ピクセル当たりのサンプル数を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetSamplesPerPixel()
{
	return CESCI2Accessor::GetSamplesPerPixel();
}

//===============================================================
//!
//! 1ピクセル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetBitsPerPixel()
{
	return CESCI2Accessor::GetBitsPerPixel();
}

//===============================================================
//!
//! カラーマトリックスを取得する(ESFloatArray or nullptr)
//!
//===============================================================
ESAny CESCI2Scanner::GetColorMatrix()
{
	return CESCI2Accessor::GetColorMatrix();
}

//===============================================================
//!
//! カラーマトリックスを設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetColorMatrix( ESFloatArray arColorMatrix )
{
	return CESCI2Accessor::SetColorMatrix( arColorMatrix );
}

//===============================================================
//!
//! サポートするガンマモードを取得する
//!
//===============================================================
ESIndexSet CESCI2Scanner::GetSupportedGammaModes()
{
	return CESCI2Accessor::GetSupportedGammaModes();
}

//===============================================================
//!
//! ガンマモードを取得する
//!
//===============================================================
ESGammaMode CESCI2Scanner::GetGammaMode()
{
	return CESCI2Accessor::GetGammaMode();
}

//===============================================================
//!
//! ガンマモードを設定する(ESGammaMode)
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetGammaMode( ESNumber nGammaMode )
{
	return CESCI2Accessor::SetGammaMode( nGammaMode );
}

//===============================================================
//!
//! ガンマテーブルを取得する
//!
//===============================================================
ESAny CESCI2Scanner::GetGammaTableMono()
{
	return CESCI2Accessor::GetGammaTableMono();
}
ESAny CESCI2Scanner::GetGammaTableRed()
{
	return CESCI2Accessor::GetGammaTableRed();
}
ESAny CESCI2Scanner::GetGammaTableGreen()
{
	return CESCI2Accessor::GetGammaTableGreen();
}
ESAny CESCI2Scanner::GetGammaTableBlue()
{
	return CESCI2Accessor::GetGammaTableBlue();
}

//===============================================================
//!
//! ガンマテーブルを設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetGammaTableMono( ESIndexArray arGammaTable )
{
	return CESCI2Accessor::SetGammaTableMono( arGammaTable );
}
ESErrorCode CESCI2Scanner::SetGammaTableRed( ESIndexArray arGammaTable )
{
	return CESCI2Accessor::SetGammaTableRed( arGammaTable );
}
ESErrorCode CESCI2Scanner::SetGammaTableGreen( ESIndexArray arGammaTable )
{
	return CESCI2Accessor::SetGammaTableGreen( arGammaTable );
}
ESErrorCode CESCI2Scanner::SetGammaTableBlue( ESIndexArray arGammaTable )
{
	return CESCI2Accessor::SetGammaTableBlue( arGammaTable );
}

//===============================================================
//!
//! サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
//!
//===============================================================
ESAny CESCI2Scanner::GetSupportedThreshold()
{
	return CESCI2Accessor::GetSupportedThreshold();
}

//===============================================================
//!
//! モノクロ閾値を取得する
//! 
//===============================================================
ESNumber CESCI2Scanner::GetThreshold()
{
	return CESCI2Accessor::GetThreshold();
}

//===============================================================
//!
//! モノクロ閾値を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetThreshold( ESNumber nThreshold )
{
	return CESCI2Accessor::SetThreshold( nThreshold );
}

//===============================================================
//!
//! サポートする静音モードを取得する
//!
//===============================================================
ESIndexSet CESCI2Scanner::GetSupportedQuietModes()
{
	return CESCI2Accessor::GetSupportedQuietModes();
}

//===============================================================
//!
//!　静音モードを取得する (ESQuietMode)
//!
//===============================================================
ESQuietMode CESCI2Scanner::GetQuietMode()
{
	return CESCI2Accessor::GetQuietMode();
}

//===============================================================
//!
//! 静音モードを設定する (ESQuietMode)
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetQuietMode( ESNumber nQuietMode )
{
	return CESCI2Accessor::SetQuietMode( nQuietMode );
}

//===============================================================
//!
//! フィーダー(ADF)機能をサポートしているか?
//!
//===============================================================
bool CESCI2Scanner::IsFeederSupported()
{
	return CESCI2Accessor::IsFeederSupported();
}

//===============================================================
//!
//! フィーダー(ADF)機能が有効か？
//!
//===============================================================
bool CESCI2Scanner::IsFeederEnabled()
{
	return CESCI2Accessor::IsFeederEnabled();
}

//===============================================================
//!
//! 両面同時読み取りの種類を取得する
//!
//===============================================================
ESDuplexType CESCI2Scanner::GetDuplexType()
{
	return CESCI2Accessor::GetDuplexType();
}

//===============================================================
//!
//! 両面同時読み取りをサポートしているか?
//!
//===============================================================
bool CESCI2Scanner::IsDuplexSupported()
{
	return CESCI2Accessor::IsDuplexSupported();
}

//===============================================================
//!
//! 両面同時読み取り機能が有効か?
//!
//===============================================================
bool CESCI2Scanner::IsDuplexEnabled()
{
	return CESCI2Accessor::IsDuplexEnabled();
}

//===============================================================
//!
//! 両面同時読み取り機能の有効状態を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetDuplexEnabled( bool bDuplexEnabled )
{
	return CESCI2Accessor::SetDuplexEnabled( bDuplexEnabled );
}

//===============================================================
//!
//! サポートしている重送検知を取得する
//!
//===============================================================
ESIndexSet CESCI2Scanner::GetSupportedDoubleFeedDetections()
{
	return CESCI2Accessor::GetSupportedDoubleFeedDetections();
}

//===============================================================
//!
//! 重送検知の設定を取得する
//!
//===============================================================
ESDoubleFeedDetection CESCI2Scanner::GetDoubleFeedDetection()
{
	return CESCI2Accessor::GetDoubleFeedDetection();
}

//===============================================================
//!
//! 重送検知を設定を変更する(ESDoubleFeedDetection)
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetDoubleFeedDetection( ESNumber nDoubleFeedDetection )
{
	return CESCI2Accessor::SetDoubleFeedDetection( nDoubleFeedDetection );
}

//===============================================================
//!
//!ドキュメントが読み込まれたか？
//!
//===============================================================
bool CESCI2Scanner::IsDocumentLoaded()
{
	return CESCI2Accessor::IsDocumentLoaded();
}

//===============================================================
//!
//! スキャンするページ枚数を取得する
//!
//===============================================================
ESNumber CESCI2Scanner::GetPagesToBeScanned()
{
	return CESCI2Accessor::GetPagesToBeScanned();
}

//===============================================================
//!
//! スキャンするページ枚数を設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetPagesToBeScanned( ESNumber nPagesToBeScanned )
{
	return CESCI2Accessor::SetPagesToBeScanned( nPagesToBeScanned );
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Scanner::IsCleaningSupported()
{
	return CESCI2Accessor::IsCleaningSupported();
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCI2Scanner::IsCalibrationSupported()
{
	return CESCI2Accessor::IsCleaningSupported();
}

//===============================================================
//!
//! キャプチャー機能をサポートするか？
//!
//===============================================================
bool CESCI2Scanner::IsCaptureCommandSupported()
{
	return CESCI2Accessor::IsCaptureCommandSupported();
}

//===============================================================
//!
//! キャプチャーコマンドをサポートしているか設定する
//!
//===============================================================
ESErrorCode CESCI2Scanner::SetCaptureCommandSupported( bool bSupported )
{
	return CESCI2Accessor::SetCaptureCommandSupported( bSupported );
}