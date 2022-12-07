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
    printf("DEFVAR GF@%%corrtype%%1\n");
    printf("MOVE GF@%%corrtype%%1 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%2\n");
    printf("MOVE GF@%%corrtype%%2 nil@nil\n");
    printf("DEFVAR GF@%%corrtype%%opt\n");
    printf("MOVE GF@%%corrtype%%opt nil@nil\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genConvertBool()
{
    static int cnt = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%p1\n");
    printf("POPS LF@%%p1\n");
    printf("JUMPIFEQ %%bool%d%%false LF@%%p1 bool@false\n", cnt);
    printf("MOVE LF@%%p1 int@1\n");
    printf("JUMP %%bool%d%%out\n", cnt);
    printf("LABEL %%bool%d%%false\n", cnt);
    printf("MOVE LF@%%p1 int@0\n");
    printf("LABEL %%bool%d%%out\n", cnt);
    printf("PUSHS LF@%%p1\n");
    printf("POPFRAME\n");
    cnt++;
}

void genStackPush(Token t)
{
    switch (t.type)
    {
    case TOKEN_STRING:
    {
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
    }

    case TOKEN_INT:
        printf("PUSHS int@%d\n", t.value.integer);
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
        printf("MOVE GF@%%curr%%inst string@GTE\n");
        printf("CALL %%math%%check\n");
        printf("GTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@GTE%%tmp\n");
        printf("POPS LF@GTE%%tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@GTE%%tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TOKEN_LESS_EQUAL:
        printf("MOVE GF@%%curr%%inst string@LTE\n");
        printf("CALL %%math%%check\n");
        printf("LTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@LTE%%tmp\n");
        printf("POPS LF@LTE%%tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@LTE%%tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TOKEN_GREATER_THAN:
        printf("MOVE GF@%%curr%%inst string@GTS\n");
        printf("CALL %%math%%check\n");
        printf("GTS\n");
        genConvertBool();
        break;

    case TOKEN_LESS_THAN:
        printf("MOVE GF@%%curr%%inst string@LTS\n");
        printf("CALL %%math%%check\n");
        printf("LTS\n");
        genConvertBool();
        break;

    case TOKEN_EQUAL:
        printf("MOVE GF@%%curr%%inst string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        genConvertBool();
        break;

    case TOKEN_NOT_EQUAL:
        printf("MOVE GF@%%curr%%inst string@EQS\n");
        printf("CALL %%math%%check\n");
        printf("EQS\n");
        printf("NOTS\n");
        genConvertBool();
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
    printf("JUMPIFEQ %%iftype%%string LF@%%iftype string@string\n");
    printf("EXIT int@7\n"); // TODO chyba pokud string@string, maybe 7?
    printf("LABEL %%iftype%%int\n");
    printf("EQ LF@%%tmpbool GF@%%exprresult int@0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprresult LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%float\n");
    printf("EQ LF@%%tmpbool GF@%%exprresult float@0x0.0p+0\n");
    printf("NOT LF@%%tmpbool LF@%%tmpbool\n");
    printf("MOVE GF@%%exprresult LF@%%tmpbool\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%nil\n");
    printf("MOVE GF@%%exprresult bool@false\n");
    printf("JUMP %%out\n");
    printf("LABEL %%iftype%%string\n");
    printf("JUMPIFEQ %%iftype%%string%%false GF@%%exprresult string@0\n");
    printf("JUMPIFEQ %%iftype%%string%%false GF@%%exprresult string@\n");
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

void genIfElse1(int num)
{
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ if%%%d%%else GF@%%exprresult bool@false\n", num);
}

void genIfElse2(int num)
{
    printf("JUMP if%%%d%%ifdefs\n", num);
    printf("LABEL if%%%d%%else\n", num);
}

void genIfElse3(int num)
{
    printf("LABEL if%%%d%%ifdefs\n", num);
    printf("JUMP if%%%d%%end\n", num);
    printf("LABEL if%%%d%%end\n", num);
}

void genWhileLoop1(int num)
{
    printf("LABEL while%%%d%%start\n", num);
}

void genWhileLoop2(int num)
{
    printf("CALL %%truthcheck\n");
    printf("JUMPIFEQ while%%%d%%end GF@%%exprresult bool@false\n", num);
}

void genWhileLoop3(int num)
{
    printf("JUMP while%%%d%%start\n", num);
    printf("LABEL while%%%d%%end\n", num);
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
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%curr%%inst string@GTS\n");
    printf("JUMPIFEQ %%LTS%%GTS%%check%%1 GF@%%curr%%inst string@LTS\n");
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
    // end check for LTE & GTE operations
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%curr%%inst string@LTE\n");
    printf("JUMPIFEQ %%OR%%EQUAL%%VARIANT GF@%%curr%%inst string@GTE\n");
    printf("JUMP %%math%%check%%exit\n");
    printf("LABEL %%OR%%EQUAL%%VARIANT\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("JUMP %%math%%check%%exit\n");
    // exit for all
    printf("LABEL %%math%%check%%exit\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmp1\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    // type check for function parameters
    printf("LABEL %%corrtype%%func\n");
    printf("JUMPIFEQ %%match%%nil%%incl GF@%%corrtype%%opt int@1\n");
    printf("JUMPIFEQ %%match%%exact GF@%%corrtype%%opt int@0\n");
    printf("LABEL %%match%%nil%%incl\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 GF@%%corrtype%%2\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 string@nil\n");
    printf("EXIT int@4\n");
    printf("LABEL %%match%%exact\n");
    printf("JUMPIFEQ %%match%%passed GF@%%corrtype%%1 GF@%%corrtype%%2\n");
    printf("EXIT int@4\n");
    printf("LABEL %%match%%passed\n");
    printf("RETURN\n");
    printf("LABEL %%skipcheck\n");
}

void generateWrite(int numofparams)
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    for (int i = numofparams; i > 0; i--)
    {
        printf("DEFVAR LF@%%write%d\n", i);
        printf("POPS LF@%%write%d\n", i);
    }
    for (int i = 1; i <= numofparams; i++)
    {
        printf("WRITE LF@%%write%d\n", i);
    }
    printf("POPFRAME\n");
}

void genStrLen()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@str\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("POPS LF@str\n");
    printf("TYPE LF@type LF@str\n");
    printf("JUMPIFEQ func%%strlen%%%d%%isstring LF@type string@string\n", n);
    printf("EXIT int@4\n");
    printf("LABEL func%%strlen%%%d%%isstring\n", n);
    printf("STRLEN LF@strlen%%retval LF@str\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@strlen%%retval\n");
    n++;
}

void genOrd()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@str\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("POPS LF@str\n");
    printf("TYPE LF@type LF@str\n");
    printf("JUMPIFEQ func%%ord%%%d%%isstring LF@type string@string\n", n);
    printf("EXIT int@4\n");
    printf("LABEL func%%ord%%%d%%isstring\n", n);
    printf("STRLEN LF@strlen%%retval LF@str\n");
    printf("JUMPIFEQ func%%ord%%%d%%ordjump LF@strlen%%retval int@0\n", n);
    printf("STRI2INT LF@strlen%%retval LF@str int@0\n");
    printf("LABEL func%%ord%%%d%%ordjump\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@strlen%%retval\n");
    n++;
}

void genChr()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@num\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@strlen%%retval\n");
    printf("MOVE LF@strlen%%retval string@a\n");
    printf("POPS LF@num\n");
    printf("TYPE LF@type LF@num\n");
    printf("JUMPIFEQ func%%chr%%%d%%isint LF@type string@int\n", n);
    printf("EXIT int@4\n");
    printf("LABEL func%%chr%%%d%%isint\n", n);
    printf("INT2CHAR LF@strlen%%retval LF@num\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@strlen%%retval\n");
    n++;
}

void genReads()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Reads%%retval\n");
    printf("READ LF@Reads%%retval string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@Reads%%retval\n");
}

void genReadi()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Readi%%retval\n");
    printf("READ LF@Readi%%retval int\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@Readi%%retval\n");
}

void genReadf()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@Readf%%retval\n");
    printf("READ LF@Readf%%retval float\n");
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@Readf%%retval\n");
}

void genFloatval()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%floatval%%%d%%retval\n", n);
    printf("DEFVAR LF@%%floatval%%%d%%val\n", n);
    printf("DEFVAR LF@%%floatval%%%d%%val%%type\n", n);
    printf("POPS LF@%%floatval%%%d%%val\n", n);
    printf("TYPE LF@%%floatval%%%d%%val%%type LF@%%floatval%%%d%%val\n", n, n);
    printf("JUMPIFEQ %%func%%floatval%%%d%%float LF@%%floatval%%%d%%val%%type string@float\n", n, n);
    printf("JUMPIFEQ %%func%%floatval%%%d%%int LF@%%floatval%%%d%%val%%type string@int\n", n, n);
    printf("JUMPIFEQ %%func%%floatval%%%d%%nil LF@%%floatval%%%d%%val%%type string@nil\n", n, n);
    printf("JUMPIFEQ %%func%%floatval%%%d%%string LF@%%floatval%%%d%%val%%type string@string\n", n, n);
    printf("LABEL %%func%%floatval%%%d%%float\n", n);
    printf("MOVE LF@%%floatval%%%d%%retval LF@%%floatval%%%d%%val\n", n, n);
    printf("JUMP %%func%%floatval%%%d%%end\n", n);
    printf("LABEL %%func%%floatval%%%d%%int\n", n);
    printf("INT2FLOAT LF@%%floatval%%%d%%retval LF@%%floatval%%%d%%val\n", n, n);
    printf("JUMP %%func%%floatval%%%d%%end\n", n);
    printf("LABEL %%func%%floatval%%%d%%string\n", n);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%floatval%%%d%%nil\n", n);
    printf("MOVE LF@%%floatval%%%d%%retval float@0x0.0p+0\n", n);
    printf("JUMP %%func%%floatval%%%d%%end\n", n);
    printf("LABEL %%func%%floatval%%%d%%end\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@%%floatval%%%d%%retval\n", n);
    n++;
}

