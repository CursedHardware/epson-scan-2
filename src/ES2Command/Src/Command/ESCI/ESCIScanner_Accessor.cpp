////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCIScanner_Accessor.cpp
//!
//! @brif     ESCIコマンド用スキャナクラス - 設定値アクセス処理部の実装
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESCIScanner.h"

//=========================================================================
//!
//! 指定したキーの設定値を取得する
//!
//=========================================================================
ESErrorCode CESCIScanner::GetValueForKey( ES_CHAR_CPTR pszKey, ESAny& anyValue )
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
ESErrorCode CESCIScanner::SetValueForKey( ES_CHAR_CPTR pszKey, ESAny anyValue )
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
ESString CESCIScanner::GetWorkFolder()
{
	return CESCIAccessor::GetWorkFolder();
}

//=========================================================================
//!
//! WorkFolderを設定する
//!
//=========================================================================
ESErrorCode CESCIScanner::SetWorkFolder( ESString strWorkFolder )
{
	return CESCIAccessor::SetWorkFolder(strWorkFolder);
}

//=========================================================================
//!
//! エラーステータスを取得する。
//!
//=========================================================================
ESErrorCode CESCIScanner::GetErrorStatus()
{
	return CESCIAccessor::GetErrorStatus();
}

//===============================================================
//!
//! バッファサイズを取得する
//!
//===============================================================
ESNumber CESCIScanner::GetBufferSize()
{
	return CESCIAccessor::GetBufferSize();
}

//===============================================================
//!
//! バッファサイズを設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetBufferSize( ESNumber nBufferSize )
{
	return CESCIAccessor::SetBufferSize( nBufferSize );
}

