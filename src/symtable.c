/**
 * @file symtable.c
 * @author Petr Bartoš (xbarto0g)
 * @brief Implementation of symtable.
 */

#include <stdlib.h>
#include <stdint.h>
#include "symtable.h"

size_t symtableHash(const char *str)
{
    uint32_t h = 0;
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

Symtable *symtableInit(size_t n)
{
    Symtable *table = malloc(sizeof(Symtable));
    if (table == NULL)
        return NULL;

    table->size = 0;
    table->arr_size = n;
    table->arr_ptr = malloc(sizeof(SymtableItem *) * n);
    if (table->arr_ptr == NULL)
    {
        free(table);
        return NULL;
    }

    for (size_t i = 0; i < n; i++)
    {
        table->arr_ptr[i] = NULL;
    }

    return table;
}

SymtablePair *symtableFind(Symtable *t, SymtableKey key)
{
    if (t == NULL)
        return NULL;

    size_t hash = symtableHash(key) % t->arr_size;
    SymtableItem *tmp = t->arr_ptr[hash];

    while (tmp != NULL)
    {
        if (!strcmp(key, tmp->pair.key))
        {
            return &(tmp->pair);
        }
        tmp = tmp->next;
    }
    return NULL;
}

SymtablePair *symtableAdd(Symtable *t, SymtableKey key, ElType type, int paramsCnt, bool undefined, LinkedList params)
{
    if (t == NULL)
        return NULL;

    SymtablePair *found = symtableFind(t, key);

    if (found != NULL)
        return found;

    SymtableItem *newitem = malloc(sizeof(SymtableItem));
    if (newitem == NULL)
    {
        return NULL;
    }

    size_t len = strlen(key);
    newitem->pair.key = calloc(len + 1, 1);
    if (newitem->pair.key == NULL)
    {
        free(newitem);
        return NULL;
    }

    memcpy((char *)newitem->pair.key, key, len);
    newitem->pair.data.type = type;
    newitem->pair.data.paramsCnt = paramsCnt;
    newitem->pair.data.possiblyUndefined = undefined;
    newitem->pair.data.parameters = params;
    newitem->next = NULL;
    t->size++;

    size_t index = symtableHash(key) % t->arr_size;
    SymtableItem *tmp = t->arr_ptr[index];

    if (tmp == NULL)
        t->arr_ptr[index] = newitem;
    else
    {
        while (tmp->next != NULL)
            tmp = tmp->next;

        tmp->next = newitem;
    }

    return &(newitem->pair);
}

void symtableFree(Symtable *t)
{
    if (t == NULL)
        return;

    SymtableItem *curr, *next;

    for (size_t i = 0; i < t->arr_size; i++)
    {
        curr = t->arr_ptr[i];
        while (curr != NULL)
        {
            next = curr->next;
            free((char *)curr->pair.key);
            free(curr);
            curr = next;
        }
        t->arr_ptr[i] = NULL;
    }
    t->size = 0;

    free(t->arr_ptr);
    free(t);
}
