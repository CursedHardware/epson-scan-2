/*!
 * @file	ESZipfileUtils.h
 * @brief	ZIPファイルの読み込みを行う
 * @author	SEC
 * @par		Copyright:
 *			Copyright (C) SEIKO EPSON CORPORATION 2014. All rights reserver. <BR>
 */
#pragma once
#include "CommonUtility/ESCommonTypedef.h"
#include "CommonUtility/utils/zipfile/zipfile.h"
#include "CommonUtility/utils/PathUtils.h"
#include "CommonUtility/utils/stringutils.h"
#include "CommonUtility/utils/ESJsonUtils.h"

namespace ES_CMN_FUNCS
{
	namespace ZIPFILE
	{

		/*!
		 * @brief	ZIPファイルは階層構造の区切り文字が Unix タイプのため, windows like な '\\' のときは置換する
		 */
		static std::string exchangeDelimiter(const std::string& strSrc)
		{
			std::stringstream ssTmp(strSrc);
			std::vector<std::string> vTmp;
			std::string strTmp;
			while(std::getline(ssTmp, strTmp, '\\'))
			{
				if(strTmp.size() != 0)
				{
					vTmp.push_back(strTmp);
				}
			}
			std::stringstream ssDst;
			for(auto itr = vTmp.begin(), itr_end = vTmp.end(); itr != itr_end; ++itr)
			{
				if(itr != vTmp.begin())
				{
					ssDst << '/';
				}
				ssDst << *itr;
			}
			return ssDst.str();
		}

		/*!
		 * @brief	Zipfileから指定されたセクタを読み込み, JSON->辞書変換して取得する
		 * @param[in]	strFilePath		zipファイルのパス
		 * @param[in]	strEntryPath	zipファイル内のセクタのパス
		 * @param[out]	dictDst			結果辞書
		 */
		static UInt32 ZipfileToDictionary(const ESString& strFilePath, const ESString& strEntryPath, ESDictionary& dictDst)
		{
			/* Zipfileを開く */
			ZipFile zipfile;
			if(!ES_CMN_FUNCS::PATH::ES_IsExistFile(strFilePath))
			{
				return ES_CMN_FUNCS::JSON::ES_JSON_INVALID_ERR;
			}

			int nRet = zipfile.Open(strFilePath.c_str());
			if(nRet)
			{
				return ES_CMN_FUNCS::JSON::ES_JSON_INVALID_ERR;
			}

			/* エントリを読み込む */
			/* エントリパスはWindowsのため, MultibyteString */
#ifdef WIN32
			const ESStringA strEntryPathMB = exchangeDelimiter(ES_CMN_FUNCS::STRING::UnicodeToMBString(strEntryPath));
#else
			const ESStringA strEntryPathMB = exchangeDelimiter(strEntryPath);
#endif
			nRet = zipfile.Read(strEntryPathMB.c_str());
			if(nRet)
			{
				return ES_CMN_FUNCS::JSON::ES_JSON_INVALID_ERR;
			}
			const unsigned int nSize = zipfile.GetSize();
			const char* pTmp = zipfile.GetPtr();
			if(nSize > 0 && pTmp)
			{
				/* 辞書変換 */
				/* JSON ファイルは  BOMなしUTF-8 */
#ifdef WIN32
				return ES_CMN_FUNCS::JSON::JSONtoDictionary(ES_CMN_FUNCS::STRING::UTF8ToUnincode(pTmp), dictDst);
#else
				return ES_CMN_FUNCS::JSON::JSONtoDictionary(pTmp, dictDst);
#endif
			}
			return ES_CMN_FUNCS::JSON::ES_JSON_INVALID_ERR;
		}
	};
};
