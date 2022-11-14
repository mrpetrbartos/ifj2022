/**
 * @file symtable.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for symtable.
 * Reusing symtable implementation from IJC course
 * taken by Petr Bartoš.
 */

#ifndef H_SYMTABLE
#define H_SYMTABLE

#include <string.h>
#include <stdbool.h>

typedef const char *SymtableKey;

typedef enum
{
    FUNC,
    VAR
} ElType;

typedef struct SymtableData
{
    ElType type;
    int paramsCnt;
} SymtableData;

typedef struct SymtablePair
{
    SymtableKey key;
    SymtableData data;
} SymtablePair;

typedef struct SymtableItem
{
    SymtablePair pair;
    struct SymtableItem *next;
} SymtableItem;

typedef struct Symtable
{
    size_t size;
    size_t arr_size;
    SymtableItem **arr_ptr;
} Symtable;

/**
 * @brief Calculates hash given a key.
 *
 * @param key Key to calculate hash for.
 * @return size_t Hash of supplemented key.
 */
size_t symtableHash(SymtableKey key);

/**
 * @brief Initializes a symtable.
 *
 * @param n Size of pointer array.
 * @return symtable* Pointer to the initialized symtable.
 */
Symtable *symtableInit(size_t n);

/**
 * @brief Searches for a key in a symtable.
 *
 * @param t The symtable to search in.
 * @param key The key to look for.
 * @return SymtablePair* Found item.
 */
SymtablePair *symtableFind(Symtable *t, SymtableKey key);

/**
 * @brief Adds a new element to a symtable.
 *
 * @param t The symtable to add the element to.
 * @param key Key of the element.
 * @param type Type of the element.
 * @param paramsCnt Number of parameters (only for functions).
 * @return SymtablePair* Pointer to the added element.
 */
SymtablePair *symtableAdd(Symtable *t, SymtableKey key, ElType type, int paramsCnt);

/**
 * @brief Frees a symtable.
 *
 * @param t The symtable to free.
 */
void symtableFree(Symtable *t);

#endif
