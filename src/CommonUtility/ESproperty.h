
#pragma once

#include "ESCommonTypedef.h"
#include "DbgLog.h"


class CESProperty{
public:
	CESProperty()
	{
		m_bReadOnly  = FALSE;
	}

	//ESAnyをプロパティとして保持して使う。Setter、Getterは自動生成。
	explicit CESProperty(ESAny anyIn,BOOL bReadOnly = FALSE)
	{
		m_bReadOnly = bReadOnly;

		m_anyValue		=anyIn;
		m_strTypeName	=anyIn.type().name();
	}

	explicit CESProperty(ESStringA strTypeName,BOOL bReadOnly = FALSE)
	{
		m_bReadOnly = bReadOnly;

		ESAny empty;
		m_anyValue		=empty;
		m_strTypeName	=strTypeName;
	}

	// コピーコンストラクタ
	CESProperty( const CESProperty& rhs )
	{
		m_bReadOnly   = rhs.m_bReadOnly;
		m_anyValue    = rhs.m_anyValue;
		m_strTypeName = rhs.m_strTypeName;
	}

	// デストラクタ
	~CESProperty()
	{
	}

	//コピーオペレータ
	CESProperty& operator = ( const CESProperty& rhs )
	{
		m_bReadOnly   = rhs.m_bReadOnly;
		m_anyValue    = rhs.m_anyValue;
		m_strTypeName = rhs.m_strTypeName;

		return *this;
	}


public:
	BOOL SetValue(const ESAny& anyIn)
	{
		if(m_bReadOnly){
			return FALSE;
		}

		if ( anyIn.type().name() == m_strTypeName ){
			m_anyValue = anyIn;
			return TRUE;
		}else{
			//型の異なる値をセットしようとしている
			ES_Error_Log(this, ES_STRING("Wrong type Property set!!"));
			return FALSE;
		}
	}

	ESAny GetValue()
	{
		return m_anyValue;
	}

private:
	BOOL				m_bReadOnly;
	ESAny				m_anyValue;
	ESStringA			m_strTypeName;
};

typedef std::unordered_map< ESString,  CESProperty > PROPERTY_MAP;
typedef PROPERTY_MAP::value_type				PROPERTY_MAP_PAIR;