void genIntval()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%intval%%%d%%retval\n", n);
    printf("DEFVAR LF@%%intval%%%d%%val\n", n);
    printf("DEFVAR LF@%%intval%%%d%%val%%type\n", n);
    printf("POPS LF@%%intval%%%d%%val\n", n);
    printf("TYPE LF@%%intval%%%d%%val%%type LF@%%intval%%%d%%val\n", n, n);
    printf("JUMPIFEQ %%func%%intval%%%d%%int LF@%%intval%%%d%%val%%type string@int\n", n, n);
    printf("JUMPIFEQ %%func%%intval%%%d%%float LF@%%intval%%%d%%val%%type string@float\n", n, n);
    printf("JUMPIFEQ %%func%%intval%%%d%%nil LF@%%intval%%%d%%val%%type string@nil\n", n, n);
    printf("JUMPIFEQ %%func%%intval%%%d%%string LF@%%intval%%%d%%val%%type string@string\n", n, n);
    printf("LABEL %%func%%intval%%%d%%int\n", n);
    printf("MOVE LF@%%intval%%%d%%retval LF@%%intval%%%d%%val\n", n, n);
    printf("JUMP %%func%%intval%%%d%%end\n", n);
    printf("LABEL %%func%%intval%%%d%%float\n", n);
    printf("FLOAT2INT LF@%%intval%%%d%%retval LF@%%intval%%%d%%val\n", n, n);
    printf("JUMP %%func%%intval%%%d%%end\n", n);
    printf("LABEL %%func%%intval%%%d%%nil\n", n);
    printf("MOVE LF@%%intval%%%d%%retval int@0\n", n);
    printf("JUMP %%func%%intval%%%d%%end\n", n);
    printf("LABEL %%func%%intval%%%d%%string\n", n);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%intval%%%d%%end\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@%%intval%%%d%%retval\n", n);
    n++;
}

