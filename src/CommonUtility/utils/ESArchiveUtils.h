/*!
 * @file	ESArchiveUtils.h
 * @brief	Archive�t�@�C��(*.db or +.json)�̓ǂݍ��݂�B������
 * @author	SEC
 * @par		Copyright:
 *			Copyright (C) SEIKO EPSON CORPORATION 2014. All rights reserver. <BR>
 */
#pragma once
#include "CommonUtility/ESCommonTypedef.h"
#ifdef WIN32
#include "CommonUtility/utils/ESZipfileUtils.h"
#endif
#include "CommonUtility/utils/ESJsonUtils.h"

#ifdef WIN32
#if 1	/* �b��I�ɉB�� */
#define _ZIP_DB
#endif
#endif

namespace ES_CMN_FUNCS
{
	namespace ARCHIVE
	{

#ifdef _ZIP_DB
		static UInt32 ArchiveToDictionary(const ESString& strDBPath, const ESString& strEntryPath, ESDictionary& dictDst)
		{
			const UInt32 nRet = ES_CMN_FUNCS::ZIPFILE::ZipfileToDictionary(strDBPath, strEntryPath, dictDst);
			assert(nRet == 0);
			return nRet;
		}
#else
		static UInt32 ArchiveToDictionary(const ESString& strDBPath, const ESString& strEntryPath, ESDictionary& dictDst)
		{
			return  ES_CMN_FUNCS::JSON::JSONFiletoDictionary(strEntryPath, dictDst);
		}
#endif
	};
};
