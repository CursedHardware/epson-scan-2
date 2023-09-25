////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESCICommand.cpp
//!
//! @brif     ESCIコマンド送受信 クラス
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "CommonUtility/utils/stringutils.h"

#include "ESCICommand.h"
#include "../ModelInfo.h"
#include "Utils/Endian.h"
#include "Utils/ESUtils.h"

enum {
	kStatSupportExtCommand  = (1 << 1),
	kStatColorAttribute1    = (1 << 2),
	kStatColorAttribute2    = (1 << 3),
	kStatOption             = (1 << 4),
	kStatAreaEnd            = (1 << 5),
	kStatNotReady           = (1 << 6),
	kStatFatalError         = (1 << 7),
};
typedef UInt8 ENUM_ESCI_STATUS_CODE_MASK;   // Status in info block

enum {
	kBlockErrorCancel     = (1 << 4),
    kBlockErrorPaperEnd   = (1 << 5),
	kBlockErrorNotReady   = (1 << 6),
	kBlockESErrorFatalError = (1 << 7),
};
typedef UInt8 ENUM_ESCI_BLOCK_ERROR_CODE_MASK;   // Status in image data block

enum {
	kCaptureReplySuccess        = 0x80,
	kCaptureReplyNack           = 0x15,
	kCaptureReplyNotReady       = 0x40,
	kCaptureReplyNoPermission   = 0xC0,
};
typedef UInt8 ENUM_ESCI_CAPTURE_REPLY;


#pragma pack(push, 1)
// 情報ブロックの受け取りを行うコマンドを実行時に返されるデーターのヘッダー情報
typedef struct {
	UInt8	un8Header;			// STX : 0x02
	UInt8	un8Status;			// ステータス情報 ( ENUM_ESCI_STATUS_CODE_MASKでマスクされている )
	UInt16	un16ByteCounter;	// 後に続くデーターブロックのサイズ
} ST_ESCI_INFO_BLOCK;
#pragma pack(pop)


//===============================================================
//!
//! コンストラクタ
//!
//===============================================================
CESCICommand::CESCICommand()
{
}

//===============================================================
//!
//! デストラクタ
//!
//===============================================================
CESCICommand::~CESCICommand()
{
}

//===============================================================
//!
//! 初期化処理
//!
//===============================================================
ESErrorCode CESCICommand::Initialize()
{
	return kESErrorNoError;
}

// CANコマンド応答のチェックを無効化する
// KimchiにおいてCANの応答が不正なものになる。そのため、例外処理により応答を無視するために参照
bool CESCICommand::IsShouldIgnoreCancelResponse()
{
	using namespace epsonscan2::es2command;
	return ModelInfo::Instance().IsShouldIgnoreCancelResponse(GetProductName());
}

//
//
// クラスステータス
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! ISプロトコルにおける、拡張データ転送をサポートしているかどうかを判断する。
//!
//===============================================================
bool CESCICommand::IsUsesExtTransfer()
{
	if (m_pDevInterface){
		return m_pDevInterface->IsSupportsExtendedTransfer();
	}
	return false;
}

//===============================================================
//!
//! 画像転送時のドライバ側からのACK応答をすべきかどうか判断する。
//!
//===============================================================
bool CESCICommand::IsAvoidsAckWhileImageTransfer()
{
	if ( IsUsesExtTransfer() ){
		return true;
	}
	if (m_pDevInterface){
		return m_pDevInterface->IsAvoidsAckWhileImageTransfer();
	}
	return false;
}

//===============================================================
//!
//! スキャナがウォームアップ状態を解除するまで待つ。
//!
//! @param nTimeout_s     タイムアウト値。これを超えてもウォームアップが解除されない場合はエラーを返す。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::WaitWhileWarmingUp( int nTimeout_s )
{
	const int WAIT_INTERVAL = 1000; // 1 second

	if ( !IsWarmingUp() ){
		return kESErrorNoError;
	}
	CallDelegateScannerWillWarmUp();

	ESErrorCode err = kESErrorNoError;
	DWORD dwStart_ms = GetTickCount();
	DWORD dwTimeout_ms = nTimeout_s * 1000;
	while ( 1 ){
		if ( IsPassedInterval( dwStart_ms, dwTimeout_ms, NULL ) ){
			return kESErrorFatalError;
		}
		if ( IsWarmingUp() ){
#ifdef WIN32
			Sleep( WAIT_INTERVAL );
#else
			Sleep( WAIT_INTERVAL );
#endif
		} else {
			CallDelegateScannerDidWarmUp();
			break;
		}
	}
	return err;
}

//
//
// コマンド送信
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//===============================================================
//!
//! スキャナにイメージ転送時のACK応答を送信する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendAckForImageData()
{
	if ( IsAvoidsAckWhileImageTransfer() ) {
		return kESErrorNoError;
	}
	return SendAck();
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//!
//! @param eCommand                スキャナに送信するコマンド（ESC Iの'I'など）。
//! @param eControlCode            スキャナに送信するコマンドの制御コード（ESC IのESCなど）。
//! @param un32ComittedReadLength  次回読み込むデータのサイズ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand1( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in UInt32 un32ComittedReadLength )
{
	// Scanner ------------------------------------------
	//               ↑
	//              ①
	//            ↑
	// Host PC ------------------------------------------

	ESErrorCode err = kESErrorNoError;

	if (eCommand == CMD_NONE) {

		UInt8 cmd[] = { (UInt8)eControlCode};
		//MyLog(@"%s: %@ '%c'", __FUNCTION__, [NSData dataWithBytes:cmd length:sizeof(cmd)], command);
		err = Write( &cmd[0], sizeof(cmd), un32ComittedReadLength);

	} else {

		UInt8 cmd[] = { (UInt8)eControlCode, eCommand};
		//MyLog(@"%s: %@", __FUNCTION__, [NSData dataWithBytes:cmd length:sizeof(cmd)]);
		err = Write( &cmd[0], sizeof(cmd), un32ComittedReadLength );

	}
	return err;
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//! [コマンド送信 -> データ受信]
//! のパターンとなるコマンドで使用する。（例：ESC 0xE2）
//!
//! @param eCommand           コマンド。
//! @param eControlCode       制御コード。
//! @param pOutBuffer         結果を受け取るバッファへのポインタ。
//! @param un32BufferLength   結果のバイト列の期待される長さ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand2( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out PESByte pOutBuffer, __in UInt32 un32BufferLength )
{
	//
	// Scanner ------------------------------------------
	//                 ↑   ↓
	//                ①     ②
	//              ↑         ↓ 
	// Host PC ------------------------------------------
	//

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		err = SendCommand1( eCommand, eControlCode, un32BufferLength );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
		err = Read( pOutBuffer, un32BufferLength );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
	}
	SYNCHRONIZED_END
	
	return err;
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//! [コマンド送信 -> ACK受信]
//! のパターンとなるコマンドで使用する。（例：ESC @)
//!
//! @param eCommand     コマンド。
//! @param eControlCode 制御コード。
//! @param outAck       ACK応答を受け取るバッファ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand2A( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out ESByte& outAck )
{
	// Scanner ------------------------------------------
	//               ↑  ↓
	//              ①     ②
	//            ↑         ↓ 
	// Host PC ------------------------------------------
	//
	//   ②で Ack 取得

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		outAck = ACK;
		err = SendCommand1( eCommand, eControlCode, sizeof(ESByte) );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
		err = ReceiveAck( outAck );
	}
	SYNCHRONIZED_END

	return err;
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//! [コマンド送信 -> 情報ブロック受信 -> データ受信]
//!  のパターンとなるコマンドで使用する。（例：ESC !）
//!
//! @param eCommand         コマンド。
//! @param eControlCode     制御コード。
//! @param pUn8OutStatus    受信した情報ブロックを受け取る構造体へのポインタ。不要な場合はNULLを指定する。
//! @param cOutData         受信したデータを受け取るバッファクラス。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand3( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __out_opt UInt8* pUn8OutStatus, __out ES_CMN_FUNCS::BUFFER::IESBuffer& cOutData )
{
	// Scanner ------------------------------------------
	//               ↑  ↓      ↓
	//              ①     ②      ③
	//            ↑         ↓      ↓ 
	// Host PC ------------------------------------------

	const ESByte STX = 0x02;

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		if ( pUn8OutStatus != NULL ){
			*pUn8OutStatus = 0;
		}
		cOutData.FreeBuffer();

		err = SendCommand1( eCommand,  eControlCode, sizeof(ST_ESCI_INFO_BLOCK) );
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_SEND_COMMAND();
			goto BAIL;
		}

		ST_ESCI_INFO_BLOCK stOutInfoBlock = {0};
		err = Read( (UInt8*)&stOutInfoBlock, sizeof(ST_ESCI_INFO_BLOCK) );
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_READ_DATA();
			goto BAIL;
		}
		if (stOutInfoBlock.un8Header != STX){ // ヘッダーには STX が入っているはず
			ES_LOG_INVALID_RESPONSE();
			err = kESErrorInvalidResponse;
			goto BAIL;
		}

		if ( pUn8OutStatus != NULL ){
			*pUn8OutStatus = stOutInfoBlock.un8Status;
		}

		UInt16 un16DataBytes = (UInt16)CFSwapInt16LittleToHost( stOutInfoBlock.un16ByteCounter ); // 後に続くデータブロックのサイズ (エンディアン変更)
		if ( !cOutData.AllocBuffer( un16DataBytes ) ){
			ES_LOG_MEMORY_ALLOCATE_ERROR();
			err = kESErrorMemoryError;
			goto BAIL;
		}

		err = Read( cOutData.GetBufferPtr(), un16DataBytes );
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_READ_DATA();
			goto BAIL;
		}
	}
	SYNCHRONIZED_END