void genStrval()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%strval%%%d%%retval\n", n);
    printf("DEFVAR LF@%%strval%%%d%%val\n", n);
    printf("DEFVAR LF@%%strval%%%d%%val%%type\n", n);
    printf("POPS LF@%%strval%%%d%%val\n", n);
    printf("TYPE LF@%%strval%%%d%%val%%type LF@%%strval%%%d%%val\n", n, n);
    printf("JUMPIFEQ %%func%%strval%%%d%%string LF@%%strval%%%d%%val%%type string@string\n", n, n);
    printf("JUMPIFEQ %%func%%strval%%%d%%int LF@%%strval%%%d%%val%%type string@int\n", n, n);
    printf("JUMPIFEQ %%func%%strval%%%d%%float LF@%%strval%%%d%%val%%type string@float\n", n, n);
    printf("JUMPIFEQ %%func%%strval%%%d%%nil LF@%%strval%%%d%%val%%type string@nil\n", n, n);
    printf("LABEL %%func%%strval%%%d%%string\n", n);
    printf("MOVE LF@%%strval%%%d%%retval LF@%%strval%%%d%%val\n", n, n);
    printf("JUMP %%func%%strval%%%d%%end\n", n);
    printf("LABEL %%func%%strval%%%d%%nil\n", n);
    printf("MOVE LF@%%strval%%%d%%retval string@\n", n);
    printf("JUMP %%func%%strval%%%d%%end\n", n);
    printf("LABEL %%func%%strval%%%d%%int\n", n);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%strval%%%d%%float\n", n);
    printf("EXIT int@4\n");
    printf("LABEL %%func%%strval%%%d%%end\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@%%strval%%%d%%retval\n", n);
    n++;
}

