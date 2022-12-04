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
    Symtable *stLoc = symtableInit(1024);
    if (stLoc == NULL)
        return ERR_INTERNAL;
    parser.stack = s;
    parser.outsideBody = false;
    parser.symtable = st;
    parser.localSymtable = stLoc;
    parser.inIf = false;
    return 0;
}

void parserDestroy()
{
    symtableFree(parser.symtable);
    symtableFree(parser.localSymtable);
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
        getToken(&t, false);
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

int parseBody();

int parsePeBody()
{
    int err = 0;
    //<pe_body> -> .
    if (parser.currToken.type == TOKEN_RIGHT_BRACE)
        return 0;
    // <pe_body> -> <body> <pe_body>.
    else
    {
        CHECKRULE(parseBody())
        GETTOKEN(&parser.currToken)
        CHECKRULE(parsePeBody())
    }

    return err;
}

// <while> -> while ( expr ) { <pe_body> }.
int parseWhile()
{
    int err = 0;
    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "While condition has to be wrapped by brackets.");
        return ERR_SYNTAX_AN;
    }

    CHECKRULE(parseExpression())

    // Right bracket is checked by expression parsing

    if (parser.currToken.type != TOKEN_LEFT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of while has to be wrapped by braces (opening).");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    CHECKRULE(parsePeBody())

    if (parser.currToken.type != TOKEN_RIGHT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of while has to be wrapped by braces (closing).");
        return ERR_SYNTAX_AN;
    }

    return err;
}

// <if> -> if ( expr ) { <pe_body> } else { <pe_body> }.
int parseIf()
{
    int err = 0;
    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "If condition has to be wrapped by brackets.");
        return ERR_SYNTAX_AN;
    }

    CHECKRULE(parseExpression())

    // Right bracket is checked by expression parsing

    if (parser.currToken.type != TOKEN_LEFT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of if has to be wrapped by braces (opening).");
        return ERR_SYNTAX_AN;
    }

    parser.inIf = true;

    GETTOKEN(&parser.currToken)
    CHECKRULE(parsePeBody())

    parser.inIf = false;

    if (parser.currToken.type != TOKEN_RIGHT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of if has to be wrapped by braces (closing).");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_KEYWORD || parser.currToken.value.keyword != KW_ELSE)
    {
        printError(LINENUM, CHARNUM, "If has to have else clause.");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of else has to be wrapped by braces (opening).");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    CHECKRULE(parsePeBody())

    if (parser.currToken.type != TOKEN_RIGHT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of else has to be wrapped by braces (closing).");
        return ERR_SYNTAX_AN;
    }

    return err;
}

int parseReturn()
{
    // <return> -> return <return_p>.
    int err = 0;
    GETTOKEN(&parser.currToken)
    // TODO: if the fuction/main body doesn't return value,
    // return can't be followed by an expression
    // <return_p>  -> expr.
    if (!parser.outsideBody || (parser.outsideBody && true))
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
    // <return_p>  -> .
    return err;
}

int parseParamsCallN(int *pc)
{
    int err = 0;
    switch (parser.currToken.type)
    {
    case TOKEN_STRING:
    case TOKEN_INT:
    case TOKEN_FLOAT:
        ++(*pc);
        break;

    case TOKEN_IDENTIFIER_VAR:
        if (symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable, parser.currToken.value.string.content) == NULL)
        {
            vStrFree(&(parser.currToken.value.string));
            printError(LINENUM, CHARNUM, "Redefinition of function.");
            return ERR_UNDEF_VAR;
        }
        ++(*pc);
        break;

    case TOKEN_KEYWORD:
        if (parser.currToken.value.keyword == KW_NULL)
        {
            ++(*pc);
            break;
        }
        else
        {
            printError(LINENUM, CHARNUM, "Function can only be called with a variable or a literal.");
            return ERR_SYNTAX_AN;
            break;
        }

    default:
        printError(LINENUM, CHARNUM, "Function can only be called with a variable or a literal.");
        return ERR_SYNTAX_AN;
        break;
    }

    GETTOKEN(&parser.currToken)
    // <params_cn> -> , <params_cp> <params_n>.
    if (parser.currToken.type == TOKEN_COMMA)
    {
        GETTOKEN(&parser.currToken)
        return parseParamsCallN(pc);
    }
    // <params_cn> -> .
    else
        return err;
}

