#ifndef TYPES_H
#define TYPES_H

// 基本整數類型定義
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef signed short        int16_t;
typedef signed int          int32_t;
typedef signed long long    int64_t;

// 大小類型
typedef unsigned long       size_t;
typedef long                ssize_t;

// 布林類型
typedef enum {
    false = 0,
    true = 1
} bool;

// NULL 定義
#define NULL ((void*)0)

#endif