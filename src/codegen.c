/**
 * @file codegen.c
 * @author Lukáš Zedek (xzedek03)
 * @brief Code generator implementation.
 */

#include "stdio.h"
#include "codegen.h"

extern Parser parser;

void genPrintHead()
{
    printf(".IFJcode22\n");
    printf("DEFVAR GF@%%funcname\n");
    printf("MOVE GF@%%funcname string@main\n");
    printf("DEFVAR GF@%%exprresult\n");
    printf("MOVE GF@%%exprresult nil@nil\n");
    printf("DEFVAR GF@%%curr%%inst\n");
    printf("MOVE GF@%%curr%%inst nil@nil\n");
    printf("DEFVAR GF@%%exists\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genStackPush(Token t)
{
    switch (t.type)
    {
    case TOKEN_STRING:
        vStr temp;
        vStrInit(&temp);
        int i = 0, j = 0, c = 0;
        char esc[5] = "";
        while ((c = t.value.string.content[i]) != '\0')
        {
            if (c < 33 || c == 35 || c == 92 || c > 126)
            {
                vStrAppend(&temp, '\\');
                if (c < 0)
                    c += 256;
                sprintf(esc, "%03d", c);
                for (j = 0; j < 3; j++)
                {
                    vStrAppend(&temp, esc[j]);
                }
            }
            else
            {
                vStrAppend(&temp, c);
            }
            i++;
        }
        printf("PUSHS string@%s\n", temp.content);
        vStrFree(&temp);
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
        printf("MOVE GF@%%curr%%inst string@CONCAT\n");
        printf("CALL %%math%%check\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@%%p1\n");
        printf("DEFVAR LF@%%p2\n");
        printf("DEFVAR LF@%%result\n");
        printf("POPS LF@%%p2\n");
        printf("POPS LF@%%p1\n");
        printf("CONCAT LF@%%result LF@%%p1 LF@%%p2\n");
        printf("PUSHS LF@%%result\n");
        printf("POPFRAME\n");
        break;

    case TOKEN_MULTIPLY:
        printf("MOVE GF@%%curr%%inst string@MULS\n");
        printf("CALL %%math%%check\n");
        printf("MULS\n");
        break;

    case TOKEN_DIVIDE:
        printf("MOVE GF@%%curr%%inst string@DIVS\n");
        printf("CALL %%math%%check\n");
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
        printf("MOVE GF@%%curr%%inst string@GTS\n");
        printf("CALL %%math%%check\n");
        printf("GTS\n");
        break;

    case TOKEN_LESS_THAN:
        printf("MOVE GF@%%curr%%inst string@LTS\n");
        printf("CALL %%math%%check\n");
        printf("LTS\n");
        break;

    case TOKEN_EQUAL:
        printf("MOVE GF@%%curr%%inst string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@%%p1\n");
        printf("POPS LF@%%p1\n");
        printf("JUMPIFEQ %%bool%%false LF@%%p1 bool@false\n");
        printf("MOVE LF@%%p1 int@1\n");
        printf("JUMP %%bool%%out\n");
        printf("LABEL %%bool%%false\n");
        printf("MOVE LF@%%p1 int@0\n");
        printf("LABEL %%bool%%out\n");
        printf("PUSHS LF@%%p1\n");
        printf("POPFRAME\n");
        break;

    case TOKEN_NOT_EQUAL:
        printf("MOVE GF@%%curr%%inst string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        printf("NOTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@%%p1\n");
        printf("POPS LF@%%p1\n");
        printf("JUMPIFEQ %%bool%%false LF@%%p1 bool@false\n");
        printf("MOVE LF@%%p1 int@1\n");
        printf("JUMP %%bool%%out\n");
        printf("LABEL %%bool%%false\n");
        printf("MOVE LF@%%p1 int@0\n");
        printf("LABEL %%bool%%out\n");
        printf("PUSHS LF@%%p1\n");
        printf("POPFRAME\n");
        break;

    case TOKEN_IDENTIFIER_VAR:
        printf("POPFRAME\n");
        printf("PUSHS LF@%s\n", t.value.string.content);
        printf("PUSHFRAME\n");

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
    printf("WRITE GF@%%exprresult\n");
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
    printf("JUMP if%%%d%%ifdefs\n", ifCnt);
    printf("LABEL if%%%d%%else\n", ifCnt);
    ++ifCnt;
}

void genIfElse3()
{
    static int ifCnt = 0;
    printf("LABEL if%%%d%%ifdefs\n", ifCnt);
    printf("JUMP if%%%d%%end\n", ifCnt);
    printf("LABEL if%%%d%%end\n", ifCnt);
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
    printf("JUMPIFEQ %%EQS%%check GF@%%curr%%inst string@EQS\n");
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
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
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
    // equals type check
    printf("LABEL %%EQS%%check\n");
    printf("JUMPIFEQ %%EQS%%check%%1 LF@tmp1%%type LF@tmp2%%type\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("MOVE LF@tmp2 int@1\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%EQS%%check%%1\n");
    printf("JUMP %%math%%check%%exit\n");
    // lesser than & greater than type check
    printf("LABEL %%LTS%%GTS%%check%%1\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%tmp1 LF@tmp1%%type nil@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%nil%%tmp2 LF@tmp2%%type nil@nil\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%tmp1\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%int LF@tmp2%%type string@nil\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%int LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1tozero%%float LF@tmp2%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1toempty LF@tmp2%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1tozero%%int\n");
    printf("MOVE LF@tmp1 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1tozero%%float\n");
    printf("MOVE LF@tmp1 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1toempty\n");
    printf("MOVE LF@tmp1 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%nil%%tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2tozero%%int LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2tozero%%float LF@tmp1%%type string@float\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2toempty LF@tmp1%%type string@string\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2tozero%%int\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2tozero%%float\n");
    printf("MOVE LF@tmp2 float@0x0.0p+0\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP %%LTS%%GTS%%check%%1\n");
    printf("LABEL %%LTS%%GTS%%check%%2\n");
    printf("TYPE LF@tmp1%%type LF@tmp1\n");
    printf("TYPE LF@tmp2%%type LF@tmp2\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@tmp1%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%str LF@tmp2%%type string@string\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp1int LF@tmp1%%type string@int\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%tmp2int LF@tmp2%%type string@int\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%LTS%%GTS%%check%%str\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("EXIT int@7\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp1int\n");
    printf("JUMPIFEQ %%math%%check%%exit LF@tmp1%%type LF@tmp2%%type\n");
    printf("INT2FLOAT LF@tmp1 LF@tmp1\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("LABEL %%LTS%%GTS%%check%%tmp2int\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP %%LTS%%GTS%%check%%2\n");
    printf("JUMP %%math%%check%%exit\n");
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
    // exit for all
    printf("LABEL %%math%%check%%exit\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%skipcheck\n");
}

<<<<<<< HEAD
void genFuncCall()
{
    /*
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@%s%%retval\n",funcname);
    for (size_t i = 1; i < paramcnt; i++)
    {
        printf("MOVE TF@%s%%p%d LF@%s\n",funcname, i, paramval);
        printf("DEFVAR TF@%s%%p%d\n",funcname, i);
    }
    printf("PUSHFRAME\n");

    printf("POPFRAME\n");
    printf("");
    */
=======
void genDefineVariable(Token t)
{
    printf("DEFVAR LF@%s\n", t.value.string.content);
}

void genAssignVariable(Token t)
{
    printf("MOVE LF@%s GF@%%exprresult\n", t.value.string.content);
}

void genCheckDefined(Token t)
{
    static int n = 0;
    printf("POPFRAME\n");
    printf("TYPE GF@%%exists LF@%s\n", t.value.string.content);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("JUMPIFNEQ %%doesexist%i GF@%%exists string@\n", n);
    printf("EXIT int@5\n");
    printf("LABEL %%doesexist%i\n", n);
    printf("POPFRAME\n");
    n++;
>>>>>>> a6ab2dd (Added defined check for codegen)
}
