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
    for (int i = 0; i < 7; i++)
    {
        getToken(&t);
        if (t.type != prologue[i].type)
        {
            return 1;
        }
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

int parseIf()
{
    return 0;
}

int parseWhile()
{
    return 0;
}

int parseReturn()
{
    int err = 0;
    GETTOKEN(&parser.currToken)
    // TODO: if the fuction/main body doesn't return value,
    // return can't be followed by an expression
    if (!parser.outsideBody || (parser.outsideBody && false))
    {
        CHECKRULE(parseExpression())
    }
    else
    {
        bool expr;
        BEGINNINGOFEX()
        if (expr && parser.outsideBody)
        {
            printError(LINENUM, CHARNUM, "Current function doesn't have a return value, so return has to be followed by a semicolon.");
            return ERR_SYNTAX_AN;
        }
    }
    return err;
}

int parseBody()
{
    int err = 0;

    switch (parser.currToken.type)
    {
    case TOKEN_KEYWORD:
        switch (parser.currToken.value.keyword)
        {
        // <body> -> <if> ; <body>
        case KW_IF:
            CHECKRULE(parseIf())
            CHECKSEMICOLON()
            break;

        // <body> -> <while> ; <body>
        case KW_WHILE:
            CHECKRULE(parseWhile())
            CHECKSEMICOLON()
            break;

        // <body> -> <return> ; <body>
        case KW_RETURN:
            CHECKRULE(parseReturn())
            CHECKSEMICOLON()
            break;

        default:
            printError(LINENUM, CHARNUM, "Unexpected keyword found.");
            return ERR_SYNTAX_AN;
            break;
        }
        break;

    default:
        break;
    }

    return err;
}

int parseFunctionDef()
{
    return 0;
}

int parseMainBody()
{
    int err = 0;

    // <body_main> -> <func_def> <body_main>
    if (parser.currToken.type == TOKEN_KEYWORD && parser.currToken.value.keyword == KW_FUNCTION)
        CHECKRULE(parseFunctionDef())
    // <body_main> -> ε
    else if (parser.currToken.type == TOKEN_CLOSING_TAG || parser.currToken.type == TOKEN_EOF)
        return 0;
    // <body_main> -> <body> <body_main>
    else
        CHECKRULE(parseBody())

    GETTOKEN(&parser.currToken)
    CHECKRULE(parseMainBody())
    return err;
}

int parseProgramEnd()
{
    // <program_e> -> ε
    if (parser.currToken.type == TOKEN_CLOSING_TAG)
    {
        GETTOKEN(&parser.currToken)
        if (parser.currToken.type == TOKEN_EOF)
            return 0;
        else
        {
            printError(LINENUM, CHARNUM, "Closing tag can only be followed by EOL.");
            return ERR_SYNTAX_AN;
        }
    }
    // <program_e> -> ?>
    else if (parser.currToken.type == TOKEN_EOF)
        return 0;
    else
        return ERR_SYNTAX_AN;
}

int parseProgram()
{
    int err = 0;
    CHECKRULE(parseMainBody())
    CHECKRULE(parseProgramEnd())
    return err;
}

int parse()
{
    if (checkPrologue() != 0)
    {
        printError(0, 0, "Incorrect prologue format.");
        return ERR_SYNTAX_AN;
    }

    // Insert builtin functions
    symtableAdd(parser.symtable, "reads", FUNC, 0);
    symtableAdd(parser.symtable, "readi", FUNC, 0);
    symtableAdd(parser.symtable, "readf", FUNC, 0);
    symtableAdd(parser.symtable, "write", FUNC, -1);
    symtableAdd(parser.symtable, "floatval", FUNC, 1);
    symtableAdd(parser.symtable, "intval", FUNC, 1);
    symtableAdd(parser.symtable, "strval", FUNC, 1);
    symtableAdd(parser.symtable, "strlen", FUNC, 1);
    symtableAdd(parser.symtable, "substr", FUNC, 3);
    symtableAdd(parser.symtable, "ord", FUNC, 1);
    symtableAdd(parser.symtable, "chr", FUNC, 1);

    GETTOKEN(&parser.currToken)
    return parseProgram();

    return 0;
}
