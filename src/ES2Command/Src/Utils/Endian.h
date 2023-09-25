////////////////////////////////////////////////////////////////////////////////////////////////////
//!
//! @file     Endian.h
//!
//! @brif     エンディアン変更関数の定義
//! @note    
//! @versoin  1.0
//! @par      Copyright SEIKO EPSON Corporation
//! @par      更新履歴
//! @par        - 新規作成       2014/12/25
//!
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define __BIG_ENDIAN__ 0
#define __LITTLE_ENDIAN__ 1


inline UInt16 CFSwapInt16(UInt16 in)
{
  union swap
    {
      UInt16 num;
      UInt8  byt[2];
    } dst;
  union swap *src = (union swap*)&in;
  dst.byt[0] = src->byt[1];
  dst.byt[1] = src->byt[0];
  return dst.num;
}

inline UInt32 CFSwapInt32(UInt32 in)
{
  union swap
    {
      UInt32 num;
      UInt8  byt[4];
    } dst;
  union swap *src = (union swap*)&in;
  dst.byt[0] = src->byt[3];
  dst.byt[1] = src->byt[2];
  dst.byt[2] = src->byt[1];
  dst.byt[3] = src->byt[0];
  return dst.num;
}

inline UInt64 CFSwapInt64(UInt64 in)
{
  union swap
    {
      UInt64 num;
      UInt8  byt[8];
    } dst;
  union swap *src = (union swap*)&in;
  dst.byt[0] = src->byt[7];
  dst.byt[1] = src->byt[6];
  dst.byt[2] = src->byt[5];
  dst.byt[3] = src->byt[4];
  dst.byt[4] = src->byt[3];
  dst.byt[5] = src->byt[2];
  dst.byt[6] = src->byt[1];
  dst.byt[7] = src->byt[0];
  return dst.num;
}


#if	__BIG_ENDIAN__

inline UInt16 CFSwapInt16BigToHost(UInt16 in)
{
  return in;
}

inline UInt16 CFSwapInt16HostToBig(UInt16 in)
{
  return in;
}

inline UInt16 CFSwapInt16HostToLittle(UInt16 in)
{
  return CFSwapInt16(in);
}

inline UInt16 CFSwapInt16LittleToHost(UInt16 in)
{
  return CFSwapInt16(in);
}

inline UInt32 CFSwapInt32BigToHost(UInt32 in)
{
  return in;
}

inline UInt32 CFSwapInt32HostToBig(UInt32 in)
{
  return in;
}

inline UInt32 CFSwapInt32HostToLittle(UInt32 in)
{
  return CFSwapInt32(in);
}

inline UInt32 CFSwapInt32LittleToHost(UInt32 in)
{
  return CFSwapInt32(in);
}

inline UInt64 CFSwapInt64BigToHost(UInt64 in)
{
  return in;
}

inline UInt64 CFSwapInt64HostToBig(UInt64 in)
{
  return in;
}

inline UInt64 CFSwapInt64HostToLittle(UInt64 in)
{
  return CFSwapInt64(in);
}

inline UInt64 CFSwapInt64LittleToHost(UInt64 in)
{
  return CFSwapInt64(in);
}

#else

inline UInt16 CFSwapInt16BigToHost(UInt16 in)
{
  return CFSwapInt16(in);
}

inline UInt16 CFSwapInt16HostToBig(UInt16 in)
{
  return CFSwapInt16(in);
}

inline UInt16 CFSwapInt16HostToLittle(UInt16 in)
{
  return in;
}

inline UInt16 CFSwapInt16LittleToHost(UInt16 in)
{
  return in;
}

inline UInt32 CFSwapInt32BigToHost(UInt32 in)
{
  return CFSwapInt32(in);
}

inline UInt32 CFSwapInt32HostToBig(UInt32 in)
{
  return CFSwapInt32(in);
}

inline UInt32 CFSwapInt32HostToLittle(UInt32 in)
{
  return in;
}

inline UInt32 CFSwapInt32LittleToHost(UInt32 in)
{
  return in;
}

inline UInt64 CFSwapInt64BigToHost(UInt64 in)
{
  return CFSwapInt64(in);
}

inline UInt64 CFSwapInt64HostToBig(UInt64 in)
{
  return CFSwapInt64(in);
}

inline UInt64 CFSwapInt64HostToLittle(UInt64 in)
{
  return in;
}

inline UInt64 CFSwapInt64LittleToHost(UInt64 in)
{
  return in;
}


#endif

