//
// inttypes
//
// Copyright 2007 Dean Ferreyra, All rights reserved
//
// This file is part of dspic-helper.
//
// dspic-helper is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// dspic-helper is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with dspic-helper.  If not, see <http://www.gnu.org/licenses/>.
//
// $Id: inttypes.h,v 1.1 2008-11-12 22:39:59 dean Exp $

#ifndef INTTYPES_H_INCL
#define INTTYPES_H_INCL 1

#include <limits.h>

typedef int __attribute__((__mode__(QI))) int8_t;
typedef int __attribute__((__mode__(HI))) int16_t;
typedef int __attribute__((__mode__(SI))) int32_t;
typedef int __attribute__((__mode__(DI))) int64_t;

typedef unsigned int __attribute__((__mode__(QI))) uint8_t;
typedef unsigned int __attribute__((__mode__(HI))) uint16_t;
typedef unsigned int __attribute__((__mode__(SI))) uint32_t;
typedef unsigned int __attribute__((__mode__(DI))) uint64_t;

/*
#define UINT8_MAX  UCHAR_MAX
#define INT8_MAX   SCHAR_MAX
#define INT8_MIN   SCHAR_MIN
#define UINT16_MAX USHRT_MAX
#define INT16_MAX  SHRT_MAX
#define INT16_MIN  SHRT_MIN
#define UINT32_MAX ULONG_MAX
#define INT32_MAX  LONG_MAX
#define INT32_MIN  LONG_MIN
#define UINT64_MAX ULLONG_MAX
#define INT64_MAX  LLONG_MAX
#define INT64_MIN  LLONG_MIN
 */

#endif // #ifndef INTTYPES_H_INCL
