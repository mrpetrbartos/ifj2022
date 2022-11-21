/**
 * @file stack.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Implementation of the stack structure.
 */

#include "stack.h"

void stackInit(Stack *s)
{
    s->head = NULL;
    s->len = 0;
}

void stackPush(Stack *s, Token t)
{
    if (s == NULL)
        return;

    StackItem *newItem = malloc(sizeof(StackItem));
    if (newItem == NULL)
        return;

    newItem->t = t;
    newItem->next = s->head;
    s->head = newItem;
    ++(s->len);
}

void stackPop(Stack *s, Token *t)
{
    if (s == NULL || s->head == NULL)
        return;

    StackItem *tmp = s->head;
    if (t != NULL)
        *t = tmp->t;
    s->head = s->head->next;
    free(tmp);
    --(s->len);
}

void stackFree(Stack *s)
{
    if (s == NULL)
        return;

    StackItem *tmp = s->head;
    while (tmp != NULL)
    {
        tmp = s->head->next;
        free(s->head);
        s->head = tmp;
    }

    s->len = 0;
}
