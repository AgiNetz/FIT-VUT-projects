#include "error.h"
#include "ifj17_api.h"
#include <stdio.h>
#include <string.h>

int status = 0;

void RegisterError(int errNum)
{
    if(errNum == INTERNAL_ERR || !status)
        status = errNum;
}

void error_msg(int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Error at line %d: ", line);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void internal_error_msg(const char *fmt, ...)
{
	va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}