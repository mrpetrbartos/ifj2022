/**
 * @file structures.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for stack and linked list structures.
 */

#ifndef H_STRUCTURES
#define H_STRUCTURES

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

typedef struct ListNode
{
    Keyword type;
    struct ListNode *next;
} ListNode;

typedef struct
{
    struct ListNode *head;
    struct ListNode *last;
    int itemCount;
} LinkedList;

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
 * @param t Variable to pop the token to (can be NULL).
 */
void stackPop(Stack *s, Token *t);

/**
 * @brief Removes all tokens from the stack.
 *
 * @param s Pointer to a stack.
 */
void stackFree(Stack *s);

/**
 * @brief Initializes linked list.
 *
 */
void listInit(LinkedList *l);

/**
 * @brief Inserts an item to linked list.
 *
 * @param l Pointer to the list.
 * @param type Type of the parameter.
 * @return int Non-zero value in case of error, zero otherwise.
 */
int listInsert(LinkedList *l, Keyword type);

/**
 * @brief Frees the space allocated by a list.
 *
 * @param l The list to dispose.
 */
void listDispose(LinkedList *l);

#endif
