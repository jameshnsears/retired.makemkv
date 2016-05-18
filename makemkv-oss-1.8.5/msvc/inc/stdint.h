/*
    Written by GuinpinSoft inc <oss@makemkv.com>

    This file is hereby placed into public domain,
    no copyright is claimed.

*/
#ifndef MSVC_STDDEF_H_INCLUDED
#define MSVC_STDDEF_H_INCLUDED

#if defined(_MSC_VER)

typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

typedef signed __int8       int8_t;
typedef signed __int16      int16_t;
typedef signed __int32      int32_t;
typedef signed __int64      int64_t;

#define	_i__CONCAT1(x,y)	x ## y
#define	_i__CONCAT(x,y)	_i__CONCAT1(x,y)

#define INT8_C(value)   ((int8_t) value)
#define UINT8_C(value) ((uint8_t) _i__CONCAT(value, U))
#define INT16_C(value) value
#define UINT16_C(value) _i__CONCAT(value, U)
#define INT32_C(value) _i__CONCAT(value, L)
#define UINT32_C(value) _i__CONCAT(value, UL)
#define INT64_C(value) _i__CONCAT(value, LL)
#define UINT64_C(value) _i__CONCAT(value, ULL)
#define INTMAX_C(value) _i__CONCAT(value, LL)
#define UINTMAX_C(value) _i__CONCAT(value, ULL)

#endif

#endif // MSVC_STDDEF_H_INCLUDED
