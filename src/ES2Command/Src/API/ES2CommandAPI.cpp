////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScannerAPI.cpp
//!
//! @brif     スキャナー操作インターフェースの実装
//!           DLL アプリケーション用にエクスポートされる関数を実装します。
//! @note    
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Include/ES2Command/ES2CommandAPI.h"
#include "Command/ESScanner.h"

//==============================================================================================
//!
//!  スキャナ操作インターフェースの作成
//!
//!  @param
//!   eCommandType : スキャナのコマンドタイプ
//!   ppScanner    : 作成されたスキャナーインターフェースを受け取るポインタへのポインタ
//!  @return
//!   エラーコード
//!    成功 : kESErrorNoError
//!    失敗 : その他
//!
//==============================================================================================
extern "C" ESSCANNER_API ESErrorCode ESCreateScanner( ESCommandType eCommandType, IESScanner** ppScanner )
{
	ES_LOG_TRACE_FUNC();

	if ( ppScanner == NULL ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorInvalidParameter;
	}
	*ppScanner = NULL;
	CESScanner* pScanner = NULL;
	try {
		pScanner = CESScanner::CreateInstance(eCommandType);
	} catch( const std::bad_alloc& ){
		ES_LOG_MEMORY_ALLOCATE_ERROR();
		return kESErrorMemoryError;
	} catch( const std::invalid_argument& ){
		ES_LOG_INVALID_INPUT_PARAM();
		return kESErrorFatalError;
	} catch(...){
		ES_LOG_EXCEPTION_ERROR();
		return kESErrorFatalError;
	}

	ESErrorCode ret = kESErrorFatalError;
	if (pScanner){
		ret = pScanner->Initialize();
		if ( IS_ERR_SUCCESS(ret) ){
			*ppScanner = dynamic_cast< IESScanner* >( pScanner );
		} else {
			pScanner->DestroyInstance();
		}
	}
	return ret;
}

