/**
 * @file codegen.h
 * @author Lukáš Zedek (xzedek03)
 * @brief Header file for code generator.
 */

#ifndef H_CODEGEN
#define H_CODEGEN

#include "scanner.h"
#include "parser.h"

/**
 * @brief Prints info about used lang.
 *
 */
void genPrintHead();

/**
 * @brief Pushes token's value to stack.
 *
 * @param t Token whose value should be pushed.
 */
void genStackPush(Token t);

void genMathInstCheck();

void genExpressionBegin();

void genExpressionEnd();

void genCheckTruth();

void genIfElse1();

void genIfElse2();

void genIfElse3();

void genWhileLoop1();

void genWhileLoop2();

void genWhileLoop3();

void genDefineVariable(Token t);

void genAssignVariable(Token t);

void genCheckDefined(Token t);

void genFuncDef1(char *funcname, int parCount, LinkedList ll);

void genFuncDef2(char *funcname);

void genFuncCall(char *funcname, int paramCount);

void genPushString(char *str);

#endif
