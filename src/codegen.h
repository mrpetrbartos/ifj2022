/**
 * @file codegen.h
 * @author Lukáš Zedek (xzedek03)
 * @brief Header file for code generator.
 */

#ifndef H_CODEGEN
#define H_CODEGEN

#include "scanner.h"

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

void genExpressionBegin();

void genExpressionEnd();

#endif
