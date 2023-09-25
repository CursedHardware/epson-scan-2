////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     CommandBase.cpp
//!
//! @brif     コマンド送受信ベース クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "CommandBase.h"
#include "Include/ES2Command/ES2CommandAPI.h" // IESScannerDelegate
#include "Interface/IInterface.h"
#include "CommonUtility/utils/PathUtils.h"

#if defined(__ANDROID__) || TARGET_OS_IPHONE || defined(TARGET_OS_EMBEDDED)
#include "sha1.h"
#else
#include "../../../thirdparty/sha1/sha1.h"
#endif


//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CCommandBase::CCommandBase()
{
	m_pDevInterface = nullptr;
	m_pDelegate = nullptr;
	m_pScanner = nullptr;

	m_bCancelled = false;
	m_bScanning = false;
	m_bIsInterrupted = false;

	// デフォルトのワークフォルダをシステムのTempフォルダに設定する
	ES_CMN_FUNCS::PATH::ES_GetSystemTempDir( m_strWorkFolder );
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CCommandBase::~CCommandBase()
{
	SYNCHRONIZED_START
	{
		if ( IsDeviceOpened() ){
			CloseDevice();
		}

		if (m_pDevInterface){
			m_pDevInterface->DestroyInstance();
			m_pDevInterface = nullptr;
		}
	}
	SYNCHRONIZED_END
}


//
//
// Delegateの呼び出し
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! Delegateをセットします
//!
//===============================================================
void CCommandBase::SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner )
{
	{
		CBlockCriticalSection cBlockCriticalSection(m_cDelegateCriticalSection);

		m_pDelegate = pDelegate;
		m_pScanner = pScanner;
	}
}

