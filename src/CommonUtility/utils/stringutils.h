#pragma once

#include "../ESCommonTypedef.h"

namespace ES_CMN_FUNCS
{
	namespace STRING
	{

		// マルチバイト→Unicode変換
		ESStringW MBStringToUnicode(const ESStringA& strSrcA);
		// Unicode→マルチバイト変換
		ESStringA UnicodeToMBString(const ESStringW& strSrcW);
		// Unicode→UTF8変換
		ESStringA UnicodeToUTF8(const ESStringW& strSrcW);
		// UTF8→Unicode変換
		ESStringW UTF8ToUnincode(const ESStringA& strSrcA);

		ESStringW ReplaceStr( ESStringW StringIn, ESStringW StringSrc, ESStringW StringNew );

#ifndef __ES_CMN_CONVERT_STRING__
#define __ES_CMN_CONVERT_STRING__
	#ifdef WIN32
		inline ESString ToESString(const ESStringA& s){
			return MBStringToUnicode(s);
		}
		inline ESString ToESString(const ES_CHAR_A* s){
			return MBStringToUnicode(s);
		}
		inline ESString ToESString(const ESStringW& s){
			return s;
		}
		inline ESString ToESString(const ES_CHAR_W* s){
			return s;
		}
	#else
		inline ESString ToESString(const ESStringA& s){
			return s;
		}
		inline ESString ToESString(const ES_CHAR_A* s){
			return s;
		}
		inline ESString ToESString(const ESStringW& s){
			return UnicodeToMBString(s);
		}
		inline ESString ToESString(const ES_CHAR_W* s){
			return UnicodeToMBString(s);
		}
	#endif
#endif // __ES_CMN_CONVERT_STRING__
	}
}
