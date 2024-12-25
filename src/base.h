#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef uintptr_t usize;
typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;
typedef intptr_t  isize;
typedef int64_t   i64;
typedef int32_t   i32;
typedef int16_t   i16;
typedef int8_t    i8;

typedef uint8_t bool;
#define false ((bool)0)
#define true ((bool)1)

#define unreachable __builtin_unreachable()

#endif