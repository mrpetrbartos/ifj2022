/**
 * @file stack.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for stack structure.
 */

#ifndef H_STACK
#define H_STACK

#include "scanner.h"

typedef struct StackItem
{
    Token t;
    struct StackItem *next;
} StackItem;

typedef struct
{
    StackItem *head;
    unsigned len;
} Stack;

/**
 * @brief Initializes the stack structure.
 *
 * @param s Pointer to a stack.
 */
void stackInit(Stack *s);

/**
 * @brief Pushes a token to the top of the stack.
 *
 * @param s Pointer to a stack.
 * @param t Token to push.
 */
void stackPush(Stack *s, Token t);

/**
 * @brief Pops a token from the top of the stack.
 *
 * @param s Pointer to a stack.
 */
void stackPop(Stack *s);

/**
 * @brief Removes all tokens from the stack.
 *
 * @param s Pointer to a stack.
 */
void stackClear(Stack *s);

#endif
