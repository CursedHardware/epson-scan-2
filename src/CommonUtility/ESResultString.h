
#pragma once

#include "ESCommonTypedef.h"

//----------------------------------------=----------------------------------------��
/*!@brief	������ۑ��p�C���^�[�t�F�[�X
*/
class IESResultString {
public:
	virtual ~IESResultString() {
	}

	//----------------------------------------=----------------------------------------��
	/*!@brief	�������setter
	@param[in]		pszString ������
	@return			������ۑ��ɐ��������ꍇtrue��Ԃ��A���s�����ꍇfalse��Ԃ��B
	*/
	virtual bool		Set(const ES_CHAR *pszString) = 0;


	//----------------------------------------=----------------------------------------��
	/*!@brief	�������getter
	@return			������ւ̃|�C���^��Ԃ��B
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

