/**
 * @file parser.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for parser module.
 */

#ifndef H_PARSER
#define H_PARSER

#include "scanner.h"
#include "structures.h"
#include "error.h"
#include <stdlib.h>
#include "expression.h"
#include "symtable.h"
#include "codegen.h"

typedef struct
{
    Token currToken;
    bool outsideBody;
    char *currFunc;
    bool condDec; // Indicates conditional declaration
    Stack *stack;
    Stack *undefStack;
    Symtable *symtable;
    Symtable *localSymtable;
} Parser;

#define LINENUM parser.currToken.pos.line
#define CHARNUM parser.currToken.pos.character
#define GETTOKEN(t)              \
    if (getToken(t, false) != 0) \
        return ERR_LEXICAL_AN;
#define CHECKSEMICOLON()                                                      \
    if (parser.currToken.type != TOKEN_SEMICOLON)                             \
    {                                                                         \
        printError(LINENUM, CHARNUM, "Missing semicolon after a statement."); \
        return ERR_SYNTAX_AN;                                                 \
    }
#define CHECKRULE(r)    \
    do                  \
    {                   \
        err = (r);      \
        if (err != 0)   \
            return err; \
    } while (0);
#define BEGINNINGOFEX()                                    \
    do                                                     \
    {                                                      \
        switch (parser.currToken.type)                     \
        {                                                  \
        case TOKEN_LEFT_BRACKET:                           \
        case TOKEN_STRING:                                 \
        case TOKEN_INT:                                    \
        case TOKEN_FLOAT:                                  \
        case TOKEN_IDENTIFIER_VAR:                         \
            expr = true;                                   \
            break;                                         \
        case TOKEN_KEYWORD:                                \
            if (parser.currToken.value.keyword == KW_NULL) \
                expr = true;                               \
            else                                           \
                expr = false;                              \
            break;                                         \
        default:                                           \
            expr = false;                                  \
            break;                                         \
        }                                                  \
    } while (0);

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

/**
 * @brief Checks the "declare(strict_types=1);" part of prologue.
 *
 * @return int Non-zero number if prologue isn't in the specified format, zero otherwise.
 */
int checkPrologue();

#endif