void genSubstring()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@%%substring%%%d%%retval\n", n);
    printf("DEFVAR LF@%%substring%%%d%%currchr\n", n);
    printf("DEFVAR LF@%%i\n");
    printf("DEFVAR LF@%%j\n");
    printf("DEFVAR LF@%%str\n");
    printf("DEFVAR LF@%%throwaway\n");
    printf("DEFVAR LF@%%len\n");
    printf("MOVE LF@%%substring%%%d%%retval nil@nil\n", n);
    printf("POPS LF@%%j\n");
    printf("POPS LF@%%i\n");
    printf("POPS LF@%%str\n");
    printf("TYPE LF@%%throwaway LF@%%j\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@int\n", n);
    printf("TYPE LF@%%throwaway LF@%%i\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@int\n", n);
    printf("TYPE LF@%%throwaway LF@%%str\n");
    printf("JUMPIFNEQ %%badtype%d LF@%%throwaway string@string\n", n);
    printf("JUMP %%correcttype%d\n", n);
    printf("LABEL %%badtype%d\n", n);
    printf("EXIT int@4\n");
    printf("LABEL %%correcttype%d\n", n);
    printf("GT LF@%%throwaway LF@%%i LF@%%j\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);
    printf("LT LF@%%throwaway LF@%%i int@0\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);
    printf("LT LF@%%throwaway LF@%%j int@0\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("EQ LF@%%throwaway LF@%%throwaway LF@%%i\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("GT LF@%%throwaway LF@%%i LF@%%throwaway\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);
    printf("STRLEN LF@%%throwaway LF@%%str\n");
    printf("GT LF@%%throwaway LF@%%j LF@%%throwaway\n");
    printf("JUMPIFEQ %%returnnull%d LF@%%throwaway bool@true\n", n);

    printf("MOVE LF@%%len LF@%%j\n");
    printf("SUB LF@%%len LF@%%len LF@%%i\n");
    printf("MOVE LF@%%substring%%%d%%retval string@\n", n);
    printf("JUMPIFEQ %%returnnull%d LF@%%len int@0\n", n);
    printf("LABEL %%for%d\n", n);
    printf("GETCHAR LF@%%substring%%%d%%currchr LF@%%str LF@%%i\n", n);
    printf("CONCAT LF@%%substring%%%d%%retval LF@%%substring%%%d%%retval LF@%%substring%%%d%%currchr\n", n, n, n);
    printf("SUB LF@%%len LF@%%len int@1\n");
    printf("ADD LF@%%i LF@%%i int@1\n");
    printf("JUMPIFNEQ %%for%d LF@%%len int@0\n", n);
    printf("LABEL %%returnnull%d\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@%%exprresult TF@%%substring%%%d%%retval\n", n);
    n++;
}

void genFuncDef1(char *funcname, int parCount, LinkedList ll)
{

    printf("JUMP %%jump%%over%%%s\n", funcname);
    printf("LABEL %%func%%%s\n", funcname);
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@%s%%retval\n", funcname);
    ListNode *tmp = ll.head;
    fprintf(stderr, "%i\n", parCount);
    for (size_t i = parCount; i > 0; i--)
    {
        printf("DEFVAR TF@%s\n", tmp->name);
        printf("POPS TF@%s\n", tmp->name);
        tmp = tmp->next;
    }
    tmp = ll.head;
    for (size_t i = parCount; i > 0; i--)
    {
        char *matchingType = "";
        if (tmp->type == 1)
        {
            matchingType = "float";
        }
        else if (tmp->type == 4)
        {
            matchingType = "int";
        }
        else if (tmp->type == 7)
        {
            matchingType = "string";
        }
        printf("TYPE GF@%%corrtype%%1 TF@%s\n", tmp->name);
        printf("MOVE GF@%%corrtype%%2 string@%s\n", matchingType);
        printf("MOVE GF@%%corrtype%%opt int@%d\n", tmp->opt);
        printf("CALL %%corrtype%%func\n");
        tmp = tmp->next;
    }
    printf("PUSHFRAME\n");
}

void genFuncDef2(char *funcname)
{
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL %%jump%%over%%%s\n", funcname);
}

void genFuncCall(char *funcname, int paramCount, ListNode *returnType)
{
    static int retnum = 0;
    if (strcmp(funcname, "write") == 0)
    {
        generateWrite(paramCount);
    }
    else if (strcmp(funcname, "strlen") == 0)
    {
        genStrLen();
    }
    else if (strcmp(funcname, "ord") == 0)
    {
        genOrd();
    }
    else if (strcmp(funcname, "chr") == 0)
    {
        genChr();
    }
    else if (strcmp(funcname, "reads") == 0)
    {
        genReads();
    }
    else if (strcmp(funcname, "readi") == 0)
    {
        genReadi();
    }
    else if (strcmp(funcname, "readf") == 0)
    {
        genReadf();
    }
    else if (strcmp(funcname, "floatval") == 0)
    {
        genFloatval();
    }
    else if (strcmp(funcname, "intval") == 0)
    {
        genIntval();
    }
    else if (strcmp(funcname, "strval") == 0)
    {
        genStrval();
    }
    else if (strcmp(funcname, "substring") == 0)
    {
        genSubstring();
    }
    else
    {
        printf("CALL %%func%%%s\n", funcname);
        if (returnType->type != KW_VOID)
        {
            printf("TYPE GF@%%corrtype%%1 TF@%s%%retval\n", funcname);
            printf("JUMPIFEQ %%hasnil%d GF@%%corrtype%%1 string@nil\n", retnum);
            printf("JUMPIFNEQ %%hasretvalue%d GF@%%corrtype%%1 string@\n", retnum);
            printf("LABEL %%hasnil%d\n", retnum);
            if (returnType->opt == true)
            {
                printf("JUMPIFEQ %%hasretvalue%d GF@%%corrtype%%1 string@nil\n", retnum);
            }
            printf("EXIT int@4\n");
            printf("LABEL %%hasretvalue%d\n", retnum);
            printf("MOVE GF@%%exprresult TF@%s%%retval\n", funcname);
        }
    }
    retnum++;
}

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
}

void genReturn(char *funcname, bool expr)
{
    (void)expr;
    if (strcmp(funcname, "main") != 0)
    {
        printf("MOVE LF@%s%%retval GF@%%exprresult\n", funcname);
        printf("POPFRAME\n");
        printf("RETURN\n");
    }
    else
    {
        printf("EXIT int@0\n");
    }
}