int parseParamsCall(int *pc)
{
    // <params_cp> -> .
    if (parser.currToken.type == TOKEN_RIGHT_BRACKET)
        return 0;
    // <params_cp> -> lit <params_cn>.
    // <params_cp> -> identifier_var <params_cn>.
    else
        return parseParamsCallN(pc);
}

int parseFunctionCall()
{
    int err = 0;

    SymtablePair *foundFunction = symtableFind(parser.symtable, parser.currToken.value.string.content);

    if (foundFunction == NULL)
    {
        vStrFree(&(parser.currToken.value.string));
        printError(LINENUM, CHARNUM, "Calling an undefined function.");
        return ERR_UNDEF_REDEF_FUNC;
    }

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function's parameters have to be in a bracket (opening).");
        return ERR_SYNTAX_AN;
    }

    int parametersRealCount = 0;

    GETTOKEN(&parser.currToken)
    CHECKRULE(parseParamsCall(&parametersRealCount))

    if ((parametersRealCount != foundFunction->data.paramsCnt) && foundFunction->data.paramsCnt != -1)
    {
        printError(LINENUM, CHARNUM, "Wrong number of parameters passed");
        return ERR_FUNC_VAR;
    }

    if (parser.currToken.type != TOKEN_RIGHT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function's parameters have to be in a bracket (closing).");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)

    return err;
}

int parseAssign()
{
    int err = 0;
    GETTOKEN(&parser.currToken)
    // <assign_v> -> <func>
    if (parser.currToken.type == TOKEN_IDENTIFIER_FUNC)
    {
        CHECKRULE(parseFunctionCall())
    }
    // <assign_v> -> expr
    else
    {
        CHECKRULE(parseExpression())
    }
    return err;
}

int parseBody()
{
    int err = 0;

    if (parser.currToken.type == TOKEN_KEYWORD)
        switch (parser.currToken.value.keyword)
        {
        // <body> -> <if> <body>
        case KW_IF:
            CHECKRULE(parseIf())
            break;

        // <body> -> <while> ; <body>
        case KW_WHILE:
            CHECKRULE(parseWhile())
            break;

        // <body> -> <return> ; <body>
        case KW_RETURN:
            CHECKRULE(parseReturn())
            CHECKSEMICOLON()
            break;

        case KW_NULL:
            CHECKRULE(parseExpression())
            CHECKSEMICOLON()
            break;

        default:
            printError(LINENUM, CHARNUM, "Unexpected keyword found.");
            return ERR_SYNTAX_AN;
            break;
        }
    else if (parser.currToken.type == TOKEN_IDENTIFIER_VAR)
    {
        Token nextToken;
        if (getToken(&nextToken, true) != 0)
            return ERR_LEXICAL_AN;

        // <body> -> identifier_var = <assign_v> ; <body>.
        if (nextToken.type == TOKEN_ASSIGN)
        {
            Token variable = parser.currToken;
            GETTOKEN(&parser.currToken)
            CHECKRULE(parseAssign())
            LinkedList empty = {.itemCount = 0};
            symtableAdd(parser.outsideBody ? parser.localSymtable : parser.symtable, variable.value.string.content, VAR, -1, parser.inIf, empty);
        }
        // <body> -> expr ; <body>.
        else
        {
            CHECKRULE(parseExpression())
        }
        CHECKSEMICOLON()
    }
    else if (parser.currToken.type == TOKEN_IDENTIFIER_FUNC)
    {
        CHECKRULE(parseFunctionCall())
        CHECKSEMICOLON()
    }
    else
    {
        // <body> -> expr ; <body>.
        CHECKRULE(parseExpression())
        CHECKSEMICOLON()
    }

    return err;
}

int parseTypeP(LinkedList *ll)
{
    if (parser.currToken.type != TOKEN_KEYWORD)
    {
        printError(LINENUM, CHARNUM, "Expected variable type.");
        return ERR_SYNTAX_AN;
    }

    // <type_p> -> float.
    // <type_p> -> int.
    // <type_p> -> string.
    switch (parser.currToken.value.keyword)
    {
    case KW_STRING:
    case KW_INT:
    case KW_FLOAT:
        if (ll != NULL)
            listInsert(ll, parser.currToken.value.keyword);
        return 0;

    default:
        printError(LINENUM, CHARNUM, "Expected variable type.");
        return ERR_SYNTAX_AN;
    }
}

int parseTypeN(LinkedList *ll)
{
    int err = 0;
    GETTOKEN(&parser.currToken)
    CHECKRULE(parseTypeP(ll))
    return err;
}

