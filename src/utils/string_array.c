/*
** EPITECH PROJECT, 2026
** string_array.c
** File description:
** NULL-terminated string array utilities
*/

#include "my.h"

#include <stdlib.h>
#include <string.h>

size_t string_array_len(char **array)
{
    size_t len = 0;

    if (array == NULL)
        return 0;
    while (array[len] != NULL)
        ++len;
    return len;
}

void free_string_array(char **array)
{
    if (array == NULL)
        return;
    for (size_t i = 0; array[i] != NULL; ++i)
        free(array[i]);
    free(array);
}

char **dup_string_array(char **array)
{
    size_t len = string_array_len(array);
    char **copy = malloc(sizeof(char *) * (len + 1));

    if (copy == NULL)
        return NULL;
    for (size_t i = 0; i < len; ++i) {
        copy[i] = strdup(array[i]);
        if (copy[i] == NULL) {
            free_string_array(copy);
            return NULL;
        }
    }
    copy[len] = NULL;
    return copy;
}
