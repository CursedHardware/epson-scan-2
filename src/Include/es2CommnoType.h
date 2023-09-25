#include <stdint.h>

//-------------------------------------------------------
// basic native type
//-------------------------------------------------------
typedef uint8_t				UInt8;		// 1 byte
typedef uint16_t			UInt16;
typedef uint32_t			UInt32;
typedef uint64_t			UInt64;

typedef int8_t				SInt8;
typedef int16_t				SInt16;
typedef int32_t				SInt32;
typedef int64_t				SInt64;

typedef UInt8			  	ESByte;
typedef ESByte*				PESByte;

typedef SInt32				ESNumber;
typedef UInt32				ESUnsigned;
typedef float				  ESFloat;

//-------------------------------------------------------
// string
//-------------------------------------------------------
typedef char				ES_CHAR_A;
typedef ES_CHAR_A*			ES_CHAR_A_PTR;
typedef const ES_CHAR_A*		ES_CHAR_A_CPTR;

typedef wchar_t				ES_CHAR_W;
typedef ES_CHAR_W*			ES_CHAR_W_PTR;
typedef const ES_CHAR_W*		ES_CHAR_W_CPTR;

#ifdef WIN32
typedef ES_CHAR_W				ES_CHAR;
#else
typedef ES_CHAR_A				ES_CHAR;
#endif
typedef ES_CHAR*				ES_CHAR_PTR;
typedef const ES_CHAR*		ES_CHAR_CPTR;


#ifndef ISDITYPEDEF_HEADER
#define	ISDITYPEDEF_HEADER

#ifdef WIN32
#ifndef STATIC_CONST_KEY
	#define	STATIC_CONST_KEY		static LPCWSTR
#endif
#ifndef ES_STRING
	#define ES_STRING(str)			L ## str
#endif
#ifndef MAC_ATTR_UNAVAILABLE
	#define MAC_ATTR_UNAVAILABLE
#endif
#else
#ifndef STATIC_CONST_KEY
	#define	STATIC_CONST_KEY		static const char* const
#endif
#ifndef ES_STRING
	#define ES_STRING(str)		 str
#endif
#endif

#endif // !ISDITYPEDEF_HEADER
