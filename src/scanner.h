/**
 * @file scanner.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for scanner.
 */

#ifndef H_SCANNER
#define H_SCANNER

#include <stdio.h>
#include "vstr.h"

typedef enum
{
    STATE_INITIAL,
    STATE_LESSTHAN,
    STATE_GREATERTHAN,
    STATE_EQUAL_OR_ASSIGN,
    STATE_NOT_EQUAL_0,
    STATE_NOT_EQUAL_1,
    STATE_EQUAL,
    STATE_VARID_PREFIX,
    STATE_VARID,
    STATE_IDENTIFIER,
    STATE_SLASH,
    STATE_OPTIONAL,
    STATE_LINE_COM,
    STATE_MULTILINE_COM,
    STATE_POT_MULTILINE_END,
    STATE_NUMBER,
    STATE_FLOAT_0,
    STATE_FLOAT_1,
    STATE_FLOAT_E_0,
    STATE_FLOAT_E_1,
    STATE_FLOAT_E_2,
    STATE_STRING,
    STATE_STRING_ESCAPE,
    STATE_STRING_HEXA_0,
    STATE_STRING_HEXA_1,
    STATE_STRING_OCTA_0,
    STATE_STRING_OCTA_1,
    STATE_CLOSING_TAG
} State;
typedef enum
{
    KW_ELSE,
    KW_FLOAT,
    KW_FUNCTION,
    KW_IF,
    KW_INT,
    KW_NULL,
    KW_RETURN,
    KW_STRING,
    KW_VOID,
    KW_WHILE
} Keyword;

typedef enum
{
    TOKEN_EOF,
    TOKEN_EOL,
    TOKEN_EMPTY,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_OPTIONAL_TYPE,
    TOKEN_CLOSING_TAG,

    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_NULL,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,

    TOKEN_ASSIGN,

    TOKEN_CONCAT,

    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_LESS_THAN,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_THAN,
    TOKEN_GREATER_EQUAL,

    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON
} TokenType;

typedef union
{
    vStr string;
    int integer;
    Keyword keyword;
    double decimal;
} TokenValue;

typedef struct
{
    size_t line;
    size_t character;
} TokenPos;

typedef struct
{
    TokenType type;
    TokenValue value;
    TokenPos pos;
} Token;

/**
 * @brief Reads next token from the source file.
 *
 * @param token Pointer to the token variable.
 * @return int 0 if token was read successfully, non-zero value representing error.
 */
int getToken(Token *token);

/**
 * @brief Set the file with source code to be scanned.
 *
 * @param f Pointer to source file.
 */
void setSourceFile(FILE *f);

/**
 * @brief Checks whether a prologue is present;
 *
 * @return int 0 if prologue was found successfully, non-zero value otherwise.
 */
int initialScan();

#endif
