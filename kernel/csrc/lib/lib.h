#pragma once
/*
 * my lib for project
 */
#include <define/types.h>


void assert_impl(bool cond, const char * condStr,const char * file, u64 line, const char * funcInfo, const char * info);
#define assert(cond, info) assert_impl((cond), #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__, (info))
void panic(const char *);
void memset(void * p, u8 byte, size_t size);