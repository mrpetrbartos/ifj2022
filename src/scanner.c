/**
 * @file scanner.c
 * @author Tomáš Rajsigl (xrajsi01)
 * @brief Implementation of scanner.
 */

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

int getToken(Token *token)
{
    vStr string;
    if (!vStrInit(&string))
    {
        printError(0, 0, "Couldn't initialize string");
        return ERR_INTERNAL;
    }

    static int lineNum = 1;
    static int charNum = 0;
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
                token->type = TOKEN_PLUS;
                tokenComplete = true;
                break;
            case '-':
                token->type = TOKEN_MINUS;
                tokenComplete = true;
                break;
            case '*':
                token->type = TOKEN_MULTIPLY;
                tokenComplete = true;
                break;
            case '/':
                state = STATE_SLASH;
                break;

                // TODO: LBR, RBR, COMMA, SEMICOL, CONCAT, EOF, ASSIGN

            case '<':
                state = STATE_LESSTHAN;
                break;
            case '>':
                state = STATE_GREATERTHAN;
                break;
            case '$':
                state = STATE_VARID_PREFIX;
                break;
            default:
                if (isalpha(c) || c == '_')
                {
                    state = STATE_IDENTIFIER;
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
                    printError(token->pos.line, token->pos.character, "Unknown lexeme");
                    return ERR_LEXICAL_AN;
                }
                break;
            }
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
            // TODO
            break;
        case STATE_VARID_PREFIX:
            if (isalpha(c) || c == '_')
            {
                ungetc(c, sourceFile);
                state = STATE_VARID;
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
                token->type = TOKEN_IDENTIFIER;
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
                    token->type = TOKEN_IDENTIFIER;
                }
                tokenComplete = true;
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
            if (c == '\n')
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

    if (token->type == TOKEN_STRING || token->type == TOKEN_IDENTIFIER)
        token->value.string = string;
    else
        vStrFree(&string);

    return 0;
}
