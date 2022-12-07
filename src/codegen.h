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

/**
 * @brief Prints premade match checking function-
 *
 */
void genMathInstCheck();

/**
 * @brief Prepares expression evaluation.
 *
 */
void genExpressionBegin();

/**
 * @brief Ends expression evaluation.
 *
 */
void genExpressionEnd();

/**
 * @brief Premade function to check truthiness of statement.
 *
 */
void genCheckTruth();

/**
 * @brief Prepares beginning of if statement.
 *
 * @param num Unique identifier of the statement.
 */
void genIfElse1(int num);

/**
 * @brief Prepares beginning of else statement.
 *
 * @param num Unique identifier of the statement.
 */
void genIfElse2(int num);

/**
 * @brief Ending of if-else statement.
 *
 * @param num Unique identifier of the statement.
 */
void genIfElse3(int num);

/**
 * @brief Prepares beginning of while statement.
 *
 * @param num Unique identifier of the statement.
 */
void genWhileLoop1(int num);

/**
 * @brief While cycle validity check.
 *
 * @param num Unique identifier of the statement.
 */
void genWhileLoop2(int num);

/**
 * @brief Ending of while cycle.
 *
 * @param num Unique identifier of the statement.
 */
void genWhileLoop3(int num);

/**
 * @brief Generates variable definition.
 *
 * @param t Token using which will be the definition made.
 */
void genDefineVariable(Token t);

/**
 * @brief Assigns a value to a variable.
 *
 * @param t Token using which will be the assignment made.
 */
void genAssignVariable(Token t);

/**
 * @brief Check whether the variable was defined.
 *
 * @param t Token using which bears the name of the variable.
 */
void genCheckDefined(Token t);

/**
 * @brief Generates head of function definition.
 *
 * @param funcname Name of the function.
 * @param parCount Number of parameters.
 * @param ll List of parameters and their types.
 */
void genFuncDef1(char *funcname, int parCount, LinkedList ll);

/**
 * @brief Generates bottom of function definition.
 *
 * @param funcname Name of the function.
 */
void genFuncDef2(char *funcname);

/**
 * @brief Generates a function call.
 *
 * @param funcname Name of the function.
 * @param paramCount Number of parameters.
 * @param returnType Expected return type.
 */
void genFuncCall(char *funcname, int paramCount, ListNode *returnType);

/**
 * @brief Generates return statement.
 *
 * @param funcname Name of the current function.
 * @param expr Is the return followed by an expression.
 */
void genReturn(char *funcname, bool expr);

/**
 * @brief Pushes a string to a stack.
 *
 * @param str The string to push.
 */
void genPushString(char *str);

#endif