BAIL:
	return err;
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//! [コマンド送信 -> ACK受信 -> パラメータ送信 -> 結果受信]
//! のパターンとなるコマンドで使用する。
//!
//! @param eCommand        コマンド。
//! @param eControlCode    制御コード。
//! @param bdParam         コマンドパラメータを記述したバッファクラス。
//! @param pOutBuffer      結果を受け取るバッファへのポインタ。
//! @param un32BufferLen   結果のバイト列の期待される長さ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand4( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in ESByteData& bdParam, __out PESByte pOutBuffer, __in UInt32 un32BufferLen )
{
	// Scanner ------------------------------------------
	//               ↑  ↓          ↑  ↓
	//              ①     ②       ③     ④
	//            ↑         ↓   ↑         ↓ 
	// Host PC ------------------------------------------

	SYNCHRONIZED_START
	{
		ESErrorCode err = SendCommand1( eCommand, eControlCode, sizeof(ESByte) );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}

		UInt8 ack = ACK;
		err = ReceiveAck( ack );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
		if ( ack != ACK ){
			return kESErrorInvalidResponse;
		}

		err = Write( &bdParam[0], (UInt32)bdParam.size(), un32BufferLen);
		if ( IS_ERR_CODE( err ) ){
			return err;
		}

		err = Read( pOutBuffer, un32BufferLen );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
	}
	SYNCHRONIZED_END

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナにコマンドを送信する。
//! [コマンド送信 -> ACK受信 -> パラメータ送信 -> ACK受信]
//! のパターンとなるコマンドで使用する。（例：FS W）
//!
//! @param eCommand     コマンド。
//! @param eControlCode 制御コード。
//! @param bdParam       コマンドパラメータ。
//! @param outAck       ACK応答を受け取るバッファ。
//!
//! @return            エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::SendCommand4A( __in ENUM_ESCI_COMMAND eCommand, __in ENUM_CONTROL_CODE eControlCode, __in ESByteData& bdParam, __out ESByte& outAck )
{
	// Scanner ------------------------------------------
	//               ↑  ↓          ↑  ↓
	//              ①     ②       ③    ④
	//            ↑         ↓   ↑        ↓ 
	// Host PC ------------------------------------------
	//
	//   ④で Ack 取得

	ESErrorCode err = kESErrorNoError;

	SYNCHRONIZED_START
	{
		outAck = ACK;
		err = SendCommand4( eCommand, eControlCode, bdParam, &outAck, sizeof(ESByte));
		if ( IS_ERR_SUCCESS(err) ){
			ES_INFO_LOG( ES_STRING("ack : %02xh"), outAck );
		}
		//MyLog(@"%s: ack = %@", __FUNCTION__, [NSData dataWithBytes:outAck length:sizeof(UInt8)]);
	}
	SYNCHRONIZED_END

	return err;
}


//
//
// コマンドリクエスト
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================
//!
//! スキャナにIdentity（スキャナのCapability）情報を要求する。
//!
//! @param stOutIdentity      受信したIdentity情報を受け取るバッファ。
//! @param arOutResolutions  スキャナがサポートする解像度を受け取る。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestIdentity( __out ST_ESCI_IDENTITY& stOutIdentity, __out ESIndexSet& arOutResolutions )
{
	//
	// ESC I コマンド送信 [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p23 ]
	//
	// Scanner ------------------------------------------
	//           ↑  ↓      ↓
	//          ①     ②      ③
	//        ↑         ↓      ↓ 
	// Host PC ------------------------------------------
	//
	//  1) ESC I code
	//  2) Information block
	//  3) Identity data
	//

	ES_LOG_TRACE_FUNC();

	arOutResolutions.clear();
	memset( &stOutIdentity, 0, sizeof(stOutIdentity) );

	UInt8 un8Status = 0;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cHeapBuffer;
	ESErrorCode err = SendCommand3( GET_IDENTITY, ESC, &un8Status, cHeapBuffer);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	ES_CMN_FUNCS::BUFFER::CESIBufferStream cStream( &cHeapBuffer );

	UInt32 unRead = cStream.Read( stOutIdentity.un8CommandLevel, sizeof(stOutIdentity.un8CommandLevel) );
	if ( unRead < sizeof(stOutIdentity.un8CommandLevel) ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}

	UInt8 un8Tag = 0;
	while ( 1 ){
		un8Tag = 0;
		unRead = cStream.Read( &un8Tag, sizeof(UInt8) );
		if ( unRead == 0 ){
			ES_LOG_INVALID_RESPONSE();
			return kESErrorInvalidResponse;
		}

		if ( un8Tag == 'R' ){
			UInt16 un16Resolution = 0;
			unRead = cStream.Read( (UInt8*)&un16Resolution, sizeof(UInt16) );
			if ( unRead < sizeof(UInt16) ){
				ES_LOG_INVALID_RESPONSE();
				return kESErrorInvalidResponse;
			}
			un16Resolution = (UInt16)CFSwapInt16LittleToHost( un16Resolution );
			arOutResolutions.insert( un16Resolution );
		} else {
			break;
		}
	}
	
	if ( un8Tag == 'A' ){

		UInt16 un16Width  = 0;
		unRead = cStream.Read( (UInt8*)&un16Width, sizeof(UInt16) );
		if ( unRead < sizeof(UInt16) ){
			ES_LOG_INVALID_RESPONSE();
			return kESErrorInvalidResponse;
		}
		stOutIdentity.un16MaxWidth = (UInt16)CFSwapInt16LittleToHost( un16Width );

		UInt16 un16Height = 0;
		unRead = cStream.Read( (UInt8*)&un16Height, sizeof(UInt16) );
		if ( unRead < sizeof(UInt16) ){
			ES_LOG_INVALID_RESPONSE();
			return kESErrorInvalidResponse;
		}
		stOutIdentity.un16MaxHeight = (UInt16)CFSwapInt16LittleToHost( un16Height );
	}
	return err;
}

