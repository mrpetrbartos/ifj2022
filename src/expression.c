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
    if (head.type == TOKEN_IDENTIFIER_VAR)
    {
        if (symtableFind(parser.symtable, head.value.string.content) == NULL)
        {
            vStrFree(&(head.value.string));
            printError(head.pos.line, head.pos.character, "Undefined variable used in an expression.");
            return ERR_UNDEF_VAR;
        }
    }

    Token t;
    stackPop(parser.stack, &t);
    // TODO: push instr
    printf("PUSHS %i\n", t.value.integer);
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

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    // TODO: push instr
    t.type == TOKEN_PLUS ? printf("ADDS\n") : printf("SUBS\n");
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
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    // TODO: push instr
    t.type == TOKEN_MULTIPLY ? printf("MULS\n") : printf("DIVS\n");
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

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    // TODO: push instr
    switch (t.type)
    {
    case TOKEN_GREATER_EQUAL:
        printf("GTS\n");
        printf("EQS\n");
        break;

    case TOKEN_LESS_EQUAL:
        printf("LTS\n");
        printf("EQS\n");
        break;

    case TOKEN_GREATER_THAN:
        printf("GTS\n");
        break;

    case TOKEN_LESS_THAN:
        printf("LTS\n");
        break;

    default:
        break;
    }
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

    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, &t);
    // TODO: push instr
    printf("AND");
    if (t.type == TOKEN_NOT_EQUAL)
        printf("NOT");
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
    Token t;
    stackPop(parser.stack, NULL);
    stackPop(parser.stack, NULL);
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

int shift()
{
    Token shift = {.type = SHIFT_SYMBOL};
    Token topmost = topmostTerminal();
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
    getToken(&(parser.currToken), false);

    return 0;
}

int parseExpression()
{
    Token bottom = {.type = DOLLAR};
    stackPush(parser.stack, bottom);

    while (true)
    {
        switch (getRelation(topmostTerminal(), parser.currToken))
        {
        case (R):
            if (reduce() != 0)
                return ERR_SYNTAX_AN;
            break;

        case (S):
            if (shift() != 0)
                return ERR_INTERNAL;
            break;

        case (E):
            stackPush(parser.stack, parser.currToken);
            getToken(&(parser.currToken), false);
            break;

        case (O):
            stackFree(parser.stack);
            return 0;

        default:
            return ERR_SYNTAX_AN;
        }
    }
}
