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
    printf("DEFVAR GF@%%curr%%inst\n");
    printf("MOVE GF@%%curr%%inst nil@nil\n");
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
        printf("PUSHS float@%a\n", t.value.decimal);
        break;

    case TOKEN_KEYWORD:
        printf("PUSHS nil@nil\n");
        break;

    case TOKEN_PLUS:
        printf("MOVE GF@%%curr%%inst string@ADDS\n");
        printf("CALL %%math%%check\n");
        printf("ADDS\n");
        break;

    case TOKEN_MINUS:
        printf("MOVE GF@%%curr%%inst string@SUBS\n");
        printf("CALL %%math%%check\n");
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

void genCheckTruth()
{
    printf("JUMP %%initialskip\n");
    printf("LABEL %%truthcheck\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%iftype\n");
    printf("TYPE LF@%%iftype GF@%%exprresult\n");
    printf("DEFVAR LF@%%tmpbool\n");
    printf("JUMPIFEQ %%iftype%%int LF@%%iftype string@int\n");
    printf("JUMPIFEQ %%iftype%%float LF@%%iftype string@float\n");
    printf("JUMPIFEQ %%iftype%%nil LF@%%iftype string@nil\n");
    printf("LABEL %%iftype%%int\n");
    printf("EQ LF@%%tmpbool GF@%%exprresult int@0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprresult LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%float\n");
    printf("EQ LF@%%tmpbool GF@exprresult float@0x0.0p+0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprresult LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%nil\n");
    printf("MOVE GF@%%exprresult bool@false\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%string\n");
    printf("EQ LF@%%tmpbool GF@exprresult string@0\n");
    printf("JUMPIFEQ %%iftype%%string%%false LF@%%tmpbool bool@true\n");
    printf("EQ LF@%%tmpbool GF@exprresult string@\n");
    printf("JUMPIFEQ %%iftype%%string%%false LF@%%tmpbool bool@true\n");
    printf("MOVE GF@%%exprresult bool@true\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%string%%false\n");
    printf("MOVE GF@%%exprresult bool@false\n");
    printf("JUMP %%out\n");
    printf("LABEL %%out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%initialskip\n");
}

void genIfElse1()
{
    static int ifCnt = 0;
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ if%%%d%%else GF@%%exprresult bool@false\n", ifCnt);
    ++ifCnt;
}

void genIfElse2()
{
    static int ifCnt = 0;
    printf("JUMP if%%%d%%end\n", ifCnt);
    printf("LABEL if%%%d%%else\n", ifCnt);
    ++ifCnt;
}

void genIfElse3()
{
    static int ifCnt = 0;
    printf("LABEL if%%%d%%end ", ifCnt);
    ++ifCnt;
}

void genWhileLoop1()
{
    static int whileCnt = 0;
    printf("LABEL while%%%d%%start\n", whileCnt);
    ++whileCnt;
}

void genWhileLoop2()
{
    static int whileCnt = 0;
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ while%%%d%%end GF@%%exprresult bool@false\n", whileCnt);
    ++whileCnt;
}

void genWhileLoop3()
{
    static int whileCnt = 0;
    printf("JUMP while%%%d%%start\n", whileCnt);
    printf("LABEL while%%%d%%end\n", whileCnt);
    ++whileCnt;
}

void genMathInstCheck()
{
    printf("JUMP %%skipcheck\n");
    printf("LABEL %%math%%check\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@tmp1\n");
    printf("DEFVAR LF@tmp1%%type\n");
    printf("POPS LF@tmp1\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("DEFVAR LF@tmp2\n");
    printf("DEFVAR LF@tmp2%%type\n");
    printf("POPS LF@tmp2\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%curr%%inst string@ADDS\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%curr%%inst string@SUBS\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check GF@%%curr%%inst string@MULS\n");
    printf("JUMPIFEQ %%DIVS%%check GF@%%curr%%inst string@DIVS\n");
    printf("JUMPIFEQ %%CONCAT%%check GF@%%curr%%inst string@CONCAT\n");
    printf("JUMP %%math%%check%%exit\n");
    // addition, subtraction, multiplication type check
    printf("LABEL %%ADDS%%SUBS%%MULS%%check\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%1\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp1tozero LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp2tozero LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%3\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp1tofloat LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%ADDS%%SUBS%%MULS%%check%%tmp2tofloat LF@tmp2%%type string@int\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp1tofloat\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp1tozero\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    printf("LABEL %%ADDS%%SUBS%%MULS%%check%%tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%ADDS%%SUBS%%MULS%%check%%2\n");
    // division type check
    printf("LABEL %%DIVS%%check\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%1\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%DIVS%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%DIVS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmptmp21\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%DIVS%%check%%tmp1tozero LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%tmp2tozero LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%DIVS%%check%%next LF@tmp1%%type string@float\n");
    printf("JUMP %%DIVS%%check%%tmp1tofloat\n");
    printf("LABEL %%DIVS%%check%%next\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp2%%type string@float\n");
    printf("JUMP %%DIVS%%check%%tmp2tofloat\n");
    printf("LABEL %%DIVS%%check%%tmp1tofloat\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp1tozero\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    printf("LABEL %%DIVS%%check%%tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%DIVS%%check%%2\n");
    // concatenation type check
    printf("LABEL %%CONCAT%%check\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@tmp1%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%1 LF@tmp1%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%1\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@tmp2%%type string@string\n");
    printf("JUMPIFEQ %%CONCAT%%check%%2 LF@tmp2%%type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL %%CONCAT%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%CONCAT%%check%%tmp1toempty LF@tmp1%%type string@nil\n");
    printf("JUMPIFEQ %%CONCAT%%check%%tmp2toempty LF@tmp2%%type string@nil\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%CONCAT%%check%%tmp1toempty\n");
    printf("MOVE LF@tmp1 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    printf("LABEL %%CONCAT%%check%%tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP %%CONCAT%%check%%2\n");
    printf("LABEL %%math%%check%%exit\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%skipcheck\n");
}
