/**
 * @file parser.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Top-down token parser.
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

int parse()
{
    int parsingCode = parseExpression();
    if (parsingCode != 0)
        return parsingCode;
    return 0;
}