//===============================================================
//!
//! 認証機能をサポートしているか？
//!
//===============================================================
bool CESCIScanner::IsAuthenticationSupported()
{
	return CESCIAccessor::IsAuthenticationSupported();
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを取得する
//!
//===============================================================
bool CESCIScanner::IsAuthenticationEnabled()
{
	return CESCIAccessor::IsAuthenticationEnabled();
}

//===============================================================
//!
//! 認証機能が有効状態かどうかを設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetAuthenticationEnabled( bool bEnabled )
{
	return CESCIAccessor::SetAuthenticationEnabled( bEnabled );
}

//===============================================================
//!
//! 認証ユーザー名を取得する
//!
//===============================================================
ESString CESCIScanner::GetAuthUserName()
{
	return CESCIAccessor::GetAuthUserName();
}

//===============================================================
//!
//! 認証ユーザー名を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetAuthUserName( ESString strAuthUserName )
{
	return CESCIAccessor::SetAuthUserName( strAuthUserName );
}

//===============================================================
//!
//! 認証パスワードを取得する
//!
//===============================================================
ESString CESCIScanner::GetAuthPassword()
{
	return CESCIAccessor::GetAuthPassword();
}

//===============================================================
//!
//! 認証パスワードを設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetAuthPassword( ESString strAuthPassword )
{
	return CESCIAccessor::SetAuthPassword( strAuthPassword );
}

//===============================================================
//!
//! プロダクト名を取得する
//!
//===============================================================
ESString CESCIScanner::GetProductName()
{
	return CESCIAccessor::GetProductName();
}

//===============================================================
//!
//! サポートする給紙タイプを取得する
//!
//===============================================================
ESIndexSet CESCIScanner::GetSupportedFunctionalUnitTypes()
{
	return CESCIAccessor::GetSupportedFunctionalUnitTypes();
}

//===============================================================
//!
//! 給紙タイプを取得する
//!
//===============================================================
ESFunctionalUnitType CESCIScanner::GetFunctionalUnitType()
{
	return CESCIAccessor::GetFunctionalUnitType();
}

//===============================================================
//!
//! 給紙タイプを設定する (ESFunctionalUnitType)
//!
//===============================================================
ESErrorCode CESCIScanner::SetFunctionalUnitType( ESNumber nFunctionalUnitType )
{
	return CESCIAccessor::SetFunctionalUnitType( nFunctionalUnitType );
}

//===============================================================
//!
//! サポートする解像度を取得する (ESIndexSet or ST_ES_RANGE)
//!
//===============================================================
ESAny CESCIScanner::GetSupportedXResolutions()
{
	return CESCIAccessor::GetSupportedXResolutions();
}
ESAny CESCIScanner::GetSupportedYResolutions()
{
	return CESCIAccessor::GetSupportedYResolutions();
}

//===============================================================
//!
//! 解像度を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetXResolution()
{
	return CESCIAccessor::GetXResolution();
}

ESNumber CESCIScanner::GetYResolution()
{
	return CESCIAccessor::GetYResolution();
}

//===============================================================
//!
//! 解像度を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetXResolution( ESNumber xResolution )
{
	return CESCIAccessor::SetXResolution( xResolution );
}

ESErrorCode CESCIScanner::SetYResolution( ESNumber yResolution )
{
	return CESCIAccessor::SetYResolution( yResolution );
}

//===============================================================
//!
//! 光学解像度を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetOpticalResolution()
{
	return CESCIAccessor::GetOpticalResolution();
}

//===============================================================
//!
//! 最大スキャンサイズを取得する(インチ） 
//!
//===============================================================
ST_ES_SIZE_F CESCIScanner::GetMaxScanSize()
{
	return CESCIAccessor::GetMaxScanSize();
}

//===============================================================
//!
//! スキャンするエリアを取得する（インチ）
//!
//===============================================================
ST_ES_RECT_F CESCIScanner::GetScanArea()
{
	return CESCIAccessor::GetScanArea();
}

//===============================================================
//!
//! スキャンするエリアを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCIScanner::SetScanArea(ST_ES_RECT_F rcScanArea)
{
	return CESCIAccessor::SetScanArea( rcScanArea );
}


//===============================================================
//!
//! スキャンするエリアをピクセル単位で取得する
//!
//===============================================================
ST_ES_RECT_UN32 CESCIScanner::GetScanAreaInPixel()
{
	return CESCIAccessor::GetScanAreaInPixel();
}

//===============================================================
//!
//! スキャンするエリアをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetScanAreaInPixel( ST_ES_RECT_UN32 rcUn32ScanAreaInPixel)
{
	return CESCIAccessor::SetScanAreaInPixel( rcUn32ScanAreaInPixel );
}

//===============================================================
//!
//! スキャンするサイズを取得する（インチ）
//!
//===============================================================
ST_ES_SIZE_F CESCIScanner::GetScanSize()
{
	return CESCIAccessor::GetScanSize();
}

//===============================================================
//!
//! スキャンするサイズを設定する（インチ）
//!
//===============================================================
ESErrorCode CESCIScanner::SetScanSize( ST_ES_SIZE_F sizeScan )
{
	return CESCIAccessor::SetScanSize( sizeScan );
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で取得する
//!
//===============================================================
ST_ES_SIZE_UN32 CESCIScanner::GetScanSizeInPixel()
{
	return CESCIAccessor::GetScanSizeInPixel();
}

//===============================================================
//!
//! スキャンするサイズをピクセル単位で設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetScanSizeInPixel( ST_ES_SIZE_UN32 sizeScan )
{
	return CESCIAccessor::SetScanSizeInPixel( sizeScan );
}

//===============================================================
//!
//! オフセットマージン
//!
//===============================================================
ESFloat CESCIScanner::GetXOffsetMargin()
{
	return CESCIAccessor::GetXOffsetMargin();
}
ESErrorCode CESCIScanner::SetXOffsetMargin( ESFloat fMargin )
{
	return CESCIAccessor::SetXOffsetMargin( fMargin );
}
ESFloat CESCIScanner::GetYOffsetMargin()
{
	return CESCIAccessor::GetYOffsetMargin();
}
ESErrorCode CESCIScanner::SetYOffsetMargin( ESFloat fMargin )
{
	return CESCIAccessor::SetYOffsetMargin( fMargin );
}

//===============================================================
//!
//! サポートするカラーモードを取得する
//!
//===============================================================
ESIndexSet CESCIScanner::GetSupportedColorFormats()
{
	return CESCIAccessor::GetSupportedColorFormats();
}

//===============================================================
//!
//! カラーフォーマットを取得する
//!
//! @return ESColorFormat, 不明時: 0
//!
//===============================================================
ESNumber CESCIScanner::GetColorFormat()
{
	return CESCIAccessor::GetColorFormat();
}

//===============================================================
//!
//! カラーフォーマットを設定する(ESColorFormat)
//!
//===============================================================
ESErrorCode CESCIScanner::SetColorFormat( ESNumber nColorFormat )
{
	return CESCIAccessor::SetColorFormat( nColorFormat );
}

//===============================================================
//!
//! サンプル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetBitsPerSample()
{
	return CESCIAccessor::GetBitsPerSample();
}

//===============================================================
//!
//! 1ピクセル当たりのサンプル数を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetSamplesPerPixel()
{
	return CESCIAccessor::GetSamplesPerPixel();
}

//===============================================================
//!
//! 1ピクセル当たりのbit数を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetBitsPerPixel()
{
	return CESCIAccessor::GetBitsPerPixel();
}

//===============================================================
//!
//! カラーマトリックスを取得する(ESFloatArray or nullptr)
//!
//===============================================================
ESAny CESCIScanner::GetColorMatrix()
{
	return CESCIAccessor::GetColorMatrix();
}

//===============================================================
//!
//! カラーマトリックスを設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetColorMatrix( ESFloatArray arColorMatrix )
{
	return CESCIAccessor::SetColorMatrix( arColorMatrix );
}

//===============================================================
//!
//! サポートするガンマモードを取得する
//!
//===============================================================
ESIndexSet CESCIScanner::GetSupportedGammaModes()
{
	return CESCIAccessor::GetSupportedGammaModes();
}

//===============================================================
//!
//! ガンマモードを取得する
//!
//===============================================================
ESGammaMode CESCIScanner::GetGammaMode()
{
	return CESCIAccessor::GetGammaMode();
}

//===============================================================
//!
//! ガンマモードを設定する(ESGammaMode)
//!
//===============================================================
ESErrorCode CESCIScanner::SetGammaMode( ESNumber nGammaMode )
{
	return CESCIAccessor::SetGammaMode( nGammaMode );
}

//===============================================================
//!
//! ガンマテーブルを取得する
//!
//===============================================================
ESAny CESCIScanner::GetGammaTableMono()
{
	return CESCIAccessor::GetGammaTableMono();
}
ESAny CESCIScanner::GetGammaTableRed()
{
	return CESCIAccessor::GetGammaTableRed();
}
ESAny CESCIScanner::GetGammaTableGreen()
{
	return CESCIAccessor::GetGammaTableGreen();
}
ESAny CESCIScanner::GetGammaTableBlue()
{
	return CESCIAccessor::GetGammaTableBlue();
}

//===============================================================
//!
//! ガンマテーブルを設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetGammaTableMono( ESIndexArray arGammaTable )
{
	return CESCIAccessor::SetGammaTableMono( arGammaTable );
}
ESErrorCode CESCIScanner::SetGammaTableRed( ESIndexArray arGammaTable )
{
	return CESCIAccessor::SetGammaTableRed( arGammaTable );
}
ESErrorCode CESCIScanner::SetGammaTableGreen( ESIndexArray arGammaTable )
{
	return CESCIAccessor::SetGammaTableGreen( arGammaTable );
}
ESErrorCode CESCIScanner::SetGammaTableBlue( ESIndexArray arGammaTable )
{
	return CESCIAccessor::SetGammaTableBlue( arGammaTable );
}

//===============================================================
//!
//! サポートするモノクロ閾値を取得する( ESIndexSet or ST_ES_RANGE )
//!
//===============================================================
ESAny CESCIScanner::GetSupportedThreshold()
{
	return CESCIAccessor::GetSupportedThreshold();
}

//===============================================================
//!
//! モノクロ閾値を取得する
//! 
//===============================================================
ESNumber CESCIScanner::GetThreshold()
{
	return CESCIAccessor::GetThreshold();
}

//===============================================================
//!
//! モノクロ閾値を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetThreshold( ESNumber nThreshold )
{
	return CESCIAccessor::SetThreshold( nThreshold );
}

//===============================================================
//!
//! サポートする静音モードを取得する
//!
//===============================================================
ESIndexSet CESCIScanner::GetSupportedQuietModes()
{
	return CESCIAccessor::GetSupportedQuietModes();
}

//===============================================================
//!
//!　静音モードを取得する (ESQuietMode)
//!
//===============================================================
ESQuietMode CESCIScanner::GetQuietMode()
{
	return CESCIAccessor::GetQuietMode();
}

//===============================================================
//!
//! 静音モードを設定する (ESQuietMode)
//!
//===============================================================
ESErrorCode CESCIScanner::SetQuietMode( ESNumber nQuietMode )
{
	return CESCIAccessor::SetQuietMode( nQuietMode );
}

//===============================================================
//!
//! フィーダー(ADF)機能をサポートしているか?
//!
//===============================================================
bool CESCIScanner::IsFeederSupported()
{
	return CESCIAccessor::IsFeederSupported();
}

//===============================================================
//!
//! フィーダー(ADF)機能が有効か？
//!
//===============================================================
bool CESCIScanner::IsFeederEnabled()
{
	return CESCIAccessor::IsFeederEnabled();
}

//===============================================================
//!
//! 両面同時読み取りの種類を取得する
//!
//===============================================================
ESDuplexType CESCIScanner::GetDuplexType()
{
	return CESCIAccessor::GetDuplexType();
}

//===============================================================
//!
//! 両面同時読み取りをサポートしているか?
//!
//===============================================================
bool CESCIScanner::IsDuplexSupported()
{
	return CESCIAccessor::IsDuplexSupported();
}

//===============================================================
//!
//! 両面同時読み取り機能が有効か?
//!
//===============================================================
bool CESCIScanner::IsDuplexEnabled()
{
	return CESCIAccessor::IsDuplexEnabled();
}

//===============================================================
//!
//! 両面同時読み取り機能の有効状態を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetDuplexEnabled( bool bDuplexEnabled )
{
	return CESCIAccessor::SetDuplexEnabled( bDuplexEnabled );
}

//===============================================================
//!
//! サポートしている重送検知を取得する
//!
//===============================================================
ESIndexSet CESCIScanner::GetSupportedDoubleFeedDetections()
{
	return CESCIAccessor::GetSupportedDoubleFeedDetections();
}

//===============================================================
//!
//! 重送検知の設定を取得する
//!
//===============================================================
ESDoubleFeedDetection CESCIScanner::GetDoubleFeedDetection()
{
	return CESCIAccessor::GetDoubleFeedDetection();
}

//===============================================================
//!
//! 重送検知を設定を変更する(ESDoubleFeedDetection)
//!
//===============================================================
ESErrorCode CESCIScanner::SetDoubleFeedDetection( ESNumber nDoubleFeedDetection )
{
	return CESCIAccessor::SetDoubleFeedDetection( nDoubleFeedDetection );
}

//===============================================================
//!
//!ドキュメントが読み込まれたか？
//!
//===============================================================
bool CESCIScanner::IsDocumentLoaded()
{
	return CESCIAccessor::IsDocumentLoaded();
}

//===============================================================
//!
//! スキャンするページ枚数を取得する
//!
//===============================================================
ESNumber CESCIScanner::GetPagesToBeScanned()
{
	return CESCIAccessor::GetPagesToBeScanned();
}

//===============================================================
//!
//! スキャンするページ枚数を設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetPagesToBeScanned( ESNumber nPagesToBeScanned )
{
	return CESCIAccessor::SetPagesToBeScanned( nPagesToBeScanned );
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIScanner::IsCleaningSupported()
{
	return CESCIAccessor::IsCleaningSupported();
}

//===============================================================
//!
//!
//!
//===============================================================
bool CESCIScanner::IsCalibrationSupported()
{
	return CESCIAccessor::IsCleaningSupported();
}

//===============================================================
//!
//! キャプチャー機能をサポートするか？
//!
//===============================================================
bool CESCIScanner::IsCaptureCommandSupported()
{
	return CESCIAccessor::IsCaptureCommandSupported();
}

//===============================================================
//!
//! キャプチャーコマンドをサポートしているか設定する
//!
//===============================================================
ESErrorCode CESCIScanner::SetCaptureCommandSupported( bool bSupported )
{
	return CESCIAccessor::SetCaptureCommandSupported( bSupported );
}