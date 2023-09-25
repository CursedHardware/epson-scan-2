
#if !defined(_UNICODE) || !defined(UNICODE)
//	#error This file is supporting only unicode.
#endif

#pragma once

#if defined WIN32
#include <windows.h>
#include <tchar.h>
#else
#include <Platform.h>
#endif

#include <string>
#include <sstream>
#define BOOST_NO_CXX11_RVALUE_REFERENCES 1
#include <boost/any.hpp>
//#include <boost/unordered_map.hpp
#include <unordered_map>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <cassert>

#include "../Include/es2CommnoType.h"

typedef std::basic_string< ES_CHAR >				ESString;
typedef std::basic_string< ES_CHAR_A >			ESStringA;
typedef std::basic_string< ES_CHAR_W >			ESStringW;

typedef std::deque< ESString  >					ESStringArray;
typedef std::deque< ESStringA >					ESStringArrayA;
typedef std::deque< ESStringW >					ESStringArrayW;

typedef std::deque< ESStringArray >				ESStringMultiArray;
typedef std::deque< ESStringArrayA >			ESStringMultiArrayA;
typedef std::deque< ESStringArrayW >			ESStringMultiArrayW;

typedef std::set< ESString  >					ESStringSet;
typedef std::set< ESStringA >					ESStringSetA;
typedef std::set< ESStringW >					ESStringSetW;

//-------------------------------------------------------
// string stream
//-------------------------------------------------------
typedef std::basic_istringstream< ES_CHAR >		ESistringstream;
typedef std::basic_istringstream< ES_CHAR_A >		ESistringstreamA;
typedef std::basic_istringstream< ES_CHAR_W >		ESistringstreamW;

typedef std::basic_ostringstream< ES_CHAR >		ESostringstream;
typedef std::basic_ostringstream< ES_CHAR_A >		ESostringstreamA;
typedef std::basic_ostringstream< ES_CHAR_W >		ESostringstreamW;

typedef std::basic_stringstream< ES_CHAR >		ESstringstream;
typedef std::basic_stringstream< ES_CHAR_A >		ESstringstreamA;
typedef std::basic_stringstream< ES_CHAR_W >		ESstringstreamW;


//-------------------------------------------------------
// any
//-------------------------------------------------------
typedef boost::any								ESAny;


//-------------------------------------------------------
// dictionary
//-------------------------------------------------------
typedef std::map< ESString,  ESAny >	ESDictionary;
typedef std::map< ESStringA, ESAny >	ESDictionaryA;
typedef std::map< ESStringW, ESAny >	ESDictionaryW;

typedef ESDictionary::value_type				ESDicPair;
typedef ESDictionaryA::value_type				ESDicPairA;
typedef ESDictionaryW::value_type				ESDicPairW;

//-------------------------------------------------------
// array
//-------------------------------------------------------
typedef std::vector< UInt8 >					ESByteData;

typedef std::deque< ESDictionary >				ESDicArray;

typedef std::deque< ESAny >						ESAnyArray;

typedef std::set< ESNumber >					ESIndexSet;

typedef std::deque< ESNumber >					ESIndexArray;
typedef std::deque< ESUnsigned >				ESUnsignedArray;
typedef std::deque< ESFloat >					ESFloatArray;

typedef std::deque< ESIndexArray >				ESIndexMultiArray;
typedef std::deque< ESUnsignedArray >			ESUnsignedMultiArray;
typedef std::deque< ESFloatArray >				ESFloatMultiArray;

//-------------------------------------------------------
// range
//-------------------------------------------------------
#pragma pack(push, 1)

typedef struct tagESRange {
	ESNumber nMin;
	ESNumber nMax;
	ESNumber nStep;
} ST_ES_RANGE;

typedef struct tagESRangeF {
	ESFloat fMin;
	ESFloat fMax;
	ESFloat fStep;
} ST_ES_RANGE_F;

typedef struct tagESRangeUN32 {
	UInt32 un32Min;
	UInt32 un32Max;
	UInt32 un32Step;
} ST_ES_RANGE_UN32;

