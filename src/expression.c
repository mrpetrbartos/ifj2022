/**
 * @file expression.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Expression LR parser.
 */

#include "expression.h"
#include "codegen.h"

extern Parser parser;

int prec_table[8][8] = {
    //+, *, i, $, R, C, (, )
    {R, S, S, R, R, R, S, R}, // +
    {R, R, S, R, R, R, S, R}, // *
    {R, R, F, R, R, R, F, R}, // i
    {S, S, S, O, S, S, S, F}, // $
    {S, S, S, R, F, F, S, R}, // Relational Operators
    {S, S, S, R, F, F, S, R}, // Comparison Operators
    {S, S, S, F, S, S, S, E}, // (
    {R, R, F, R, R, R, F, R}  // )
};

Token topmostTerminal()
{
    StackItem *tmp = parser.stack->head;

    while (tmp != NULL)
    {
        if (tmp->t.type < 501)
            return tmp->t;
        else
            tmp = tmp->next;
    }

    Token err = {.type = 999};

    return err;
}

int reduceI()
{
    Token head = parser.stack->head->t;
    SymtablePair *foundVar;
    if (head.type == TOKEN_IDENTIFIER_VAR)
    {
        foundVar = symtableFind(parser.outsideBody ? parser.localSymtable : parser.symtable, head.value.string.content);
        if (foundVar == NULL)
        {
            vStrFree(&(head.value.string));
            printError(head.pos.line, head.pos.character, "Undefined variable used in an expression.");
            return ERR_UNDEF_VAR;
        }
    }

    Token t;
    stackPop(parser.stack, &t);
    if (t.type != TOKEN_IDENTIFIER_VAR)
        genStackPush(t);
    else
    {
        if (foundVar->data.possiblyUndefined)
            genCheckDefined(t);
        genStackPush(t);
    }
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }
    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
};

int reducePlus()
{

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceMultiply()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceRelation()
{

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceComparison()
{

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    genStackPush(t);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceBracket()
{
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_SYNTAX_AN;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

tableIndex getTableIndex(Token t)
{
    switch (t.type)
    {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_CONCAT:
        return I_PLUS;
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
        return I_MULTIPLY;
    case TOKEN_STRING:
    case TOKEN_INT:
    case TOKEN_FLOAT:
    case TOKEN_IDENTIFIER_VAR:
        return I_DATA;
    case TOKEN_GREATER_EQUAL:
    case TOKEN_GREATER_THAN:
    case TOKEN_LESS_EQUAL:
    case TOKEN_LESS_THAN:
        return I_RELATIONAL;
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL:
        return I_COMPARISON;
    case TOKEN_LEFT_BRACKET:
        return I_OPENB;
    case TOKEN_RIGHT_BRACKET:
        return I_CLOSEB;
    case TOKEN_KEYWORD:
        if (t.value.keyword == KW_NULL)
            return I_DATA;
        else
            return I_DOLLAR;
    default:
        return I_DOLLAR;
    }
}

precValues getRelation(Token top, Token new)
{
    return prec_table[getTableIndex(top)][getTableIndex(new)];
};

int reduce()
{
    switch (getTableIndex(topmostTerminal()))
    {
    case I_DATA:
        return reduceI();

    case I_PLUS:
        return reducePlus();

    case I_MULTIPLY:
        return reduceMultiply();

    case I_RELATIONAL:
        return reduceRelation();

    case I_COMPARISON:
        return reduceComparison();

    case I_CLOSEB:
        return reduceBracket();

    default:
        printError(0, 0, "No reduction rule for given token.");
        return ERR_INTERNAL;
    }
}

int shift(Token *preShift)
{
    Token shift = {.type = SHIFT_SYMBOL};
    Token topmost = topmostTerminal();
    if (topmost.type == 999)
    {
        printError(parser.currToken.pos.line, parser.currToken.pos.character, "Couldn't shift symbol, invalid expression.");
        return ERR_SYNTAX_AN;
    }
    StackItem *tmp = parser.stack->head;

    // Prepare stack to temporarily store tokens between
    // topmost nonterminal and top of the parser stack
    Stack *putaway = malloc(sizeof(Stack));
    if (putaway == NULL)
        return ERR_INTERNAL;
    stackInit(putaway);

    Token toPush;
    while (tmp->t.type != topmost.type)
    {
        stackPop(parser.stack, &toPush);
        stackPush(putaway, toPush);
        tmp = parser.stack->head;
    }

    // Push the shift symbol and return tokens to parser stack
    stackPush(parser.stack, shift);
    while (putaway->head != NULL)
    {
        stackPop(putaway, &toPush);
        stackPush(parser.stack, toPush);
    }

    free(putaway);

    stackPush(parser.stack, parser.currToken);
    *preShift = parser.currToken;
    int err = getToken(&(parser.currToken), false);

    return err;
}

int parseExpression(bool endWithBracket)
{
    int err = 0;
    Token bottom = {.type = DOLLAR};
    static Token beforeEnd = {.type = DOLLAR};
    stackPush(parser.stack, bottom);

    genExpressionBegin();

    while (true)
    {
        switch (getRelation(topmostTerminal(), parser.currToken))
        {
        case (R):
            err = reduce();
            break;

        case (S):
            err = shift(&beforeEnd);
            break;

        case (E):
            stackPush(parser.stack, parser.currToken);
            beforeEnd = parser.currToken;
            err = getToken(&(parser.currToken), false);
            break;

        case (O):
            stackFree(parser.stack);
            if (endWithBracket && beforeEnd.type != TOKEN_RIGHT_BRACKET)
            {
                printError(beforeEnd.pos.line, beforeEnd.pos.character, "Expression has to be wrapped by braces.");
                return 2;
            }
            genExpressionEnd();
            return 0;

        default:
            return ERR_SYNTAX_AN;
        }

        if (err != 0)
            return err;
    }
}