//===============================================================
//!
//! スキャナにステータス情報を要求する。
//!
//! @param stOutStatus  受信したステータス情報を受け取るバッファ
//!
//! @return エラーコード
//!
//===============================================================
ESErrorCode CESCICommand::RequestStatus( __out ST_ESCI_SCANNER_STATUS& stOutStatus )
{
	//
	// FS F コマンド送信  [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p80 ]
	//
	//  Scanner ------------------------------------------
	//              ↑  ↓
	//             ①     ②
	//           ↑         ↓
	//  Host PC ------------------------------------------
	//
	//   1) FS F code
	//   2) Scanner status
	//

	ES_LOG_TRACE_FUNC();

	memset( &stOutStatus, 0, sizeof(stOutStatus) );

	ESErrorCode err = SendCommand2( GET_STATUS, FS, (PESByte)&stOutStatus, sizeof(ST_ESCI_SCANNER_STATUS));
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	stOutStatus.un16AdfPaperSize  = (UInt16)CFSwapInt16LittleToHost( stOutStatus.un16AdfPaperSize  );
	stOutStatus.un16MainPaperSize = (UInt16)CFSwapInt16LittleToHost( stOutStatus.un16MainPaperSize );

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに拡張ステータス情報を要求する。
//!
//! @param stOutExtStatus  受信した拡張ステータス情報を受け取るバッファ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestExtendedStatus( __out ST_ESCI_EXTENDED_STATUS& stOutExtStatus )
{
	//
	//     ESC f コマンド送信  [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p26 ]
	//
	//  Scanner ------------------------------------------
	//               ↑  ↓      ↓
	//              ①     ②      ③
	//            ↑         ↓      ↓ 
	//  Host PC ------------------------------------------
	//

	ES_LOG_TRACE_FUNC();
	
	UInt8 un8Status = 0;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cHeapBuffer;
	ESErrorCode err = SendCommand3( GET_EXT_STATUS, ESC, &un8Status, cHeapBuffer);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( un8Status & kStatNotReady ){
		ES_LOG_NOT_READY();
		return kESErrorDeviceInBusy;
	}
	if ( un8Status & kStatFatalError ){
		ES_LOG_INVALID_STATUS();
		return kESErrorDeviceFatalError;
	}

	if ( cHeapBuffer.GetLength() != sizeof(ST_ESCI_EXTENDED_STATUS) ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	
	memcpy_s( &stOutExtStatus, sizeof(ST_ESCI_EXTENDED_STATUS), cHeapBuffer.GetBufferPtr(), cHeapBuffer.GetLength());

	stOutExtStatus.un16AdfX  = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16AdfX );
	stOutExtStatus.un16AdfY  = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16AdfY );
	stOutExtStatus.un16TpuX  = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16TpuX );
	stOutExtStatus.un16TpuY  = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16TpuY );
	stOutExtStatus.un16X     = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16X );
	stOutExtStatus.un16Y     = (UInt16)CFSwapInt16LittleToHost( stOutExtStatus.un16Y );

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナにハードウェア情報を要求する。
//! 
//! @param stOutHWProperty     受信したハードウェア情報を受け取るバッファ。
//! @param arOutXResolutions   スキャナがサポートする主走査解像度を受け取る。
//! @param arOutYResolutions   スキャナがサポートする副走査解像度を受け取る。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestHWProperty( __out ST_ESCI_HW_PROPERTY& stOutHWProperty, __out ESIndexSet& arOutXResolutions, __out ESIndexSet& arOutYResolutions )
{
	//
	// ESC i
	//

	ES_LOG_TRACE_FUNC();

	arOutXResolutions.clear();
	arOutYResolutions.clear();

	UInt8 un8Status = 0;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cHeapBuffer;
	ESErrorCode err = SendCommand3( GET_HW_PROPERTY, ESC, &un8Status, cHeapBuffer);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	ES_CMN_FUNCS::BUFFER::CESIBufferStream cStream( &cHeapBuffer );

	UInt32 unRead = cStream.Read( (UInt8*)&stOutHWProperty, sizeof(stOutHWProperty) );
	if ( unRead < sizeof(stOutHWProperty) ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	stOutHWProperty.un16StdRes = (UInt16)CFSwapInt16LittleToHost( stOutHWProperty.un16StdRes );

	ESIndexSet* pArSetResolution[2] = {&arOutXResolutions, &arOutYResolutions};
	for ( int i = 0; i < 2; i++ ){
		while( 1 ){
			UInt16 un16Resolution = 0;
			unRead = cStream.Read( (UInt8*)&un16Resolution, sizeof(UInt16) );
			if ( unRead < sizeof(UInt16) ){
				ES_LOG_INVALID_RESPONSE();
				return kESErrorInvalidResponse;
			}
			un16Resolution = (UInt16)CFSwapInt16LittleToHost(un16Resolution);
			if ( un16Resolution == 0 ){
				break;
			}
			pArSetResolution[i]->insert(un16Resolution);
		}
	}
	return err;
}

//===============================================================
//!
//! スキャナに拡張Identity情報を要求する。
//!
//! @param stOutExtIdentity  受信した拡張Identity情報を受け取るバッファ
//!
//! @return エラーコード
//!
//===============================================================
ESErrorCode CESCICommand::RequestExtendedIdentity( __out ST_ESCI_EXTENDED_IDENTITY& stOutExtIdentity )
{
	//
	// FS I コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p73 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓
	//               ①     ②
	//             ↑         ↓
	//  Host PC ------------------------------------------
	// 
	//         1) FS I code
	//         2) Extended identity data
	//

	ES_LOG_TRACE_FUNC();

	memset( &stOutExtIdentity, 0, sizeof( stOutExtIdentity ) );

	ESErrorCode err = SendCommand2( GET_IDENTITY, FS, (PESByte)&stOutExtIdentity, sizeof(stOutExtIdentity) );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	stOutExtIdentity.un32BasicResolution		= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32BasicResolution);
	stOutExtIdentity.un32MinResolution			= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32MinResolution);
	stOutExtIdentity.un32MaxResolution			= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32MaxResolution);
	stOutExtIdentity.un32MaxWidthPixels			= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32MaxWidthPixels);
	stOutExtIdentity.un32PixelsInBasicXRes		= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32PixelsInBasicXRes);
	stOutExtIdentity.un32PixelsInBasicYRes		= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32PixelsInBasicYRes);
	stOutExtIdentity.un32AdfPixelsInBasicXRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32AdfPixelsInBasicXRes);
	stOutExtIdentity.un32AdfPixelsInBasicYRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32AdfPixelsInBasicYRes);
	stOutExtIdentity.un32TpuPixelsInBasicXRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32TpuPixelsInBasicXRes);
	stOutExtIdentity.un32TpuPixelsInBasicYRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32TpuPixelsInBasicYRes);
	stOutExtIdentity.un32Tpu2PixelsInBasicXRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32Tpu2PixelsInBasicXRes);
	stOutExtIdentity.un32Tpu2PixelsInBasicYRes	= (UInt32)CFSwapInt32LittleToHost(stOutExtIdentity.un32Tpu2PixelsInBasicYRes);

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに設定されているスキャンパラメータ情報を要求する。
//!
//! @param stOutScanningParameter  受信したスキャンパラメータ情報を受け取るバッファ
//!
//! @return エラーコード
//!
//===============================================================
ESErrorCode CESCICommand::RequestScanningParameter( __out ST_ESCI_SCANNING_PARAMETER& stOutScanningParameter )
{
	//
	// FS S コマンド送信    [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p95 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓
	//               ①     ②
	//             ↑         ↓
	//  Host PC ------------------------------------------
	//
	//      1) FS S code
	//      2) Scanning parameter
	//

	ES_LOG_TRACE_FUNC();

	memset( &stOutScanningParameter, 0, sizeof( stOutScanningParameter ) );

	ESErrorCode err = SendCommand2( GET_SCANNINGPARAMETER, FS, (PESByte)&stOutScanningParameter, sizeof(stOutScanningParameter) );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	stOutScanningParameter.un32XResolution   = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32XResolution);
	stOutScanningParameter.un32YResolution   = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32YResolution);
	stOutScanningParameter.un32XOffset       = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32XOffset);
	stOutScanningParameter.un32YOffset       = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32YOffset);
	stOutScanningParameter.un32Width         = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32Width);
	stOutScanningParameter.un32Height        = (UInt32)CFSwapInt32LittleToHost(stOutScanningParameter.un32Height);

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナのDigitalICE機能を有効化するために送るバイト列を暗号化する関数。
//! ESC #コマンドのパラメータとして使用する。
//!
//! FS Sコマンド（ファームに現在セットされているスキャンパラメータの取得）の返り値の先頭32バイトを暗号化してスキャナに送信すると、
//! ファーム側で復号化してファーム内部のFS Sの現在状態と一致するかどうか判定する。
//! 一致していれば、DigitalICEの使用をライセンスされたソフトウェアからの要求であると判断し、
//! 以降のDigitalICE関連コマンドを受け付けるようになる。関数内部の暗号化キーは固定。
//!
//! @param plain   平文データ。実際にはFS Sコマンドで得られたバイト列の先頭32バイト。
//! @param pOutEncrypted    暗号化データ。
//! @param uLength  平文および暗号化データの長さ。現在の仕様では32バイト固定。
//!
//===============================================================
static void DICEGetEncryptedBytes(const PESByte plain, PESByte pOutEncrypted, UInt32 uLength)
{
	static const UInt8 diceKey1[] = { 0xc6, 0x30, 0xb8, 0x53, 0x9c };
	static const UInt8 diceKey2[] = { 0x81, 0x79, 0xf9, 0x44, 0x97, 0x1a, 0x5d, 0x36 };
	static const UInt8 q = 3;

	UInt32 diceKey1Size = sizeof(diceKey1);
	UInt32 diceKey2Size = sizeof(diceKey2);

	for (UInt32 i = 0; i < uLength; i++) {
		UInt8 x = 0;
		UInt32 j = 0;

		x = plain[i];

		j = i % diceKey1Size;
		x = x ^ diceKey1[j];
		x = (x << (8 - q)) | (x >> q);

		j = i % diceKey2Size;
		x = x ^ diceKey2[j];
		x = (x << q) | (x >> (8 - q));

		pOutEncrypted[i] = x;
	}
}

