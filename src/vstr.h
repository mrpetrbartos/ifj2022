/**
 * @file vstr.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for custom variable-length string.
 */

#ifndef H_VSTR
#define H_VSTR

#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_SIZE 128

typedef struct
{
    char *content;
    int length;
    int maxLength;
} vStr;

/**
 * @brief Initializes variable-length string.
 *
 * @param str The string to initialize.
 * @return True if the initialization was successful, false if not.
 */
bool vStrInit(vStr *str);

/**
 * @brief Deallocates memory used by the string.
 *
 * @param str The string to deallocate.
 */
void vStrFree(vStr *str);

/**
 * @brief Appends a character to the end of the string.
 *
 * Should the length of the string after appending the character
 * be larger than maxLength, the length is doubled.
 *
 * @param str The string to modify.
 * @param character Character which will be appended.
 * @return True if the character was appended, false if not.
 */
bool vStrAppend(vStr *str, char character);

#endif
