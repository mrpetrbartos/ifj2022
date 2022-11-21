/**
 * @file expression.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Expression LR parser.
 */

#include "expression.h"

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

int reduceI()
{
    Token head = parser.stack->head->t;
    if (head.type == TOKEN_IDENTIFIER_VAR)
    {
        if (symtableFind(parser.symtable, head.value.string.content) == NULL)
        {
            vStrFree(&(head.value.string));
            printError(head.pos.line, head.pos.character, "Undefined variable used in an expression.");
            return ERR_UNDEF_REDEF_FUNC;
        }
    }

    // TODO: push instr
    printf("reduced identifier\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
    }
    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
};

int reducePlus()
{
    // TODO: push instr
    printf("reduced plus\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceMultiply()
{
    // TODO: push instr
    printf("reduced multiply\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceRelation()
{
    // TODO: push instr
    printf("reduced relational\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceComparison()
{
    // TODO: push instr
    printf("reduced comparison\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
    }

    t.type = REDUCED;
    stackPush(parser.stack, t);

    return 0;
}

int reduceBracket()
{
    // TODO: push instr
    printf("reduced brackets\n");

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    if (t.type != SHIFT_SYMBOL)
    {
        printError(0, 0, "Reduction of expression failed.");
        return ERR_INTERNAL;
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
    case TOKEN_NULL:
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
    switch (getTableIndex(parser.stack->head->t))
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

int parseExpression()
{
    Token bottom = {.type = DOLLAR};
    Token shift = {.type = SHIFT_SYMBOL};
    stackPush(parser.stack, bottom);
    getToken(&(parser.currToken));

    while (true)
    {
        switch (getRelation(parser.stack->head->t, parser.currToken))
        {
        case (R):
            if (reduce() != 0)
                return ERR_SYNTAX_AN;
            getToken(&(parser.currToken));
            break;

        case (S):
            getToken(&(parser.currToken));
            stackPush(parser.stack, shift);
            stackPush(parser.stack, parser.currToken);
            break;

        case (E):
            // reduce until reducible by E -> (E)
            if (reduce() != true)
                return ERR_SYNTAX_AN;
            break;

        case (O):
            stackFree(parser.stack);
            return 0;

        default:
            return ERR_SYNTAX_AN;
        }
    }
}
