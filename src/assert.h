#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ASSERTS
#ifndef ASSERTS
#  define assert(condition, exception_str) ((void)0)
#else
#  define assert(condition, exception_str) ASSERT_IMPL(condition, exception_str, __LINE__, __FILE__)
#endif

void ASSERT_IMPL(const bool condition, const char* exception_on_hit, int line, const char* file){
    if (condition) return;
    printf("\n\nAssertion failed! \n\tAt %s:%d\n\tReason: %s", file, line, exception_on_hit);
    exit(1);
}