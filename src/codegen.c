/**
 * @file codegen.c
 * @author Lukáš Zedek (xzedek03)
 * @brief Code generator implementation.
 */

#include "stdio.h"
#include "codegen.h"

void genPrintHead()
{
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%funcname\n");
    printf("MOVE GF@%%funcname string@main\n");
    printf("DEFVAR GF@%%exprresult\n");
    printf("MOVE GF@%%exprresult nil@nil\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genStackPush(Token t)
{
    switch (t.type)
    {
    case TOKEN_STRING:
        printf("PUSHS string@%s\n", t.value.string.content);
        vStrFree(&t.value.string);
        break;

    case TOKEN_INT:
        printf("PUSHS int@%i\n", t.value.integer);
        break;

    case TOKEN_FLOAT:
        printf("PUSHS float@%f\n", t.value.decimal);
        break;

    case TOKEN_KEYWORD:
        printf("PUSHS nil@nil\n");
        break;

    case TOKEN_PLUS:
        printf("ADDS\n");
        break;

    case TOKEN_MINUS:
        printf("SUBS\n");
        break;

    case TOKEN_CONCAT:
        printf("CONCAT\n");
        break;

    case TOKEN_MULTIPLY:
        printf("MULS\n");
        break;

    case TOKEN_DIVIDE:
        printf("DIVS\n");
        break;

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
}

void genExpressionBegin()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genExpressionEnd()
{
    printf("POPS GF@%%exprresult\n");
    printf("POPFRAME\n");
}

void genIfBegin()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%iftype");
    printf("TYPE LF@%%iftype GF@%%exprresult");
}
