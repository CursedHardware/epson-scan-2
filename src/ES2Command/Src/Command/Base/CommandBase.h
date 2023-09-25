////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     CommandBase.h
//!
//! @brif     コマンド送受信ベース クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommonUtility/utils/CritSec.h"
#include "Interface/IInterface.h"


class IESScanner;
class IESScannedImage;
class IESScannerDelegate;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! CCommandBase
//!
//! @bref      コマンド送受信ベース クラス
//!
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CCommandBase : public IInterfaceDelegate
{
public:
	CCommandBase();
	virtual ~CCommandBase();


	//
	// Delegateの操作
	//-----------------------------------------------
	// スキャン時のプロセス処理を行うDelegateを設定する
	void SetDelegate( IESScannerDelegate* pDelegate, IESScanner* pScanner );
	// スキャン時のプロセス処理を行うDelegateを取得する
	IESScannerDelegate* GetDelegate();

	// Delegateの呼び出し
	virtual ESErrorCode CallDelegateScannerWillBeginContinuousScanning();
	virtual ESErrorCode CallDelegateScannerDidEndContinuousScanning();
	virtual ESErrorCode CallDelegateScannerWillScanToScannedImage( IESScannedImage* pESImage );
	virtual ESErrorCode CallDelegateScannerDidScanToScannedImage( IESScannedImage* pESImage );
	virtual ESErrorCode CallDelegateScannerWillCancelScanning();
	virtual ESErrorCode CallDelegateScannerDidCancelScanning();
	virtual ESErrorCode CallDelegateScannerDidCompleteScanningWithError( ESErrorCode err);
	virtual ESErrorCode CallDelegateScannerDidInterruptScanningWithError( ESErrorCode err);
	virtual ESErrorCode CallDelegateScannerDidEncounterDeviceCommunicationError( ESErrorCode err );
	virtual ESErrorCode CallDelegateScannerWillWarmUp();
	virtual ESErrorCode CallDelegateScannerDidWarmUp();
	virtual ESErrorCode CallDelegateNetworkScannerDidRequestStartScanning();
	virtual ESErrorCode CallDelegateNetworkScannerDidRequestStopScanning();
	virtual ESErrorCode CallDelegateScannerDidDisconnect();
	virtual ESErrorCode CallDelegateNetworkScannerDidReceiveServerError();
	virtual BOOL        CallDelegateNetworkScannerShouldPreventTimeout();
	virtual ESErrorCode CallDelegateNetworkScannerScannerDidTimeout();
	virtual ESErrorCode CallDelegateScannerIsReservedByHost( const ES_CHAR* pszAddress );
	virtual ESErrorCode CallDelegateScannerDidPressButton( UInt8 un8ButtonNumber );
	virtual ESErrorCode CallDelegateScannerDidRequestStop();
	virtual ESErrorCode CallDelegateScannerDidRequestPushScanConnection();
	virtual ESErrorCode CallDelegateScannerDidNotifyStatusChange();

	// IESDeviceInterfaceDelegate
	virtual void CALLBACK DidPressButton( UInt8 un8ButtonNumber ) override;
	virtual void CALLBACK DidRequestStartScanning() override;
	virtual void CALLBACK DidRequestStopScanning() override;
	virtual void CALLBACK DidRequestStartOrStop() override;
	virtual void CALLBACK DidRequestStop() override;
	virtual void CALLBACK DidReceiveServerError() override;
	virtual void CALLBACK DidDisconnect() override;
	virtual void CALLBACK DidRequestPushScanConnection() override;
	virtual BOOL CALLBACK ShouldPreventTimeout() override;
	virtual void CALLBACK DidTimeout() override;
	virtual void CALLBACK IsReservedByHost(IInterface* pDeviceInterface, const ES_CHAR* pszAddress ) override;
	virtual BOOL CALLBACK ShouldStopScanning( IInterface* pDeviceInterface ) override;
	virtual void CALLBACK DidRequestGetImageData() override;
	virtual void CALLBACK DidNotifyStatusChange() override;
	virtual void CALLBACK DeviceCommunicationError( ESErrorCode err ) override; // Win only

	//
	// DeviceInterface操作
	//------------------------------------------------
	// DeviceInterfaceをセットします
	ESErrorCode SetDeviceInterface(IInterface* pDeviceInterface );
	// DeviceInterface を Open する
	virtual ESErrorCode OpenDevice();
	// DeviceInterface を Close する
	virtual ESErrorCode CloseDevice();
	// DeviceStream を 取得する
	IInterface* GetDeviceInterface();

	//デバイス接続切断時の処理
	virtual void DeviceDisconnected();

	// DeviceInterface が オープン中か？
	bool IsDeviceOpened() const;
	// スキャン中か？
	bool IsScanning() const;
	// キャンセル処理中か？
	bool IsCancelled();
	// 中断中か？
	bool IsInterrupted() const;

protected:
	// スキャン中フラグのセット
	void SetScanning( bool bScanning );
	// キャンセル中フラグのセット
	void SetCancelled( bool bCanncelled );
	// 中断中かのフラグセット
	void SetInterrupted( bool bInterrupted );

protected:

	typedef enum {
		EOT         = 0x04,
		ESC 		= 0x1B,
		FS 			= 0x1C,
		CAN 		= 0x18,
		FORM_FEED	= 0x0C,
		PAPER_FEED	= 0x19,
	} ENUM_CONTROL_CODE;

	typedef enum {
		ACK 		= 0x06,
		NACK		= 0x15,
		BUSY		= 0x07,
	} ENUM_ACK_CODE;

	//
	// データー送受信
	//-----------------------------------------------
#if 0 // DeviceStreamで振り分ける
	// バイト列を物理デバイスに書き込む。
	ESErrorCode Write ( __in PESByte pBuf, __in UInt32 un32BufferLen );
#endif
	// バイト列を物理デバイスに書き込み、その際、次に読み出すバイト数を宣言する。
	ESErrorCode Write ( __in PESByte pBuf, __in UInt32 un32BufferLen, __in UInt32 un32ComittedReadLength );
	// バイト列を物理デバイスから読み込む。
	ESErrorCode Read  ( __out PESByte pBuf, __in UInt32 un32BufferLen );

	//
	// ACK送受信
	//-----------------------------------------------
	// スキャナにACK応答を送信する。
	ESErrorCode SendAck();
	// スキャナからのACK応答を受信する。
	ESErrorCode ReceiveAck( __out ESByte& outAck );

	//
	// WorkFolderの設定
	//-----------------------------------------------
	// WorkFolderを取得する
	ESString GetWorkFolder();
	// WorkFolderを設定する
	ESErrorCode SetWorkFolder( ESString strWorkFolder );

	//
	// misc
	//-----------------------------------------------
	// CritSecの取得
	CritSec& GetCriticalSection() const { return m_cCriticalSection; }

	enum {	
		MAX_USERNAME_LENGTH = 20,
		HASH_DATA_SIZE   = 20 // SHA1HashSize
	};
	// SHA1 ハッシュデーター化されたパスワードデーターを作成します。
	ESErrorCode CreateSha1HashedPasswordData( __in const ESStringA strAuthUserNameA, __in const ESStringA strPasswordDataA, __out ESByteData& bdPasswordData );
	ESErrorCode CreateSha1HashedData(__in const ESStringA strDataA, __out ESByteData& bdSha1HashData);

protected:
	mutable CritSec			m_cCriticalSection;
	mutable CritSec			m_cDelegateCriticalSection;

	IInterface				*m_pDevInterface;
	IESScannerDelegate*		m_pDelegate;
	IESScanner*				m_pScanner;

	bool					m_bCancelled;
	bool					m_bScanning;
	bool					m_bIsInterrupted;

	ESString				m_strWorkFolder;
};

//
// CRITICAL_SECTIONによる同期処理
//--------------------------------------
// 開始
#define SYNCHRONIZED_START				{ CBlockCriticalSection cBlockCriticalSection( GetCriticalSection() );
// 終了
#define SYNCHRONIZED_END		}

