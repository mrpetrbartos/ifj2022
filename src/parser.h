/**
 * @file parser.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for parser module.
 */

#ifndef H_PARSER
#define H_PARSER

#include "scanner.h"
#include "stack.h"
#include "error.h"
#include "stdlib.h"
#include "expression.h"
#include "symtable.h"

typedef struct
{
    Token currToken;
    bool outsideBody;
    Stack *stack;
    Symtable *symtable;
} Parser;

/**
 * @brief Initializes all structures, that need to by malloced.
 *
 * @return int Non-zero number if any of the mallocs fail, zero otherwise.
 */
int parserInit();

/**
 * @brief Frees all malloced structures used in parser global file.
 *
 */
void parserDestroy();

/**
 * @brief Initalizes parsing.
 *
 * @return int Zero if code was parsed without an error, non-zero otherwise.
 */
int parse();

#endif
