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

void genIfElse1(int num);

void genIfElse2(int num);

void genIfElse3(int num);

void genWhileLoop1(int num);

void genWhileLoop2(int num);

void genWhileLoop3(int num);

void genDefineVariable(Token t);

void genAssignVariable(Token t);

void genCheckDefined(Token t);

void genFuncDef1(char *funcname, int parCount, LinkedList ll);

void genFuncDef2(char *funcname);

void genFuncCall(char *funcname, int paramCount, Keyword returnType);

void genPushString(char *str);

#endif
