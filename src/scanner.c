/**
 * @file scanner.c
 * @author Tomáš Rajsigl (xrajsi01)
 * @brief Implementation of scanner.
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "error.h"

FILE *sourceFile;

void setSourceFile(FILE *f)
{
    sourceFile = f;
}

void checkKeyword(Token *token, vStr *string)
{
    if (strcmp(string->content, "else") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_ELSE;
    }
    else if (strcmp(string->content, "float") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_FLOAT;
    }
    else if (strcmp(string->content, "function") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_FUNCTION;
    }
    else if (strcmp(string->content, "if") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_IF;
    }
    else if (strcmp(string->content, "int") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_INT;
    }
    else if (strcmp(string->content, "null") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_NULL;
    }
    else if (strcmp(string->content, "return") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_RETURN;
    }
    else if (strcmp(string->content, "string") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_STRING;
    }
    else if (strcmp(string->content, "void") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_VOID;
    }
    else if (strcmp(string->content, "while") == 0)
    {
        token->type = TOKEN_KEYWORD;
        token->value.keyword = KW_WHILE;
    }
}

int initialScan()
{
    vStr string;
    int c;

    if (!vStrInit(&string))
    {
        printError(0, 0, "Couldn't initialize string");
        return ERR_INTERNAL;
    }

    while (!isspace(c = getc(sourceFile)))
    {
        vStrAppend(&string, c);
        if (c == EOF)
            break;
    }
    if (strcmp((&string)->content, "<?php") || !isspace(c))
    {
        vStrFree(&string);
        printError(0, 0, "The program has to start with a prologue, missing opening tag");
        return ERR_LEXICAL_AN;
    }

    vStrFree(&string);

    return 0;
}

int getToken(Token *token, bool peek)
{
    vStr string;
    if (!vStrInit(&string))
    {
        printError(0, 0, "Couldn't initialize string");
        return ERR_INTERNAL;
    }

    static Token peekedToken = {.type = TOKEN_EMPTY};

    if (peekedToken.type != TOKEN_EMPTY)
    {
        *token = peekedToken;
        if (!peek)
            peekedToken.type = TOKEN_EMPTY;
        return 0;
    }

    static int lineNum = 1;
    static int charNum = 0;
    char hexa[3] = {0};
    char octa[4] = {0};
    bool tokenComplete = false;

    token->type = TOKEN_EMPTY;
    token->pos.line = lineNum;
    token->pos.character = charNum;

    State state = STATE_INITIAL;

    int c;
    while (!tokenComplete)
    {
        c = getc(sourceFile);
        charNum++;
        switch (state)
        {
        case STATE_INITIAL:
            switch (c)
            {
            case '+':
                state = STATE_PLUS;
                break;
            case '-':
                state = STATE_MINUS;
                break;
            case '*':
                state = STATE_MULTIPLY;
                break;
            case '/':
                state = STATE_SLASH;
                break;
            case '(':
                state = STATE_LEFT_BRACKET;
                break;
            case ')':
                state = STATE_RIGHT_BRACKET;
                break;
            case '{':
                state = STATE_LEFT_BRACE;
                break;
            case '}':
                state = STATE_RIGHT_BRACE;
                break;
            case ',':
                state = STATE_COMMA;
                break;
            case ':':
                state = STATE_COLON;
                break;
            case ';':
                state = STATE_SEMICOLON;
                break;
            case '.':
                state = STATE_CONCATENATE;
                break;
            case EOF:
                state = STATE_EOF;
                break;
            case '?':
                state = STATE_OPTIONAL;
                break;
            case '=':
                state = STATE_EQUAL_OR_ASSIGN;
                break;
            case '!':
                state = STATE_NOT_EQUAL_0;
                break;
            case '<':
                state = STATE_LESSTHAN;
                break;
            case '>':
                state = STATE_GREATERTHAN;
                break;
            case '$':
                state = STATE_VARID_PREFIX;
                break;
            case '\"':
                state = STATE_STRING_0;
                break;
            default:
                if (isalpha(c) || c == '_')
                {
                    state = STATE_IDENTIFIER;
                    ungetc(c, sourceFile);
                }
                else if (isdigit(c))
                {
                    state = STATE_NUMBER;
                    ungetc(c, sourceFile);
                }
                else if (isspace(c))
                {
                    if (c == '\n')
                    {
                        lineNum++;
                        charNum = 0;
                    }
                }
                else
                {
                    vStrFree(&string);
                    printError(token->pos.line, token->pos.character, "Unknown token found");
                    return ERR_LEXICAL_AN;
                }
                break;
            }
            break;
        case STATE_PLUS:
            token->type = TOKEN_PLUS;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_MINUS:
            token->type = TOKEN_MINUS;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_MULTIPLY:
            token->type = TOKEN_MULTIPLY;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_CONCATENATE:
            token->type = TOKEN_CONCAT;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_LEFT_BRACKET:
            token->type = TOKEN_LEFT_BRACKET;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_RIGHT_BRACKET:
            token->type = TOKEN_RIGHT_BRACKET;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_LEFT_BRACE:
            token->type = TOKEN_LEFT_BRACE;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_RIGHT_BRACE:
            token->type = TOKEN_RIGHT_BRACE;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_EOF:
            token->type = TOKEN_EOF;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_COMMA:
            token->type = TOKEN_COMMA;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_COLON:
            token->type = TOKEN_COLON;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_SEMICOLON:
            token->type = TOKEN_SEMICOLON;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_LESSTHAN:
            if (c == '=')
                token->type = TOKEN_LESS_EQUAL;
            else
            {
                token->type = TOKEN_LESS_THAN;
                ungetc(c, sourceFile);
            }
            tokenComplete = true;
            break;
        case STATE_GREATERTHAN:
            if (c == '=')
                token->type = TOKEN_GREATER_EQUAL;
            else
            {
                token->type = TOKEN_GREATER_THAN;
                ungetc(c, sourceFile);
            }
            tokenComplete = true;
            break;
        case STATE_OPTIONAL:
            if (c == '>')
                state = STATE_CLOSING_TAG_0;
            else
            {
                token->type = TOKEN_OPTIONAL_TYPE;
                tokenComplete = true;
                ungetc(c, sourceFile);
            }
            break;
        case STATE_CLOSING_TAG_0:
            if (c == EOF)
            {
                state = STATE_CLOSING_TAG_2;
            }
            else if (c == '\n')
                state = STATE_CLOSING_TAG_1;
            else
            {
                vStrFree(&string);
                printError(0, 0, "Expected EOF after closing tag");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_CLOSING_TAG_1:
            if (c == EOF)
            {
                state = STATE_CLOSING_TAG_2;
            }
            else
            {
                vStrFree(&string);
                printError(0, 0, "Expected EOF after closing tag");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_CLOSING_TAG_2:
            token->type = TOKEN_CLOSING_TAG;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_NOT_EQUAL_0:
            if (c == '=')
                state = STATE_NOT_EQUAL_1;
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_NOT_EQUAL_1:
            if (c == '=')
            {
                state = STATE_NOT_EQUAL_2;
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found, did you mean to use '!=='?");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_NOT_EQUAL_2:
            token->type = TOKEN_NOT_EQUAL;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_EQUAL_OR_ASSIGN:
            if (c != '=')
            {
                token->type = TOKEN_ASSIGN;
                tokenComplete = true;
                ungetc(c, sourceFile);
            }
            else
            {
                state = STATE_EQUAL_0;
            }
            break;
        case STATE_EQUAL_0:
            if (c == '=')
            {
                state = STATE_EQUAL_1;
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found, did you mean to use '==='?");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_EQUAL_1:
            token->type = TOKEN_EQUAL;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_VARID_PREFIX:
            if (isalpha(c) || c == '_')
            {
                state = STATE_VARID;
                ungetc(c, sourceFile);
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Variable identifier can only start with either a letter or an underscore");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_VARID:
            if (isalpha(c) || isdigit(c) || c == '_')
                vStrAppend(&string, c);
            else
            {
                token->type = TOKEN_IDENTIFIER_VAR;
                tokenComplete = true;
                ungetc(c, sourceFile);
            }
            break;
        case STATE_IDENTIFIER:
            if (isalpha(c) || isdigit(c) || c == '_')
                vStrAppend(&string, c);
            else
            {
                ungetc(c, sourceFile);
                checkKeyword(token, &string);
                if (token->type != TOKEN_KEYWORD)
                {
                    token->type = TOKEN_IDENTIFIER_FUNC;
                }
                tokenComplete = true;
            }
            break;
        case STATE_NUMBER:
            if (isdigit(c))
            {
                vStrAppend(&string, c);
            }
            else if (c == '.')
            {
                vStrAppend(&string, c);
                state = STATE_FLOAT_0;
            }
            else if (c == 'e' || c == 'E')
            {
                vStrAppend(&string, c);
                state = STATE_FLOAT_E_0;
            }
            else
            {
                char *endPtr = NULL;
                ungetc(c, sourceFile);
                unsigned long value = strtoul((&string)->content, &endPtr, 10);
                if (*endPtr != '\0')
                {
                    vStrFree(&string);
                    printError(0, 0, "Integer conversion has failed");
                    return ERR_INTERNAL;
                }
                token->type = TOKEN_INT;
                token->value.integer = value;
                tokenComplete = true;
            }
            break;
        case STATE_FLOAT_0:
            if (isdigit(c))
            {
                state = STATE_FLOAT_1;
                ungetc(c, sourceFile);
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found, dot must be followed by a digit");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_FLOAT_1:
            if (isdigit(c))
            {
                vStrAppend(&string, c);
            }
            else if (c == 'e' || c == 'E')
            {
                state = STATE_FLOAT_E_0;
                vStrAppend(&string, c);
            }
            else
            {
                char *endPtr = NULL;
                ungetc(c, sourceFile);
                double value = strtod((&string)->content, &endPtr);
                if (*endPtr != '\0')
                {
                    vStrFree(&string);
                    printError(0, 0, "Float conversion has failed");
                    return ERR_INTERNAL;
                }
                token->type = TOKEN_FLOAT;
                token->value.decimal = value;
                tokenComplete = true;
            }
            break;
        case STATE_FLOAT_E_0:
            if (c == '+' || c == '-')
            {
                vStrAppend(&string, c);
                state = STATE_FLOAT_E_1;
            }
            else if (isdigit(c))
            {
                state = STATE_FLOAT_E_1;
                ungetc(c, sourceFile);
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found, exponent must be followed by a (signed) digit");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_FLOAT_E_1:
            if (isdigit(c))
            {
                vStrAppend(&string, c);
                state = STATE_FLOAT_E_2;
            }
            else
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unknown token found, exponent must be followed by a (signed) digit");
                return ERR_LEXICAL_AN;
            }
            break;
        case STATE_FLOAT_E_2:
            if (isdigit(c))
            {
                vStrAppend(&string, c);
            }
            else
            {
                char *endPtr = NULL;
                ungetc(c, sourceFile);
                double value = strtod((&string)->content, &endPtr);
                if (*endPtr != '\0')
                {
                    vStrFree(&string);
                    printError(0, 0, "Float conversion has failed");
                    return ERR_INTERNAL;
                }
                token->type = TOKEN_FLOAT;
                token->value.decimal = value;
                tokenComplete = true;
            }
            break;
        case STATE_STRING_0:
            if (c == '\n')
            {
                lineNum++;
                charNum = 0;
            }
            else if (c < 32 || c == '$')
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "String contains unsupported character(s)");
                return ERR_LEXICAL_AN;
            }
            else if (c == '\"')
            {
                state = STATE_STRING_1;
            }
            else if (c == '\\')
            {
                state = STATE_STRING_ESCAPE;
            }
            else
            {
                vStrAppend(&string, c);
            }
            break;
        case STATE_STRING_1:
            token->type = TOKEN_STRING;
            tokenComplete = true;
            ungetc(c, sourceFile);
            break;
        case STATE_STRING_ESCAPE:
            switch (c)
            {
            case '\\':
                state = STATE_STRING_0;
                vStrAppend(&string, '\\');
                break;
            case '\"':
                state = STATE_STRING_0;
                vStrAppend(&string, '\"');
                break;
            case 'n':
                state = STATE_STRING_0;
                vStrAppend(&string, '\n');
                break;
            case 't':
                state = STATE_STRING_0;
                vStrAppend(&string, '\t');
                break;
            case '$':
                state = STATE_STRING_0;
                vStrAppend(&string, c);
                break;
            case 'x':
                state = STATE_STRING_HEXA_0;
                break;
            default:
                if (c >= '0' && c <= '3')
                {
                    octa[0] = c;
                    state = STATE_STRING_OCTA_0;
                }
                else
                {
                    vStrAppend(&string, '\\');
                    ungetc(c, sourceFile);
                    state = STATE_STRING_0;
                }
            }
            break;
        case STATE_STRING_HEXA_0:
            if (isxdigit(c))
            {
                hexa[0] = c;
                state = STATE_STRING_HEXA_1;
            }
            else
            {
                vStrAppend(&string, '\\');
                vStrAppend(&string, 'x');
                ungetc(c, sourceFile);
                state = STATE_STRING_0;
            }
            break;
        case STATE_STRING_HEXA_1:
            if (!isxdigit(c) || (hexa[0] == '0' && c == '0'))
            {
                vStrAppend(&string, '\\');
                vStrAppend(&string, 'x');
                vStrAppend(&string, hexa[0]);
                ungetc(c, sourceFile);
                state = STATE_STRING_0;
            }
            else
            {
                state = STATE_STRING_0;
                hexa[1] = c;
                hexa[2] = '\0';
                char *endPtr = NULL;
                long value = strtol(hexa, &endPtr, 16);
                if (*endPtr != '\0')
                {
                    vStrFree(&string);
                    printError(0, 0, "Hexadecimal conversion has failed");
                    return ERR_INTERNAL;
                }
                vStrAppend(&string, value);
            }
            break;
        case STATE_STRING_OCTA_0:
            if (c >= '0' && c <= '7')
            {
                octa[1] = c;
                state = STATE_STRING_OCTA_1;
            }
            else
            {
                vStrAppend(&string, '\\');
                vStrAppend(&string, octa[0]);
                ungetc(c, sourceFile);
                state = STATE_STRING_0;
            }
            break;
        case STATE_STRING_OCTA_1:
            if ((octa[0] == '0' && octa[1] == '0' && c == '0') || (c > '7'))
            {
                vStrAppend(&string, '\\');
                vStrAppend(&string, octa[0]);
                vStrAppend(&string, octa[1]);
                ungetc(c, sourceFile);
                state = STATE_STRING_0;
            }
            else if (c >= '0' && c <= '7')
            {
                state = STATE_STRING_0;
                octa[2] = c;
                octa[3] = '\0';
                char *endPtr = NULL;
                long value = strtol(octa, &endPtr, 8);
                if (*endPtr != '\0')
                {
                    vStrFree(&string);
                    printError(0, 0, "Octal conversion has failed");
                    return ERR_INTERNAL;
                }
                vStrAppend(&string, value);
            }
            break;
        case STATE_SLASH:
            if (c == '/')
                state = STATE_LINE_COM;
            else if (c == '*')
                state = STATE_MULTILINE_COM;
            else
            {
                token->type = TOKEN_DIVIDE;
                tokenComplete = true;
                ungetc(c, sourceFile);
            }
            break;
        case STATE_LINE_COM:
            if (c == '\n' || c == EOF)
            {
                lineNum++;
                charNum = 0;
                state = STATE_INITIAL;
            }
            break;
        case STATE_MULTILINE_COM:
            if (c == '\n')
            {
                lineNum++;
                charNum = 0;
            }
            else if (c == EOF)
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unterminated block comment");
                return ERR_LEXICAL_AN;
            }
            else if (c == '*')
                state = STATE_POT_MULTILINE_END;
            break;
        case STATE_POT_MULTILINE_END:
            if (c == '/')
                state = STATE_INITIAL;
            else if (c == EOF)
            {
                vStrFree(&string);
                printError(token->pos.line, token->pos.character, "Unterminated block comment");
                return ERR_LEXICAL_AN;
            }
            else
                state = STATE_MULTILINE_COM;
        }
    }

    if (token->type == TOKEN_STRING || token->type == TOKEN_IDENTIFIER_VAR || token->type == TOKEN_IDENTIFIER_FUNC)
        token->value.string = string;
    else
        vStrFree(&string);

    if (peek)
        peekedToken = *token;

    return 0;
}
