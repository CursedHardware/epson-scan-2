#pragma once

#include "../ESCANLIB.h"

void SetErrorCode(DWORD* pErrCode, DWORD ErrCode);

//--- calculate 4bytes alignment ---//
template <class T> T BITS_TO_BYTES(T bits)
{
	return (((bits) + 7) / 8);
}

//--- calculate 4bytes alignment ---//
template <class T> T BITS_TO_BYTES_DWORD_ALIGNED(T bits)
{
	return (ROUND_UP(bits, 32) / 8);
//	return (((bits) + 31) / 32 * 32 / 8);
}

//--- calculate 4bytes alignment ---//
template <class T> T BYTES_TO_DWORD_ALIGNED(T bytes)
{
	return ROUND_UP(bytes, 4);
//	return (((bytes) + 3) / 4 * 4);
}

template <class T> T ROUND_UP(T value, int unit)
{
	return ((value + unit - 1) / unit * unit);
}

template <class T> T ROUND_DOWN(T value, int unit)
{
	return (value / unit * unit);
}