#pragma pack(pop)

inline ST_ES_RANGE MakeRange( ESNumber nMin, ESNumber nMax, ESNumber nStep )
{
	ST_ES_RANGE stRange = { nMin, nMax, nStep };
	return stRange;
}

inline ST_ES_RANGE_F MakeRange( ESFloat fMin, ESFloat fMax, ESFloat fStep )
{
	ST_ES_RANGE_F stRange = { fMin, fMax, fStep };
	return stRange;
}

inline ST_ES_RANGE_UN32 MakeRange( UInt32 un32Min, UInt32 un32Max, UInt32 un32Step )
{
	ST_ES_RANGE_UN32 stRange = { un32Min, un32Max, un32Step };
	return stRange;
}

//-------------------------------------------------------
// gamma table
//-------------------------------------------------------
const int GAMMA_TABLE_SIZE = 256;
typedef ESByte				ES_GAMMA_TABLE[GAMMA_TABLE_SIZE];

class CESGammaTable {
public:
	CESGammaTable(){
		memset( &m_table, 0, sizeof(m_table));
	}
	CESGammaTable( ES_GAMMA_TABLE& table ){
		SetGammaTable( table );
	}

	CESGammaTable& operator=( CESGammaTable& rhs ){
		SetGammaTable( rhs.GetGammaTable() );
		return *this;
	}

	ES_GAMMA_TABLE& GetGammaTable()
	{
		return m_table;
	}

	void SetGammaTable(ES_GAMMA_TABLE& table){
		memcpy_s( m_table, sizeof(m_table), table, sizeof(table));
	}
protected:
	ES_GAMMA_TABLE	m_table;
};

//-------------------------------------------------------
// color matrix
//-------------------------------------------------------
typedef float									ES_COLOR_MATRIX[3][3];

class CESColorMatrix {
public:
	CESColorMatrix(){
		memset( &m_matrix, 0, sizeof(ES_COLOR_MATRIX));
	}
	CESColorMatrix( ES_COLOR_MATRIX& matrix ){
		SetMatrix( matrix );
	}

	CESColorMatrix& operator=( CESColorMatrix& rhs ){
		SetMatrix( rhs.GetMatrix() );
		return *this;
	}

	ES_COLOR_MATRIX& GetMatrix()
	{
		return m_matrix;
	}
	void SetMatrix(ES_COLOR_MATRIX& matrix){
		for ( int i = 0; i < 3; i++ )
			for ( int j = 0; j < 3; j++ )
				m_matrix[i][j] = matrix[i][j];
	}
private:
	ES_COLOR_MATRIX m_matrix;
};

//-------------------------------------------------------
// geometry
//-------------------------------------------------------
#pragma pack(push, 1)

template< typename T > struct stESPoint {
	T	x;
	T	y;
};
typedef stESPoint< ESFloat > ST_ES_POINT_F;
typedef stESPoint< SInt32 > ST_ES_POINT_S32;
typedef stESPoint< UInt32 > ST_ES_POINT_UN32;

template< typename T > struct stESSize {
	T	cx;
	T	cy;
};
typedef stESSize< ESFloat > ST_ES_SIZE_F;
typedef stESSize< SInt32 > ST_ES_SIZE_S32;
typedef stESSize< UInt32 > ST_ES_SIZE_UN32;

template< typename T > struct stESRect {
	T	left;
	T	top;
	T	right;
	T	bottom;
};
typedef stESRect< ESFloat > ST_ES_RECT_F;
typedef stESRect< SInt32 > ST_ES_RECT_S32;
typedef stESRect< UInt32 > ST_ES_RECT_UN32;

#pragma pack(pop)

template< typename T >
stESSize< T > MakeSize( T cx, T cy)
{
	stESSize< T > size = {cx, cy};
	return size;
}

template< typename T >
stESRect< T > MakeRect( T left, T top, T right, T bottom )
{
	stESRect< T > rc = { left, top, right, bottom };
	return rc;
}

