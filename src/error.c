/**
 * @file error.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Error reporting.
 */

#include <stdio.h>
#include "error.h"

void printError(int line, int ch, char *message)
{
    if (line > 0)
        fprintf(stderr, "%d:%d\tError: %s", line, ch, message);
    else
        fprintf(stderr, "Error: %s", message);
}
