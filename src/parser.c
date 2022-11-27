/**
 * @file parser.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Recursive descent parser.
 */

#include "parser.h"

Parser parser;

int parserInit()
{
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL)
        return ERR_INTERNAL;
    stackInit(s);
    Symtable *st = symtableInit(1024);
    if (st == NULL)
        return ERR_INTERNAL;
    parser.stack = s;
    parser.outsideBody = false;
    parser.symtable = st;
    return 0;
}

void parserDestroy()
{
    symtableFree(parser.symtable);
    stackFree(parser.stack);
    free(parser.stack);
}

int checkPrologue()
{
    Token prologue[7] = {{.type = TOKEN_IDENTIFIER_FUNC, .value.string.content = "declare"},
                         {.type = TOKEN_LEFT_BRACKET},
                         {.type = TOKEN_IDENTIFIER_FUNC, .value.string.content = "strict_types"},
                         {.type = TOKEN_ASSIGN},
                         {.type = TOKEN_INT, .value.integer = 1},
                         {.type = TOKEN_RIGHT_BRACKET},
                         {.type = TOKEN_SEMICOLON}};

    Token t;
    for (int i = 0; i < 8; i++)
    {
        getToken(&t);
        if (t.type != prologue[i].type)
            return 1;
        if (t.type == TOKEN_IDENTIFIER_FUNC)
        {
            if (strcmp(t.value.string.content, prologue[i].value.string.content))
            {
                vStrFree(&t.value.string);
                return 1;
            }
            vStrFree(&t.value.string);
        }
        if (t.type == TOKEN_INT)
        {
            if (t.value.integer != prologue[i].value.integer)
                return 1;
        }
    }

    return 0;
}

int parse()
{
    int errCode = checkPrologue();
    if (errCode != 0)
    {
        parserDestroy();
        return errCode;
    }
    // Insert builtin functions
    symtableAdd(parser.symtable, "floatval", FUNC, 1);
    symtableAdd(parser.symtable, "intval", FUNC, 1);
    symtableAdd(parser.symtable, "strval", FUNC, 1);
    symtableAdd(parser.symtable, "strlen", FUNC, 1);
    symtableAdd(parser.symtable, "substr", FUNC, 3);
    symtableAdd(parser.symtable, "ord", FUNC, 1);
    symtableAdd(parser.symtable, "chr", FUNC, 1);

    return 0;
}
