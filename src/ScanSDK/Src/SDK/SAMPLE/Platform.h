#pragma once
//-------------------------------------------------------
// function replacement
//-------------------------------------------------------
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef WIN32
#include <Windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <dlfcn.h>
#endif

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#ifndef WIN32
#define sprintf_s snprintf
#define CALLBACK

#define __stdcall
#define _cdecl

typedef unsigned char BYTE;
typedef BYTE* LPBYTE;

typedef int BOOL;
typedef BOOL* LPBOOL;

typedef const char* LPCSTR;
typedef char* LPSTR;

typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HANDLE;
typedef HANDLE HGLOBAL;
typedef void* FARPROC;
typedef void* PVOID;
typedef HANDLE HPROFILE;        // handle to profile object

typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;

typedef long LONG;
typedef unsigned long ULONG;
typedef int64_t LONG_PTR;
typedef uint64_t ULONG_PTR;

typedef uint8_t	UINT8;
typedef uint32_t UINT32;

typedef uint8_t	UInt8;
typedef uint32_t UInt32;

typedef long HRESULT;
#define S_OK            ((HRESULT)0L)

typedef int errno_t;
typedef uint32_t DWORD;
typedef DWORD *LPDWORD;
typedef unsigned short WORD;

typedef unsigned int UINT32, *PUINT32;
typedef signed long long  INT64;

typedef ULONG_PTR SIZE_T;

typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
typedef char TCHAR;

typedef unsigned long DWORD_PTR;

// macros
#define SUCCEEDED(hr)   (((HRESULT)(hr)) >= 0)
#define FAILED(hr)      (((HRESULT)(hr)) < 0)

#define UNREFERENCED_PARAMETER(P)	(P)

errno_t fopen_s(
				FILE** pFile,
				const char *filename,
				const char *mode);

errno_t _wfopen_s(
					FILE** pFile,
					const wchar_t *filename,
					const wchar_t *mode);

#define _tfopen_s fopen_s

int _wtoi( const wchar_t* str);
float _wtof( const wchar_t* str);

#define _countof(a) (sizeof(a)/sizeof(*(a)))
#define _T(s) s

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

LONG
InterlockedIncrement (
	LONG volatile *Addend
	);

LONG
InterlockedDecrement (
  LONG volatile *Addend
  );

#endif //WIN32

#ifdef WIN32
#define SEPARATOR ES_STRING('\\')
#else
#define SEPARATOR ES_STRING('/')
#endif

#ifdef WIN32
#define esslen(str1) wcslen(str1)
#else
#define esslen(str1) strlen(str1)
#endif

#ifdef WIN32
#define esscmp(str1, str2) wcscmp(str1, str2)
#else
#define esscmp(str1, str2) strcmp(str1, str2)
#endif

#ifndef WIN32
#define _T(s) s
#endif

#ifndef WIN32
#ifndef WINAPI
#define WINAPI
#endif
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef _ASSERT
#define _ASSERT assert
#endif

#ifndef _MAX_PATH
#define _MAX_PATH   260 /* max. length of full pathname */
#endif

#ifndef MAX_PATH
#define MAX_PATH	260
#endif

#ifndef _MAX_DRIVE
#define _MAX_DRIVE  3   /* max. length of drive component */
#endif

#ifndef _MAX_DIR
#define _MAX_DIR    256 /* max. length of path component */
#endif

#ifndef LOWORD
#define LOWORD(_dw)     ((WORD)(((DWORD_PTR)(_dw)) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(_dw)     ((WORD)((((DWORD_PTR)(_dw)) >> 16) & 0xffff))
#endif

#ifndef WIN32
#define	GLOBAL_HANDLE_ID		0x00000000
#define	THREAD_HANDLE_ID		0x00000001
#define	EVENT_HANDLE_ID			0x00000002
#define	FILE_HANDLE_ID			0x00000003
#define	STILL_ACTIVE				259

/*  Library define */
#define LOAD_WITH_ALTERED_SEARCH_PATH       0x00000008

/* Thread define */
#define STATUS_WAIT_0			((DWORD   )0x00000000L)
#define INFINITE				0xFFFFFFFF  // Infinite timeout
#define WAIT_FAILED				((DWORD)0xFFFFFFFF)
#define WAIT_OBJECT_0			((STATUS_WAIT_0 ) + 0 )
#define MAXIMUM_WAIT_OBJECTS	64     // Maximum number of wait objects
#endif