//===============================================================
//!
//! Delegateを取得する
//!
//===============================================================
IESScannerDelegate* CCommandBase::GetDelegate()
{
	{
		CBlockCriticalSection cBlockCriticalSection(m_cDelegateCriticalSection);
		return m_pDelegate;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerWillBeginContinuousScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerWillBeginContinuousScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidEndContinuousScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidEndContinuousScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerWillScanToScannedImage( IESScannedImage* pESImage )
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerWillScanToScannedImage( m_pScanner, pESImage );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidScanToScannedImage( IESScannedImage* pESImage )
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidScanToScannedImage( m_pScanner, pESImage );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerWillCancelScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerWillCancelScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidCancelScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidCancelScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidCompleteScanningWithError( ESErrorCode err)
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("ScannerDidCompleteScanningWithError( %d )"), err );

	{
		// 通信エラーならば、DeviceStream を Close する
		if ( (err == kESErrorDataSendFailure) || (err == kESErrorDataReceiveFailure) ){
			DeviceDisconnected();
			CloseDevice();
		}

		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidCompleteScanningWithError( m_pScanner, err );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidInterruptScanningWithError( ESErrorCode err )
{
	ES_LOG_TRACE_FUNC();

	{
		// 通信エラーならば、DeviceStream を Close する
		if ( (err == kESErrorDataSendFailure) || (err == kESErrorDataReceiveFailure) ){
			DeviceDisconnected();
			CloseDevice();
		}

		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidInterruptScanningWithError( m_pScanner, err );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidEncounterDeviceCommunicationError( ESErrorCode err )
{
	ES_LOG_TRACE_FUNC();

	{
		// DeviceStream を Close する
		{
			DeviceDisconnected();
			CloseDevice();
		}

		if(!IsScanning()){
			IESScannerDelegate* pDelegate = GetDelegate();
			if ( pDelegate == NULL ){
				ES_LOG_DELEGATE_NOT_REGISTERD();
				return kESErrorFatalError;
			}
			pDelegate->ScannerDidEncounterDeviceCommunicationError( m_pScanner, err );
		}
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerWillWarmUp()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerWillWarmUp( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidWarmUp()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidWarmUp( m_pScanner );
		return kESErrorNoError;
	}
}


ESErrorCode CCommandBase::CallDelegateNetworkScannerDidRequestStartScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->NetworkScannerDidRequestStartScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateNetworkScannerDidRequestStopScanning()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->NetworkScannerDidRequestStopScanning( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidDisconnect()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidDisconnect( m_pScanner );

		ES_TRACE_LOG(ES_STRING("Leave CallDelegateScannerDidDisconnect"));
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateNetworkScannerDidReceiveServerError()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->NetworkScannerDidReceiveServerError( m_pScanner );
		return kESErrorNoError;
	}
}

BOOL CCommandBase::CallDelegateNetworkScannerShouldPreventTimeout()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return FALSE;
		}
		return pDelegate->NetworkScannerShouldPreventTimeout( m_pScanner );
	}
}

ESErrorCode CCommandBase::CallDelegateNetworkScannerScannerDidTimeout()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->NetworkScannerDidTimeout( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerIsReservedByHost( const ES_CHAR* pszAddress )
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerIsReservedByHost( m_pScanner, pszAddress );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidPressButton( UInt8 un8ButtonNumber )
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidPressButton( un8ButtonNumber );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidRequestStop()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidRequestStop( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidRequestPushScanConnection()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidRequestPushScanConnection( m_pScanner );
		return kESErrorNoError;
	}
}

ESErrorCode CCommandBase::CallDelegateScannerDidNotifyStatusChange()
{
	ES_LOG_TRACE_FUNC();

	{
		IESScannerDelegate* pDelegate = GetDelegate();
		if ( pDelegate == NULL ){
			ES_LOG_DELEGATE_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		pDelegate->ScannerDidNotifyStatusChange( m_pScanner );
		return kESErrorNoError;
	}
}

void CALLBACK CCommandBase::DidPressButton( UInt8 un8ButtonNumber )
{
	CallDelegateScannerDidPressButton( un8ButtonNumber );
}
void CALLBACK CCommandBase::DidRequestStartScanning()
{
	CallDelegateNetworkScannerDidRequestStartScanning();
}
void CALLBACK CCommandBase::DidRequestStopScanning()
{
	CallDelegateNetworkScannerDidRequestStopScanning();
}
void CALLBACK CCommandBase::DidRequestStartOrStop()
{
	if ( !IsScanning() ){
		CallDelegateNetworkScannerDidRequestStartScanning();
	} else {
		CallDelegateNetworkScannerDidRequestStopScanning();
	}
}
void CALLBACK CCommandBase::DidReceiveServerError()
{
	CallDelegateNetworkScannerDidReceiveServerError();
}
void CALLBACK CCommandBase::DidRequestStop()
{
	CallDelegateScannerDidRequestStop();
}
void CALLBACK CCommandBase::DidDisconnect()
{
	DeviceDisconnected(); // ボタン監視終了処理などを行う
	CallDelegateScannerDidDisconnect();
}

void CALLBACK CCommandBase::DidRequestPushScanConnection()
{
	CallDelegateScannerDidRequestPushScanConnection();
}

BOOL CALLBACK CCommandBase::ShouldPreventTimeout()
{
	return CallDelegateNetworkScannerShouldPreventTimeout();
}

void CALLBACK CCommandBase::DidTimeout()
{
	DeviceDisconnected(); // ボタン監視終了処理などを行う
	CallDelegateNetworkScannerScannerDidTimeout();
}

void CALLBACK CCommandBase::IsReservedByHost(IInterface* pDeviceInterface, const ES_CHAR* pszAddress )
{
	CallDelegateScannerIsReservedByHost( pszAddress );
}

void CALLBACK CCommandBase::DeviceCommunicationError( ESErrorCode err )
{
	CallDelegateScannerDidEncounterDeviceCommunicationError( err );
}

BOOL CALLBACK CCommandBase::ShouldStopScanning( IInterface* pDeviceInterface )
{
	return IsCancelled();
}
void CALLBACK CCommandBase::DidRequestGetImageData()
{
	ES_LOG_TRACE_FUNC();
}
void CALLBACK CCommandBase::DidNotifyStatusChange()
{
	CallDelegateScannerDidNotifyStatusChange();
}

//
//
// Interface操作
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! Interfaceをセットします
//!
//===============================================================
ESErrorCode CCommandBase::SetDeviceInterface(IInterface* pDeviceInterface )
{
	SYNCHRONIZED_START
	{
		if (m_pDevInterface && m_pDevInterface != pDeviceInterface ){
			m_pDevInterface->SetDelegate(nullptr);
			m_pDevInterface->DestroyInstance();
			m_pDevInterface = nullptr;
		}
		if (pDeviceInterface){
			pDeviceInterface->SetDelegate( this );
		}
		m_pDevInterface = pDeviceInterface;
	}
	SYNCHRONIZED_END

	return kESErrorNoError;
}

//===============================================================
//!
//! Interface を Open する
//!
//===============================================================
ESErrorCode CCommandBase::OpenDevice()
{
	SYNCHRONIZED_START
	{
		if (!m_pDevInterface){
			ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		ESErrorCode err = m_pDevInterface->Open();
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_OPEN_DEVICE();
		}
		return err;
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! Interface を Close する
//!
//===============================================================
ESErrorCode CCommandBase::CloseDevice()
{
	SYNCHRONIZED_START
	{
		if (!m_pDevInterface){
			ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
			return kESErrorFatalError;
		}
		m_pDevInterface->Close();
		return kESErrorNoError;
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! Interface を 取得する
//!
//===============================================================
IInterface* CCommandBase::GetDeviceInterface()
{
	SYNCHRONIZED_START
	{
		return m_pDevInterface;
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! Interface が オープン中か？
//!
//===============================================================
bool CCommandBase::IsDeviceOpened() const
{
	SYNCHRONIZED_START
	{
		if (!m_pDevInterface){
			ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
			return false;
		}
		return m_pDevInterface->IsOpened();
	}
	SYNCHRONIZED_END
}

//===============================================================
//!
//! デバイス接続切断時の処理
//!
//===============================================================
void CCommandBase::DeviceDisconnected()
{
}

//===============================================================
//!
//! スキャン中フラグのセット
//!
//===============================================================
void CCommandBase::SetScanning( bool bScanning )
{
	m_bScanning = bScanning;
}

//===============================================================
//!
//! スキャン中か？
//!
//===============================================================
bool CCommandBase::IsScanning() const
{
	return m_bScanning;
}

//===============================================================
//!
//! キャンセル中フラグのセット
//!
//===============================================================
void CCommandBase::SetCancelled( bool bCanncelled )
{
	m_bCancelled = bCanncelled;
}

//===============================================================
//!
//! キャンセル処理中か？
//!
//===============================================================
bool CCommandBase::IsCancelled()
{
	return m_bCancelled;
}

//===============================================================
//!
//! 中断中かのフラグセット
//!
//===============================================================
void CCommandBase::SetInterrupted( bool bInterrupted )
{
	m_bIsInterrupted = bInterrupted;
}

//===============================================================
//!
//! 中断中か？
//!
//===============================================================
bool CCommandBase::IsInterrupted() const
{
	return m_bIsInterrupted;
}

//
//
// データー送受信
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0 // DeviceStreamで振り分ける
//===============================================================
//!
//! バイト列を物理デバイスに書き込む。
//!
//! @param pBuf           書き込むデータへのポインタ。
//! @param un32BufferLen  書き込むデータのサイズ。
//! 
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::Write( __in PESByte pBuf, __in UInt32 un32BufferLen )
{
	if ( pBuf == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorFatalError;
	}
	if ( m_pDevStream == NULL ){
		ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
		return kESErrorFatalError;
	}

	ESErrorCode err = m_pDevStream->Write( pBuf, un32BufferLen );
	if ( IS_ERR_CODE( err ) ){
		err = kESErrorDataSendFailure;
		ES_LOG_FAILED_WRITE_DATA();
		CallDelegateDidEncounterDeviceCommunicationError( err );
	}

	return err;
}
#endif

//===============================================================
//!
//! バイト列を物理デバイスに書き込み、その際、次に読み出すバイト数を宣言する。
//!
//! ISプロトコルでは、送信パケットに次回受信する予定のサイズを合わせて送ることで、ネットワークトラフィックを削減できる。
//! 
//! @param pBuf                    書き込むデータへのポインタ。
//! @param un32BufferLen           書き込むデータのサイズ。
//! @param un32ComittedReadLength  次回読み込むデータのサイズ。
//!
//! @return ラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::Write( __in PESByte pBuf, __in UInt32 un32BufferLen, __in UInt32 un32ComittedReadLength )
{
	if ( pBuf == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorFatalError;
	}
	if (!m_pDevInterface){
		ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
		return kESErrorFatalError;
	}

	ESErrorCode err = m_pDevInterface->Write( pBuf, un32BufferLen, un32ComittedReadLength );
	if ( IS_ERR_CODE( err ) ){
		err = kESErrorDataSendFailure;
		ES_LOG_FAILED_WRITE_DATA();

		if ( !IsScanning() ){
			CallDelegateScannerDidEncounterDeviceCommunicationError( err );
		}
	}

	return err;
}

//===============================================================
//!
//! バイト列を物理デバイスから読み込む。
//!
//! @param pOutBuf        読み込んだデータを入れるバッファへのポインタ。
//! @param un32BufferLen  読み込むデータのサイズ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::Read( __out PESByte pOutBuf, __in UInt32 un32BufferLen )
{
	if ( pOutBuf == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorFatalError;
	}
	if (!m_pDevInterface){
		ES_LOG_DEVICE_STREAM_NOT_REGISTERD();
		return kESErrorFatalError;
	}

	memset( pOutBuf, 0, un32BufferLen );
	ESErrorCode err = m_pDevInterface->Read( pOutBuf, un32BufferLen );
	if ( IS_ERR_CODE( err ) ){
		err = kESErrorDataReceiveFailure;
		ES_LOG_FAILED_READ_DATA();

		if ( !IsScanning() ){
			CallDelegateScannerDidEncounterDeviceCommunicationError( err );
		}
	}

	return err;
}

//
//
// Ack送受信
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//===============================================================
//!
//! スキャナにACK応答を送信する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::SendAck()
{
	ES_LOG_TRACE_FUNC();

	UInt8 rep = ACK;
	return Write( &rep, sizeof(rep), 0 );
}

//===============================================================
//!
//! スキャナからのACK応答を受信する。
//!
//! @param outAck  ACK応答を入れるバッファ（NACKである場合もあるため）。
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::ReceiveAck( __out ESByte& outAck )
{
	outAck = ACK;
	ESErrorCode err = Read( &outAck, sizeof(ESByte) );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_READ_ACK();
		return err;
	}
	ES_INFO_LOG( ES_STRING("ack : %02xh"), outAck );
	return err;
}

//===============================================================
//!
//! SHA1 ハッシュデーター化されたパスワードデーターを作成します。
//!
//! @param strAuthUserNameA  ユーザー名
//!        strPasswordDataA  パスワード(平文)
//!        bdPasswordData    SHA1ハッシュデーター化されたパスワードデータ
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CCommandBase::CreateSha1HashedPasswordData(  __in const ESStringA strAuthUserNameA, __in const ESStringA strPasswordDataA, __out ESByteData& bdPasswordData )
{
	//enum {	
	//	MAX_USERNAME_LENGTH = 20,
	//	HASH_DATA_SIZE   = 20
	//};
	if ( strAuthUserNameA.empty() || strAuthUserNameA.size() > MAX_USERNAME_LENGTH || strPasswordDataA.size() > HASH_DATA_SIZE ){
		ES_LOG_INVALID_SETTING_PARAM();
		return kESErrorInvalidParameter;
	}

	char szUser[ MAX_USERNAME_LENGTH + 1 ] = {0}; //MAX_USERNAME_LENGTH: 20 + Null:1
	char szHash[ HASH_DATA_SIZE + 1 ] = {0}; // PASSWORDDATA_SIZE: 20 + Null:1

	::memcpy_s( szUser, sizeof(szUser),   strAuthUserNameA.c_str(), strAuthUserNameA.size() );
	::memcpy_s( szHash, sizeof(szHash), strPasswordDataA.c_str(), strPasswordDataA.size() );

	// ユーザー名とパスワードからハッシュ値を算出する
	SHA1Context sha = {0};
	BYTE byHash[ SHA1HashSize ] = {0};

	bdPasswordData.clear();
	bdPasswordData.resize( (MAX_USERNAME_LENGTH + SHA1HashSize), 0 );

	if ( SHA1Reset( &sha ) != shaSuccess ){
		ES_LOG_FAILED_MSG( ES_STRING("password encode"), ES_STRING("reset") );
		return kESErrorFatalError;
	}

	if ( SHA1Input( &sha, (const uint8_t *)szUser, (unsigned int)strlen(szUser)) != shaSuccess ) {
		ES_LOG_FAILED_MSG( ES_STRING("password encode"), ES_STRING("input user name") );
		return kESErrorFatalError;
	}

	if ( SHA1Input( &sha, (const uint8_t *)szHash, (unsigned int)strlen(szHash)) != shaSuccess ) {
		ES_LOG_FAILED_MSG( ES_STRING("password encode"), ES_STRING("input pass data") );
		return kESErrorFatalError;
	}

	if ( SHA1Result( &sha, byHash ) != shaSuccess ) {
		ES_LOG_FAILED_MSG( ES_STRING("password encode"), ES_STRING("result") );
		return kESErrorFatalError;
	}

	// ユーザー名とハッシュ値を結合し、認証データを作成する
	::memcpy( &bdPasswordData[0], szUser, MAX_USERNAME_LENGTH );
	::memcpy( &bdPasswordData[MAX_USERNAME_LENGTH], byHash, SHA1HashSize );

	return kESErrorNoError;
}

ESErrorCode CCommandBase::CreateSha1HashedData(__in const ESStringA strDataA, __out ESByteData& bdSha1HashData)
{
	if (strDataA.size() > HASH_DATA_SIZE){
		ES_LOG_INVALID_SETTING_PARAM();
		return kESErrorInvalidParameter;
	}

	char szHash[HASH_DATA_SIZE + 1] = { 0 };

	::memcpy_s(szHash, sizeof(szHash), strDataA.c_str(), strDataA.size());

	SHA1Context sha = { 0 };
	BYTE byHash[SHA1HashSize] = { 0 };

	bdSha1HashData.clear();
	bdSha1HashData.resize(SHA1HashSize, 0);

	if (SHA1Reset(&sha) != shaSuccess){
		ES_LOG_FAILED_MSG(L"password encode", L"reset");
		return kESErrorFatalError;
	}

	if (SHA1Input(&sha, (const uint8_t *)szHash, strlen(szHash)) != shaSuccess) {
		ES_LOG_FAILED_MSG(L"password encode", L"input pass data");
		return kESErrorFatalError;
	}

	if (SHA1Result(&sha, byHash) != shaSuccess) {
		ES_LOG_FAILED_MSG(L"password encode", L"result");
		return kESErrorFatalError;
	}

	::memcpy(&bdSha1HashData[0], byHash, SHA1HashSize);

	return kESErrorNoError;
}

//
//
// WorkFolderの設定
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! WorkFolderを取得する
//!
//===============================================================
ESString CCommandBase::GetWorkFolder()
{
	return m_strWorkFolder;
}

//===============================================================
//!
//! WorkFolderを設定する
//!
//===============================================================
ESErrorCode CCommandBase::SetWorkFolder( ESString strWorkFolder )
{
	if ( ES_CMN_FUNCS::PATH::ES_IsExistFolder( strWorkFolder ) ){
		m_strWorkFolder = strWorkFolder;
		return kESErrorNoError;
	} else {
		return kESErrorInvalidParameter;
	}
}
