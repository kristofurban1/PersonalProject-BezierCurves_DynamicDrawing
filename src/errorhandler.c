#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "errorhandler.h"


extern void Info(const char *__format, ...) {
    va_list args;
    va_start(args, __format);
    printf("INFO::");
    vprintf(__format, args);
    va_end(args);
}
extern void Warning(const char *__format, ...) {
    va_list args;
    va_start(args, __format);
    printf("WARNING::");
    vprintf(__format, args);
    va_end(args);
}
extern void Error(const char *error_message){ printf("ERROR:: %s\n", error_message); }
extern void PANIC(int error_code, const char* error_message){ printf("ERROR:%d: %s\n", error_code, error_message); exit(error_code); }