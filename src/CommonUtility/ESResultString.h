
#pragma once

#include "ESCommonTypedef.h"

//----------------------------------------=----------------------------------------┐
/*!@brief	文字列保存用インターフェース
*/
class IESResultString {
public:
	virtual ~IESResultString() {
	}

	//----------------------------------------=----------------------------------------┐
	/*!@brief	文字列のsetter
	@param[in]		pszString 文字列
	@return			文字列保存に成功した場合trueを返し、失敗した場合falseを返す。
	*/
	virtual bool		Set(const ES_CHAR *pszString) = 0;


	//----------------------------------------=----------------------------------------┐
	/*!@brief	文字列のgetter
	@return			文字列へのポインタを返す。
	*/
	virtual const ES_CHAR	*Get() = 0;
};

class CESResultString : public IESResultString{
public:
	CESResultString() {
		m_String.clear();
	}

	virtual ~CESResultString() {
		m_String.clear();
	}

	virtual bool		Set(const ES_CHAR *pszString) {
		if (!pszString){
			return false;
		}
		try {
			m_String = pszString;
		}
		catch (const std::bad_alloc &) {
			_ASSERT(FALSE);
			return false;
		}
		return true;
	}
	virtual const ES_CHAR	*Get() {
		return m_String.c_str();
	}

	ESString	  m_String;
};

