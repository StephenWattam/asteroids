#include "util.h"

void fail(char* fmt, ...){
    va_list args;
    va_start(args,fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(1);
}