//===============================================================
//!
//!スキャナにDigitalICE機能の使用許可を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestUseDICE()
{
	//
	// ESC # コマンド送信
	//

	ES_LOG_TRACE_FUNC();

	const int ENCRYPTEDDATA_SIZE = 32;

	ST_ESCI_SCANNING_PARAMETER stScanningParameter = {};
	ESErrorCode err = RequestScanningParameter( stScanningParameter );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	ESByteData bdEncryptedData( ENCRYPTEDDATA_SIZE , 0);
	assert( bdEncryptedData.size() == ENCRYPTEDDATA_SIZE );
	DICEGetEncryptedBytes((const PESByte)&stScanningParameter, &bdEncryptedData[0], (UInt32)bdEncryptedData.size() );

	ESByte ack = ACK;
	err = SendCommand4A( REQ_USE_DICE, ESC, bdEncryptedData, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}

	return err;
}

//===============================================================
//!
//! スキャナにスキャン動作のキャンセルを要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestCancelScanning()
{
	//
	// CAN コマンド送信    [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p36 ]
	//
	//  Scanner ------------------------------------------Abort scanning
	//             ↓    ↓        ↑  ↓       ↑  ↓
	//               ①    ②     ③    ②     ④    ③
	//                 ↓   ↓  ↑        ↓  ↑       ↓ 
	//  Host PC ------------------------------------------
	//
	//     1) Information block
	//     2) Image data
	//     3) ACK (06h)
	//     4) CAN (18h)
	//

	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if ( IsUsesExtTransfer() ){

		if (m_pDevInterface) {
			err = m_pDevInterface->AbortExtendedTransferWithCode( CAN );
			if ( IS_ERR_CODE( err ) ){
				ES_LOG_FAILED_SEND_COMMAND();
				return err;
			}
		} else {
			ES_LOG_INVALID_SETTING_PARAM();
			return kESErrorFatalError;
		}
	} else {
		ESByte ack = ACK;
		err = SendCommand2A( CMD_NONE, CAN, ack);
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_SEND_COMMAND();
			return err;
		}
		if ( ack != ACK ){
			ES_LOG_INVALID_RESPONSE();
			return kESErrorInvalidResponse;
		}
	}
	return err;
}

//===============================================================
//!
//! スキャナに画像転送の終了を通知する。
//! 
//===============================================================
ESErrorCode CESCICommand::RequestEndTransmission()
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;

	if ( IsUsesExtTransfer() ){
		if (m_pDevInterface) {
			err = m_pDevInterface->AbortExtendedTransferWithCode( EOT );
			if ( IS_ERR_CODE( err ) ){
				ES_LOG_FAILED_SEND_COMMAND();
				return err;
			}
		} else {
			ES_LOG_INVALID_SETTING_PARAM();
			return kESErrorFatalError;
		}
	} else {
		ESByte ack = ACK;
		err = SendCommand2A( CMD_NONE, EOT, ack);
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_SEND_COMMAND();
			return err;
		}
		if ( ack != ACK ){
			ES_LOG_INVALID_RESPONSE();
			return kESErrorInvalidResponse;
		}
	}
	return err;
}

