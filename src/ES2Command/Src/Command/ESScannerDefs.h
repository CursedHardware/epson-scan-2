////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     ESScannerDefs.h
//!
//! @brif     スキャナーコマンド用 パラメーター定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

const DWORD c_dwButtonCheckIntervalForComunication_ms	= 1000; // 1sec
const DWORD c_dwButtonCheckIntervalForInterrupt_ms		= 200;	// 200 ms


STATIC_CONST_KEY ES_CAPABILITY_KEY_ALLVALUES		= ES_STRING("AllValues");
STATIC_CONST_KEY ES_CAPABILITY_KEY_AVAILABLEVALUES	= ES_STRING("AvailableValues");
STATIC_CONST_KEY ES_CAPABILITY_KEY_DEFAULT			= ES_STRING("Default");