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
#define REDUCED 502

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

/**
 * @brief Parses expression following current token.
 *
 * @return int Zero if expression was parsed successfully, non-zero otherwise.
 */
int parseExpression();

#endif