//===============================================================
//!
//! スキャナに排紙を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestEjectPaper()
{
	//
	// FF コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p71 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓
	//               ①     ②
	//             ↑         ↓
	//  Host PC ------------------------------------------
	//
	//      1) FF code
	//      2) ACK (06h) / NACK (15h)
	//

	ES_LOG_TRACE_FUNC();

	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( CMD_NONE, FORM_FEED, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return err;
}

//===============================================================
//!
//! スキャナに給紙を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestLoadPaper()
{
	// 
	// PF コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p72 ]
	//
	//   Scanner ------------------------------------------
	//                    ↑  ↓
	//                  ①      ②
	//                ↑          ↓
	//   Host PC ------------------------------------------
	//
	//     1) PF code
	//     2) ACK (06h) / NACK (15h)
	//

	ES_LOG_TRACE_FUNC();

	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( CMD_NONE, PAPER_FEED, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return err;
}

//===============================================================
//!
//! スキャナに占有開始を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestCaptureScanner()
{
	//
	// ( コマンド送信   [ see R41AD7980/7990 ESC/I Command Specifications (R41AD7980_90_CmdSpecD.pdf) p73 ]
	//
	//   Scanner ------------------------------------------
	//               ↑  ↓
	//             ①      ②
	//           ↑          ↓
	//   Host PC ------------------------------------------
	//
	//    1) ESC ( code
	//    2) Scanner capture status
	//

	ES_LOG_TRACE_FUNC();

	if ( IsCaptured() ){
		return kESErrorNoError;
	}

	if ( IsAuthenticationSupported() && IsAuthenticationEnabled() ) {
		return RequestCaptureScannerWithAuth( GetAuthUserName(), GetAuthPassword() );
	}

	if ( !IsCaptureCommandSupported() ){
		return kESErrorNoError;
	}

	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( REQ_CAPTURE_SCANNER, ESC, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	switch (ack) {
		case kCaptureReplyNotReady:
			ES_LOG_NOT_READY();
			err = kESErrorDeviceInUse;
			break;

		case kCaptureReplySuccess:
			SetCaptured(true);
			break;

		case kCaptureReplyNack:
		default:
			break;
	}
	return err;
}

//===============================================================
//!
//! スキャナに占有終了を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestReleaseScanner()
{
	// 
	// ) コマンド送信   [ see R41AD7980/7990 ESC/I Command Specifications (R41AD7980_90_CmdSpecD.pdf) p75 ]
	//
	//   Scanner ------------------------------------------
	//                 ↑  ↓
	//               ①      ②
	//             ↑           ↓
	//   Host PC ------------------------------------------
	//
	//      1) ESC ) code
	//      2) Scanner release status
	//

	ES_LOG_TRACE_FUNC();

	if ( !IsCaptured() ){
		return kESErrorNoError;
	}

	if ( IsAuthenticationSupported() && IsAuthenticationEnabled() ) {
		return RequestReleaseScannerWithAuth();
	}

	if ( !IsCaptureCommandSupported() ){
		return kESErrorNoError;
	}

	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( REQ_RELEASE_SCANNER, ESC, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != kCaptureReplySuccess ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}

	SetCaptured(false);

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに占有開始を要求する。その際、ユーザー名とパスワードを合わせて送信する。
//!
//! @param strAuthUserName    ユーザー名。
//! @param strPassword   パスワード文字列
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestCaptureScannerWithAuth( __in const ESString& strAuthUserName, __in const ESString& strPassword )
{
	//
	// [ コマンド送信   [ see R41AD7980/7990 ESC/I Command Specifications (R41AD7980_90_CmdSpecD.pdf) p76 ]
	// 
	//   Scanner ------------------------------------------
	//               ↑  ↓          ↑  ↓
	//              ①     ②       ③    ④
	//            ↑         ↓   ↑        ↓ 
	//   Host PC ------------------------------------------
	//
	//     1) ESC [ code
	//     2) ACK (06h)
	//     3) User Authentication parameter d[40]
	//     4) Scanner capture status
	//

	ES_LOG_TRACE_FUNC();

	if ( !IsAuthenticationSupported() ){
		return kESErrorNoError;
	}

#ifdef WIN32
	ESStringA strAuthUserNameA = ES_CMN_FUNCS::STRING::UnicodeToMBString( strAuthUserName );
	ESStringA strHashOriginA = ES_CMN_FUNCS::STRING::UnicodeToMBString( strPassword );
#else
	ESStringA strAuthUserNameA = strAuthUserName;
	ESStringA strHashOriginA = strPassword;
#endif
	// strPasswordDataをSHA1ハッシュ化した20バイトのデータの作成。
	ESByteData bdPasswordData;
	ESErrorCode err = CreateSha1HashedPasswordData( strAuthUserNameA, strHashOriginA, bdPasswordData );
	if ( IS_ERR_CODE( err ) || bdPasswordData.size() != (MAX_USERNAME_LENGTH + HASH_DATA_SIZE) ){
		ES_LOG_INVALID_SETTING_PARAM();

		// 認証データを送らないとファームウェア側で認証をパスしてしまうため、
		// ユーザー名とパスワードが空の場合は、ダミーでも何らかの認証データを送る必要がある
		bdPasswordData.clear();
		bdPasswordData.resize( (MAX_USERNAME_LENGTH + HASH_DATA_SIZE), 0 );

		if ( strAuthUserNameA.size() <= MAX_USERNAME_LENGTH ){
			::memcpy_s( &bdPasswordData[0], MAX_USERNAME_LENGTH, strAuthUserNameA.c_str(), strAuthUserNameA.size() );
		}
	}

	// コマンド送信
	ESByte ack = ACK;
	err = SendCommand4A( REQ_CAPTURE_SCANNER_AUTH, ESC, bdPasswordData, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	switch (ack) {
		case kCaptureReplyNotReady:
			ES_LOG_NOT_READY();
			err = kESErrorDeviceInUse;
			break;

		case kCaptureReplyNoPermission:
			ES_LOG_INVALID_RESPONSE();
			err = kESErrorNoPermission;
			break;

		case kCaptureReplyNack:
			ES_LOG_AUTH_FAILURE();
			err = kESErrorAuthFailure;
			break;

		case kCaptureReplySuccess:
			SetCaptured(true);
			break;

		default:
			break;
	}
	return err;
}

//===============================================================
//!
//! スキャナに占有終了を要求する。requestCaptureScannerWithAuth:passwordで占有開始を要求した場合のみ使用可能。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestReleaseScannerWithAuth()
{
	//
	// ] コマンド送信   [ see R41AD7980/7990 ESC/I Command Specifications (R41AD7980_90_CmdSpecD.pdf) p77 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓
	//               ①     ②
	//             ↑         ↓
	//  Host PC ------------------------------------------
	//
	//     1) ESC ] code
	//     2) Scanner release status
	//

	ES_LOG_TRACE_FUNC();

	if ( !IsCaptured() ){
		return kESErrorNoError;
	}

	if ( !IsAuthenticationSupported() ){
		return kESErrorNoError;
	}
	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( REQ_RELEASE_SCANNER_AUTH, ESC, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != kCaptureReplySuccess ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}

	SetCaptured(false);

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに初期化を要求する。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestInitializeScanner()
{
	//
	// ESC @ コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p69 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓
	//               ①     ②
	//             ↑         ↓
	//  Host PC ------------------------------------------
	//
	//        1) ESC @ code
	//        2) ACK (06h)
	//

	ES_LOG_TRACE_FUNC();

	ESByte ack = ACK;
	ESErrorCode err = SendCommand2A( REQ_INIT_SCANNER, ESC, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに取込み装置の変更を要求する。
//!
//! @param eType                  取込み装置タイプ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetFunctionalUnitType( __in ENUM_ESCI_OPTION_CONTROL eType )
{
	//
	// ESC e コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p38 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓          ↑  ↓
	//               ①     ②       ③     ④
	//             ↑         ↓   ↑         ↓ 
	//  Host PC ------------------------------------------
	//  
	//     1) ESC e code
	//     2) ACK (06h)
	//     3) Parameter i
	//     4) ACK (06h) / NACK (15h)
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("type = %d"), eType );

	ESByteData param( sizeof(ENUM_ESCI_OPTION_CONTROL) );
	memcpy_s( &param[0], param.size(), &eType, sizeof(ENUM_ESCI_OPTION_CONTROL) );

	ESByte ack = ACK;
	ESErrorCode err = SendCommand4A( SET_OPTIONUNIT, ESC, param, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに色補正係数マトリックスの設定を要求する。
//!
//! @param matrix    色補正係数マトリックス。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetColorMatrix( __in const ES_COLOR_MATRIX& matrix )
{
	//
	//  ESC m コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p52 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓          ↑  ↓
	//               ①     ②       ③     ④
	//             ↑         ↓   ↑         ↓ 
	//  Host PC ------------------------------------------
	//  
	//     1) ESC m code
	//     2) ACK (06h)
	//     3) Color correction coefficients d[9]
	//     4) ACK (06h)
	//

	ES_LOG_TRACE_FUNC();

	if ( IsDumb() ){
		return kESErrorNoError;
	}

	// Matrix to be sent to scanner must be following d1-d9 order:
	//
	//     G  R  B
	// G [d1 d4 d7]
	// R [d2 d5 d8]
	// B [d3 d6 d9]
	//
	// So, we will convert it with index table.

	UInt8   index[9]    = {4, 1, 7, 3, 0, 6, 5, 2, 8};
	SInt8   ordered[9]  = {0};
	double  flatten[9]  = {0};
	int     rounded[9]  = {0};

	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			flatten[row * 3 + col] = matrix[row][col];
		}
	}
	ESCIRoundColorCorrectionMatrix(32, flatten, rounded);


	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			UInt8 oct = 0;
			int val = 0;

			val = rounded[row * 3 + col];
			oct = (UInt8)abs(val);
			oct |= ((val < 0) ? (1 << 7) : 0);
			ordered[index[row * 3 + col]] = oct;
		}
	}

	ESByteData data( &ordered[0], &ordered[9] );

	ESByte ack = ACK;
	ESErrorCode err = SendCommand4A( SET_COLORCORRECTIONMAT, ESC, data, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナにガンマテーブルの設定を要求する。
//!
//! @param eChannel カラーチャネル。
//! @param cGammaTable    ガンマテーブル（256バイト）。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetGammaTable( __in ESGammaChannel eChannel, __in CESGammaTable& cGammaTable )
{
	//
	// ESC z コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p52 ]
	//
	//  Scanner ------------------------------------------
	//              ↑  ↓          ↑  ↓
	//            ①     ②       ③     ④
	//          ↑         ↓   ↑         ↓ 
	//  Host PC ------------------------------------------
	// 
	//     1) ESC z code
	//     2) ACK (06h)
	//     3) Parameter i and Gamma table data d[256]
	//     4) ACK (06h) / NACK (15h)
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("channel = '%c'"), eChannel );

	ESByteData param( sizeof(ESGammaChannel) );
	memcpy_s( &param[0], param.size(), &eChannel, sizeof(ESGammaChannel) );
	ES_GAMMA_TABLE& table = cGammaTable.GetGammaTable();
	param.insert( param.end(), table, table + GAMMA_TABLE_SIZE );

	ESByte ack = ACK;
	ESErrorCode err = SendCommand4A( SET_GAMMATABLE, ESC, param, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナにフォーカスの設定を要求する。
//!
//! @param eFocus   フォーカス値。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetFocus( __in ESFocus eFocus )
{
	//
	// ESC p コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p65 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓          ↑  ↓
	//               ①     ②       ③     ④
	//             ↑         ↓   ↑         ↓ 
	//  Host PC ------------------------------------------
	//
	//     1) ESC p code
	//     2) ACK (06h)
	//     3) Parameter i
	//     4) ACK (06h)
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("focus = %d"), eFocus );

	ESByteData param( sizeof(ESFocus) );
	memcpy_s( &param[0], param.size(), &eFocus, sizeof(ESFocus) );

	ESByte ack = ACK;
	ESErrorCode err = SendCommand4A( SET_FOCUS, ESC, param, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに現在のフォーカス設定を要求する。
//!
//! @param eOutFocus   受信したフォーカス値を受け取るバッファ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestFocus( __out ESFocus& eOutFocus )
{
	//
	// ESC q コマンド送信  [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p66 ]
	//
	//  Scanner ------------------------------------------
	//                ↑  ↓      ↓
	//              ①      ②      ③
	//            ↑          ↓      ↓ 
	//  Host PC ------------------------------------------
	//
	//     1) ESC q code
	//     2) Information block
	//     3) Focus position status
	//

	ES_LOG_TRACE_FUNC();

	UInt8 un8Status = 0;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer data;
	ESErrorCode err = SendCommand3(GET_FOCUS, ESC, &un8Status, data);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( un8Status & kStatNotReady ){
		ES_LOG_NOT_READY();
		return kESErrorDeviceInBusy;
	}
	if ( un8Status & kStatFatalError ){
		ES_LOG_INVALID_STATUS();
		return kESErrorDeviceFatalError;
	}

	UInt8 u8FocusErr = 0;
	if ( data.GetLength() < (sizeof(UInt8)*2) ){
		return kESErrorFocusError;
	}
	memcpy_s( &u8FocusErr, sizeof(UInt8), data.GetBufferPtr(), sizeof(UInt8) );
	if ( u8FocusErr != 0 ){
		return kESErrorFocusError;
	}
	memcpy_s( &eOutFocus, sizeof(UInt8), data.GetBufferPtr()+sizeof(UInt8), sizeof(UInt8) );

	return err;
}

//===============================================================
//!
//! スキャナに現在のボタン押下状況を要求する。
//! スキャナがUSB Interruptによるボタンイベント通知に対応していない場合に使用する。
//!
//! @param un8OutButtonStatus  受信したボタン情報を受け取るバッファへのポインタ。スキャナのボタンには番号が割り振られており、その番号が入る。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestButtonStatus( __out UInt8& un8OutButtonStatus )
{
	//
	// ESC ! コマンド送信  [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p63 ]
	//
	//  Scanner ------------------------------------------
	//              ↑  ↓      ↓
	//            ①      ②      ③
	//          ↑          ↓      ↓ 
	//  Host PC ------------------------------------------
	//
	//     1) ESC ! code
	//     2) Information block
	//     3) Push button status
	//

//	ES_LOG_TRACE_FUNC();

	UInt8 un8Status = 0;
	ES_CMN_FUNCS::BUFFER::CESHeapBuffer data;
	ESErrorCode err = SendCommand3(GET_PUSHBUTTON_STATUS, ESC, &un8Status, data);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}

	if ( data.GetLength() < 1 ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	un8OutButtonStatus = data.GetBufferPtr()[0];

	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに自動電源オフへの移行時間の設定を要求する。
//!
//! @param 	un32PowerOffTime 自動電源オフ移行時間[min]。0を与えた場合は、自動電源オフしない。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetPowerOffTime( __in UInt32 un32PowerOffTime )
{
	return RequestWriteLogForFunction( kLogFunctionPowerOffTime, un32PowerOffTime );
}

//===============================================================
//!
//! スキャナに自動電源オフへの移行時間の取得を要求する。
//!
//! @param 	un32OutPowerOffTime 省電力移行時間[min]。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestGetPowerOffTime( __out UInt32& un32OutPowerOffTime )
{
	return RequestReadLogForFunction( kLogFunctionPowerOffTime, un32OutPowerOffTime );
}

//===============================================================
//!
//! スキャナにメンテナンスコマンドの設定を要求する。
//!
//! @param stParameter   メンテナンスパラメータ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestMaintenanceWithParameter( __in ST_ESCI_MAINTENANCE_PARAMETER stParameter )
{
	//
	// ESC 1 コマンド送信 
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("paramter.mode = %d"), stParameter.un16Mode );

	stParameter.un16Mode = (UInt16)CFSwapInt16HostToLittle(stParameter.un16Mode);

	ESByteData data( sizeof(ST_ESCI_MAINTENANCE_PARAMETER), 0 );
	memcpy_s( &data[0], data.size(), &stParameter, sizeof(stParameter) );

	UInt8 ack = ACK;
	ESErrorCode err = SendCommand4A( REQ_MAINTENANCE, ESC, data, ack );
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	switch (ack) {
		case NACK:
			ES_LOG_INVALID_RESPONSE();
			err = kESErrorDeviceFatalError;
			break;
		case BUSY:
			ES_LOG_BUSY();
			err = kESErrorDeviceInUse;
			break;
		case ACK:
		default:
			break;
	}
	return err;
}

//===============================================================
//!
//! スキャナにメンテナンスコマンドの実行ステータスを要求する。
//! 
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestMaintenanceStatus()
{
	ES_LOG_TRACE_FUNC();

	ST_ESCI_MAINTENANCE_PARAMETER stParameter = {0};

	stParameter.un16Mode = kMaintenanceStatusRequest;

	return RequestMaintenanceWithParameter(stParameter);
}

//===============================================================
//!
//! スキャナにスキャンパラメータの設定を要求する。
//!
//!  @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestSetScanningParameters( __in ST_ESCI_SCANNING_PARAMETER eParameters )
{
	//
	// FS W コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p89 ]
	//
	//   Scanner ------------------------------------------
	//                ↑  ↓          ↑  ↓
	//              ①      ②       ③     ④
	//            ↑          ↓   ↑         ↓ 
	//   Host PC ------------------------------------------
	//
	//     1) FS W code
	//     2) ACK (06h)
	//     3) Scanning parameter d[64]
	//     4) ACK (06h) / NACK (15h)
	//

	ES_LOG_TRACE_FUNC();

	ES_INFO_LOG( ES_STRING("X Resolution        = %d"), eParameters.un32XResolution );
	ES_INFO_LOG( ES_STRING("Y Resolution        = %d"), eParameters.un32YResolution );
	ES_INFO_LOG( ES_STRING("X Offset            = %d"), eParameters.un32XOffset );
	ES_INFO_LOG( ES_STRING("Y Offset            = %d"), eParameters.un32YOffset );
	ES_INFO_LOG( ES_STRING("Width               = %d"), eParameters.un32Width );
	ES_INFO_LOG( ES_STRING("Height              = %d"), eParameters.un32Height );
	ES_INFO_LOG( ES_STRING("Color               = %d"), eParameters.un8Color );
	ES_INFO_LOG( ES_STRING("Data Format         = %d"), eParameters.un8DataFormat );
	ES_INFO_LOG( ES_STRING("Option Control      = %d"), eParameters.un8OptionControl );
	ES_INFO_LOG( ES_STRING("Scanning Mode       = %d"), eParameters.un8ScanningMode );
	ES_INFO_LOG( ES_STRING("Block Line Number   = %d"), eParameters.un8BlockLineNumber );
	ES_INFO_LOG( ES_STRING("Gamma Correction    = %d"), eParameters.un8GammaCorrection );
	ES_INFO_LOG( ES_STRING("Brightness          = %d"), eParameters.un8Brightness );
	ES_INFO_LOG( ES_STRING("Color Correction    = %d"), eParameters.un8ColorCorrection );
	ES_INFO_LOG( ES_STRING("HalftoneMode        = %d"), eParameters.un8HalftoneMode );
	ES_INFO_LOG( ES_STRING("Threshold           = %d"), eParameters.un8Threshold );
	ES_INFO_LOG( ES_STRING("Auto Area Seg       = %d"), eParameters.un8AutoAreaSeg );
	ES_INFO_LOG( ES_STRING("Sharpness           = %d"), eParameters.un8Sharpness );
	ES_INFO_LOG( ES_STRING("Mirroring           = %d"), eParameters.un8Mirroring );
	ES_INFO_LOG( ES_STRING("Film Type           = %d"), eParameters.un8FilmType );
	ES_INFO_LOG( ES_STRING("Lamp Mode           = %d"), eParameters.un8LampMode );
	ES_INFO_LOG( ES_STRING("DoubleFeedDetection = %d"), eParameters.un8DoubleFeedDetection );
	ES_INFO_LOG( ES_STRING("Auto Scan           = %d"), eParameters.un8AutoScan );
	ES_INFO_LOG( ES_STRING("Quiet Mode          = %d"), eParameters.un8QuietMode );
	ES_INFO_LOG( ES_STRING("LightIntensity      = %d"), eParameters.un16LightIntensity);

	eParameters.un32XResolution    = (UInt32)CFSwapInt32HostToLittle(eParameters.un32XResolution);
	eParameters.un32YResolution    = (UInt32)CFSwapInt32HostToLittle(eParameters.un32YResolution);
	eParameters.un32XOffset        = (UInt32)CFSwapInt32HostToLittle(eParameters.un32XOffset);
	eParameters.un32YOffset        = (UInt32)CFSwapInt32HostToLittle(eParameters.un32YOffset);
	eParameters.un32Width          = (UInt32)CFSwapInt32HostToLittle(eParameters.un32Width);
	eParameters.un32Height         = (UInt32)CFSwapInt32HostToLittle(eParameters.un32Height);
	eParameters.un16LightIntensity = (UInt16)CFSwapInt16HostToLittle(eParameters.un16LightIntensity);

	ESByteData param(sizeof(ST_ESCI_SCANNING_PARAMETER));
	memcpy_s(&param[0], param.size(), &eParameters, sizeof(eParameters) );

	UInt8 ack = ACK;
	ESErrorCode err =SendCommand4A( SET_SCANNINGPARAMETER, FS, param, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに保存されているログ情報を要求する。スキャンカウンターの取得などで使用する。
//!
//! @param eFunction   ログ取得したい機能。
//! @param un32OutValue    取得されたログ情報。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestReadLogForFunction( __in ENUM_ESCI_LOG_FUNCTION eFunction, __out UInt32& un32OutValue )
{
	//
	// ESC 0xE2
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("function = %d"), eFunction );

	UInt16 un16SwappedFunc = (UInt16)CFSwapInt16HostToLittle(eFunction);

	ESByteData param(sizeof(ENUM_ESCI_LOG_FUNCTION));
	memcpy_s(&param[0], param.size(), &un16SwappedFunc, sizeof(un16SwappedFunc) );

	ESErrorCode err =SendCommand4( GET_LOGINFO, ESC, param, (PESByte)&un32OutValue, sizeof(UInt32));
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	un32OutValue = (UInt32)CFSwapInt32LittleToHost(un32OutValue);

	ES_INFO_LOG( ES_STRING("value = %d"), un32OutValue );
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナに保存されているログ情報の書き換えを要求する。スキャンカウンターのリセットなどで使用する。
//!
//! @param eFunction   ログ書き換えをしたい機能。
//! @param un32Value   書き換えるログ情報。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestWriteLogForFunction( __in ENUM_ESCI_LOG_FUNCTION eFunction, __in UInt32 un32Value )
{
	//
	// ESC 0xE1
	//

	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG( ES_STRING("function = %d, value = %d"), eFunction, un32Value );

	UInt16 un16SwappedFunc = (UInt16)CFSwapInt16HostToLittle(eFunction);
	UInt32 un32SwappedValue = (UInt32)CFSwapInt32HostToLittle(un32Value);

	ESByteData param(sizeof(ENUM_ESCI_LOG_FUNCTION));
	memcpy_s(&param[0], param.size(), &un16SwappedFunc, sizeof(un16SwappedFunc) );
	param.insert( param.end(), (PESByte)&un32SwappedValue, ((PESByte)&un32SwappedValue) + sizeof(UInt32) );

	ESByte ack = ACK;
	ESErrorCode err = SendCommand4A( SET_LOGINFO, ESC, param, ack);
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	if ( ack != ACK ){
		ES_LOG_INVALID_RESPONSE();
		return kESErrorInvalidResponse;
	}
	return kESErrorNoError;
}

//===============================================================
//!
//! スキャナにスキャンの開始を要求する。
//!
//! @param stImageInfoBlock   スキャナから受信する転送バイト数・ブロック数の情報を受け取るバッファへのポインタ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::RequestStartScanning( __out ST_ESCI_IMAGE_INFO_BLOCK& stImageInfoBlock )
{
	//
	// FS G コマンド送信   [ see ESC/I-B Command Specifications (EsciBCmdEM.pdf) p88 ]
	//

	ES_LOG_TRACE_FUNC();

	const int WAIT_MAX = 60;	// seconds
	ESErrorCode err = SendCommand2( REQ_START_SCANNING, FS, (PESByte)&stImageInfoBlock, sizeof(stImageInfoBlock));
	if ( IS_ERR_CODE( err ) ){
		ES_LOG_FAILED_SEND_COMMAND();
		return err;
	}
	// Retry if error is in info block
	if ( stImageInfoBlock.un8Status & kStatFatalError ){
		err = GetErrorStatus();
		if ( IS_ERR_CODE( err ) ){
			return err;
		}

		err = WaitWhileWarmingUp( WAIT_MAX );
		if ( IS_ERR_CODE( err ) ){
			return err;
		}
		err = SendCommand2( REQ_START_SCANNING, FS, (PESByte)&stImageInfoBlock, sizeof(ST_ESCI_IMAGE_INFO_BLOCK));
		if ( IS_ERR_CODE( err ) ){
			ES_LOG_FAILED_SEND_COMMAND();
			return err;
		}
	}

	if ( stImageInfoBlock.un8Status & kStatNotReady ){
		ES_LOG_NOT_READY();
		return kESErrorDeviceInBusy;
	}
	if ( stImageInfoBlock.un8Status & kStatFatalError ){
		ES_LOG_INVALID_STATUS();
		return kESErrorDeviceFatalError;
	}

	stImageInfoBlock.un32ByteCounter			= (UInt32)CFSwapInt32LittleToHost(stImageInfoBlock.un32ByteCounter);
	stImageInfoBlock.un32BlockNumber			= (UInt32)CFSwapInt32LittleToHost(stImageInfoBlock.un32BlockNumber);
	stImageInfoBlock.un32LastBlockByteCounter	= (UInt32)CFSwapInt32LittleToHost(stImageInfoBlock.un32LastBlockByteCounter);

	// Request extended transfer if the scanner supports
	if ( IsUsesExtTransfer() ){
		if (m_pDevInterface){
	
			err = m_pDevInterface->StartExtendedTransferWithBlocks( stImageInfoBlock.un32BlockNumber + 1,	// REQ_START_SCANNINGの結果が1引かれた数値のため加算
																	  stImageInfoBlock.un32ByteCounter + 1,	// 1Byte分の情報ブロック分を追加
																	  stImageInfoBlock.un32LastBlockByteCounter + 1 );	// 1Byte分の情報ブロック分を追加
			if ( IS_ERR_CODE( err ) ){
				ES_LOG_FAILED_SEND_COMMAND();
			}
		} else {
			ES_LOG_INVALID_SETTING_PARAM();
			err = kESErrorFatalError;
		}
	}

	return err;
}

//===============================================================
//
// スキャナに1ページのスキャンを要求し、CESScannedImageに画像データを書き込む。
//
// @param pImage  画像データを書き込むためのCESScannedImage
//
// @return エラーコード。
//
//===============================================================
ESErrorCode CESCICommand::RequestScanToImage( __out CESScannedImage*& pImage )
{
	ES_LOG_TRACE_FUNC();

	ESErrorCode err = kESErrorNoError;
	ST_ESCI_IMAGE_INFO_BLOCK stInfoBlock = {};

	CallDelegateScannerWillScanToScannedImage( pImage );
	pImage->Open();

	SYNCHRONIZED_START
	{
		UInt32 un32BlockCounter = 0;
		err = RequestStartScanning( stInfoBlock );
		if ( IS_ERR_CODE( err ) ){
			goto BAIL;
		}

		ES_INFO_LOG( ES_STRING("Header                  = %d"), stInfoBlock.un8Header );
		ES_INFO_LOG( ES_STRING("Status                  = %d"), stInfoBlock.un8Status );
		ES_INFO_LOG( ES_STRING("Block Number            = %d"), stInfoBlock.un32BlockNumber );
		ES_INFO_LOG( ES_STRING("Byte Counter            = %d"), stInfoBlock.un32ByteCounter );
		ES_INFO_LOG( ES_STRING("Last Block Byte Counter = %d"), stInfoBlock.un32LastBlockByteCounter );

		while ( 1 ){
			UInt8 un8ErrCode = 0;
			UInt32 un32BytesToRead = stInfoBlock.un32ByteCounter;

			if ( un32BlockCounter == stInfoBlock.un32BlockNumber ){
				un32BytesToRead = stInfoBlock.un32LastBlockByteCounter;
			}

			// Read image data
			ES_CMN_FUNCS::BUFFER::CESHeapBuffer cImageBuffer;
			err = ReadImageData( cImageBuffer, un32BytesToRead, un8ErrCode );
			if ( IS_ERR_CODE( err ) ){
				break;
			}

			err = pImage->WriteData( cImageBuffer );
			if ( IS_ERR_CODE( err ) ){
				SetCancelled(true);
				RequestCancelScanning();
				break;
			}

			// Check error code in data block
			if ( un8ErrCode & kBlockESErrorFatalError ){
				err = GetErrorStatus();
				break;
			}

			// Cancel
			if ( ((un8ErrCode & kBlockErrorCancel) && !IsShouldIgnoreCancelFromScanner()) || IsCancelled() ){

				SetCancelled(true);

				CallDelegateScannerWillCancelScanning();

				if ( un32BlockCounter < stInfoBlock.un32BlockNumber ){
					err = RequestCancelScanning();
					if ( IS_ERR_CODE( err ) ){
						break;
					}
				}
				if ( IsFeederEnabled() ){
					err = RequestEjectPaper();
					if ( IS_ERR_CODE( err ) ){
						break;
					}
				}

				CallDelegateScannerDidCancelScanning();
				DeleteScanedImage( &pImage );
				break;
			}

			if ( un32BlockCounter < stInfoBlock.un32BlockNumber ){
				// Paper end
				if ( (un8ErrCode & kBlockErrorPaperEnd) && IsPaperEndDetectionEnabled() ){
					UInt32 un32Width  = 0;
					UInt32 un32Height = 0;

					err = RequestEndTransmission();
					if ( IS_ERR_CODE( err ) ){
						break;
					}

					{
						ST_ESCI_SCANNING_PARAMETER stScanningParameter = {};
						err = RequestScanningParameter( stScanningParameter );
						if ( IS_ERR_CODE( err ) ){
							ES_LOG_FAILED_SEND_COMMAND();
							break;
						}
						un32Width    = CFSwapInt32LittleToHost( stScanningParameter.un32Width );
						un32Height   = CFSwapInt32LittleToHost( stScanningParameter.un32Height );
					}
					pImage->CloseWithWidth( (ESNumber)un32Width, (ESNumber)un32Height );

					break;
				} else {
					err = SendAckForImageData();
					if ( IS_ERR_CODE( err ) ){
						break;
					}
				}
			} else {
				pImage->Close();
				break;
			}

			un32BlockCounter += 1;

		} // while

	}
	SYNCHRONIZED_END

BAIL:
	if ( IS_ERR_SUCCESS(err) ){
		if ( pImage != NULL ){
			CallDelegateScannerDidScanToScannedImage( pImage );
		}
	} else {
		RequestInitializeScanner();
	}
	DeleteScanedImage( &pImage );

	return err;
}

//===============================================================
//!
//! スキャナから画像データを受信する。
//!
//! @param cOutData    受信した画像データを受け取るバッファクラス。
//! @param un32BytesToRead   受信する画像データの期待される長さ。
//! @param un8OutErrorCode  画像データ転送中に、画像データブロックごとにスキャナが付加してくるエラーコードを格納するバッファ。
//!
//! @return エラーコード。
//!
//===============================================================
ESErrorCode CESCICommand::ReadImageData( ES_CMN_FUNCS::BUFFER::IESBuffer& cOutData, UInt32 un32BytesToRead, UInt8& un8OutErrorCode )
{
	ES_LOG_TRACE_FUNC();
	ES_INFO_LOG(ES_STRING("length = %d"), un32BytesToRead );

	ES_CMN_FUNCS::BUFFER::CESHeapBuffer cBuffer;
	if ( !cBuffer.AllocBuffer( un32BytesToRead + 1 ) ){ // 読み込み画像サイズ + エラーコード
		ES_LOG_MEMORY_ALLOCATE_ERROR();
		return kESErrorMemoryError;
	}
	if ( !cOutData.AllocBuffer( un32BytesToRead ) ){
		ES_LOG_MEMORY_ALLOCATE_ERROR();
		return kESErrorMemoryError;
	}

	ESErrorCode err = kESErrorNoError;
	bool bReaded = false;
	if ( IsUsesExtTransfer() ){
		if (m_pDevInterface){
			if ( IS_ERR_CODE(m_pDevInterface->ReadExtendedTransfer( cBuffer.GetBufferPtr(), cBuffer.GetLength() )) ){
				err = kESErrorDataReceiveFailure;
			}
			bReaded = true;
		}
	}
	if ( !bReaded ){
		err = Read( cBuffer.GetBufferPtr(), cBuffer.GetLength() );
	}

	ES_INFO_LOG(ES_STRING("length = %d"), cBuffer.GetLength() );

	if ( IS_ERR_CODE(err) ){
		goto BAIL;
	}

	un8OutErrorCode = *(UInt8*)( cBuffer.GetBufferPtr() + un32BytesToRead );
	// Apply color matrix for dumb scanner
	ApplyColorMatrix( cBuffer, un32BytesToRead );

	memcpy_s( cOutData.GetBufferPtr(), cOutData.GetLength(), cBuffer.GetBufferPtr(), un32BytesToRead );

BAIL:
	return err;
}
