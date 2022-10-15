/**
 * @file vstr.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Implementation of custom variable-length string.
 */

#include "vstr.h"

bool vStrInit(vStr *str)
{
    str->content = malloc(INITIAL_SIZE * sizeof(char)); // sizeof(char) could be ommited, left for clarity
    if (str->content == NULL)
        return false;

    str->content[0] = '\0';
    str->length = 0;
    str->maxLength = INITIAL_SIZE;

    return true;
}

void vStrFree(vStr *str)
{
    free(str->content);
}

bool vStrAppend(vStr *str, char character)
{
    if (str->length + 1 >= str->maxLength)
    {
        char *tempContent = realloc(str->content, 2 * str->maxLength);
        if (tempContent == NULL)
            return false;

        str->content = tempContent;
    }

    str->content[str->length] = character;
    str->length++;
    str->content[str->length] = '\0';

    return true;
}
