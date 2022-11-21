/**
 * @file expression.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for custom expression parsing structures.
 */

#ifndef H_EXPRESSION
#define H_EXPRESSION

#include "symtable.h"
#include "parser.h"
#include "scanner.h"
#include "error.h"

#define DOLLAR 500
#define SHIFT_SYMBOL 501
#define REDUCED 501

typedef enum
{
    R, // reduce >
    S, // shift <
    E, // equal
    F, // error
    O  // ok
} precValues;

typedef enum
{
    I_PLUS,
    I_MULTIPLY,
    I_DATA,
    I_DOLLAR,
    I_RELATIONAL,
    I_COMPARISON,
    I_OPENB,
    I_CLOSEB
} tableIndex;

int parseExpression();

#endif
