#include "debug.h"
#include "iobase.h"

#include <fstream>
#include <cstdarg>


void _debug(const char *file, int line, const char *format, ...)
{
    fprintf(stderr, ANSI_BLUE "%s" ANSI_RESET ":" ANSI_RED "%d" ANSI_RESET ": " , file, line);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}
