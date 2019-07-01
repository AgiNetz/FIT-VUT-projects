#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <stdarg.h>

extern int status;

void RegisterError(int errNum);
void error_msg(int line, const char *fmt, ...);
void internal_error_msg(const char *fmt, ...);

#endif // ERROR_H_INCLUDED