template< typename T >
stESPoint< T > MakePoint( T x, T y )
{
	stESPoint< T > po = { x, y };
	return po;
}

template< typename T >
T RECT_WIDTH( stESRect< T > rc )
{
	return rc.right - rc.left;
}

template< typename T >
T RECT_HEIGHT( stESRect< T > rc )
{
	return rc.bottom - rc.top;
}

template< typename T >
bool ESIntersectsRect ( stESRect< T > rc1, stESRect< T > rc2 )
{
	if ( (rc1.left > rc2.right) || (rc1.right < rc2.left) ){
		return false;
	}
	if ( (rc1.top > rc2.bottom) || (rc1.bottom < rc2.top) ){
		return false;
	}
	return true;
}

template< typename T >
stESRect< T > ESIntersectionRect( stESRect< T > rc1, stESRect< T > rc2 )
{
	stESRect< T > rcRet = {0};

	if ( !ESIntersectsRect( rc1, rc2 ) ){
		return rcRet;
	}

	if ( rc1.left > rc2.left){
		rcRet.left = rc1.left;
	} else {
		rcRet.left = rc2.left;
	}

	if ( rc1.right < rc2.right ){
		rcRet.right = rc1.right;
	} else {
		rcRet.right = rc2.right;
	}

	if ( rc1.top > rc2.top ){
		rcRet.top = rc1.top;
	} else {
		rcRet.top =  rc2.top;
	}

	if ( rc1.bottom < rc2.bottom ){
		rcRet.bottom = rc1.bottom;
	} else {
		rcRet.bottom = rc2.bottom;
	}
	return rcRet;
}

typedef ESDictionary ESDictLocation;

STATIC_CONST_KEY	kESLocationAreaKey		=	ES_STRING("esLocationMemberArea")			;	//ST_ES_RECT_UN32
STATIC_CONST_KEY	kESLocationActualSize	=	ES_STRING("esLocationMemberActualSize")	;	//ST_ES_SIZE_UN32
STATIC_CONST_KEY	kESLocationTangentKey	=	ES_STRING("esLocationMemberTangent")		;	//ST_ES_POINT_UN32
STATIC_CONST_KEY	kESLocationTheta		=	ES_STRING("esLocationMemberTheta")		;	//double

typedef struct tagLocation{

	ST_ES_RECT_UN32 stArea;
	ST_ES_SIZE_UN32 stActualSize;
	ST_ES_POINT_UN32 stTangent;
	double dTheta;
}ST_ES_LOCATION;



inline ESDictLocation MakeDictLocation(const ST_ES_LOCATION& stLocation){
	ESDictLocation dictLocation;

	dictLocation[kESLocationAreaKey]	=	stLocation.stArea;
	dictLocation[kESLocationActualSize]	=	stLocation.stActualSize;
	dictLocation[kESLocationTangentKey]	=	stLocation.stTangent;
	dictLocation[kESLocationTheta]		=	(float)	stLocation.dTheta;

	return dictLocation;
}

inline ST_ES_LOCATION MakeStLocation(const ESDictLocation& dictLocation){

	ST_ES_LOCATION stLocation = {0};
	if(dictLocation.count(kESLocationAreaKey))		stLocation.stArea		=	boost::any_cast<ST_ES_RECT_UN32>(dictLocation.at(kESLocationAreaKey));
	if(dictLocation.count(kESLocationActualSize))	stLocation.stActualSize	=	boost::any_cast<ST_ES_SIZE_UN32>(dictLocation.at(kESLocationActualSize));
	if(dictLocation.count(kESLocationTangentKey))	stLocation.stTangent	=	boost::any_cast<ST_ES_POINT_UN32>(dictLocation.at(kESLocationTangentKey));
	if(dictLocation.count(kESLocationTheta))		stLocation.dTheta		=	boost::any_cast<float>(dictLocation.at(kESLocationTheta));

	return stLocation;
}