int parseParamsDefN(LinkedList *ll)
{
    int err = 0;
    if (parser.currToken.type != TOKEN_OPTIONAL_TYPE)
    {
        CHECKRULE(parseTypeP(ll))
    }
    else
    {
        CHECKRULE(parseTypeN(ll))
    }

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_IDENTIFIER_VAR)
    {
        printError(LINENUM, CHARNUM, "Type has to be followed by a variable.");
        return ERR_SYNTAX_AN;
    }

    LinkedList empty = {.itemCount = 0};
    symtableAdd(parser.localSymtable, parser.currToken.value.string.content, VAR, -1, parser.inIf, empty);

    GETTOKEN(&parser.currToken)
    // <params_dn>  -> , <params_dp> <params_n>.
    if (parser.currToken.type == TOKEN_COMMA)
    {
        GETTOKEN(&parser.currToken)
        return parseParamsDefN(ll);
    }
    // <params_dn>  -> .
    else
        return err;
}

int parseParamsDef(LinkedList *ll)
{
    // <params_dp>  -> .
    if (parser.currToken.type == TOKEN_RIGHT_BRACKET)
        return 0;
    // <params_dp>  -> <type_p> identifier_var <params_dn>.
    else
        return parseParamsDefN(ll);
}

int parseType()
{
    if (parser.currToken.type == TOKEN_OPTIONAL_TYPE)
    {
        return parseTypeN(NULL);
    }
    // <type> -> void.
    // <type> -> <type_p>.
    else if (parser.currToken.type == TOKEN_KEYWORD && parser.currToken.value.keyword == KW_VOID)
        return 0;

    // <type> -> ?<type_p>.
    return parseTypeP(NULL);
}

// <func_def>  -> function identifier_func ( <params_dp> ) : <type> { <pe_body> }.
int parseFunctionDef()
{
    int err = 0;
    parser.outsideBody = true;

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_IDENTIFIER_FUNC)
    {
        printError(LINENUM, CHARNUM, "Keyword function has to be followed by function identifier.");
        return ERR_SYNTAX_AN;
    }

    if (symtableFind(parser.symtable, parser.currToken.value.string.content) != NULL)
    {
        vStrFree(&(parser.currToken.value.string));
        printError(LINENUM, CHARNUM, "Redefinition of function.");
        return ERR_UNDEF_REDEF_FUNC;
    }

    Token func = parser.currToken;

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function definition has to have (empty) set of params.");
        return ERR_SYNTAX_AN;
    }

    // TODO: linked list of parameters?
    LinkedList ll;
    listInit(&ll);
    GETTOKEN(&parser.currToken)
    CHECKRULE(parseParamsDef(&ll))

    if (parser.currToken.type != TOKEN_RIGHT_BRACKET)
    {
        printError(LINENUM, CHARNUM, "Function definition has to have (empty) set of params.");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_COLON)
    {
        printError(LINENUM, CHARNUM, "Incorrect function definition syntax, missing colon.");
        return ERR_SYNTAX_AN;
    }

    // TODO: return value type to symtable
    GETTOKEN(&parser.currToken)
    CHECKRULE(parseType())

    GETTOKEN(&parser.currToken)
    if (parser.currToken.type != TOKEN_LEFT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of function has to be wrapped by braces (opening).");
        return ERR_SYNTAX_AN;
    }

    GETTOKEN(&parser.currToken)
    CHECKRULE(parsePeBody())

    if (parser.currToken.type != TOKEN_RIGHT_BRACE)
    {
        printError(LINENUM, CHARNUM, "The body of function has to be wrapped by braces (closing).");
        return ERR_SYNTAX_AN;
    }

    symtableAdd(parser.symtable, func.value.string.content, FUNC, ll.itemCount, parser.inIf, ll);

    parser.outsideBody = false;
    return err;
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

// <program> -> <body_main> <program_e>.
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

    LinkedList empty = {.itemCount = 0};
    // Insert builtin functions
    symtableAdd(parser.symtable, "reads", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readi", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "readf", FUNC, 0, false, empty);
    symtableAdd(parser.symtable, "write", FUNC, -1, false, empty);
    symtableAdd(parser.symtable, "floatval", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "intval", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "strval", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "strlen", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "substring", FUNC, 3, false, empty);
    symtableAdd(parser.symtable, "ord", FUNC, 1, false, empty);
    symtableAdd(parser.symtable, "chr", FUNC, 1, false, empty);

    GETTOKEN(&parser.currToken)
    return parseProgram();

    return 0;
}
