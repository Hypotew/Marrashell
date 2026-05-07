/*
** EPITECH PROJECT, 2026
** string_array.c
** File description:
** NULL-terminated string array utilities
*/

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
    for (size_t i = 0; i <= len; ++i)
        copy[i] = NULL;
    for (size_t i = 0; i < len; ++i) {
        copy[i] = strdup(array[i]);
        if (copy[i] == NULL) {
            free_string_array(copy);
            return NULL;
        }
    }
    return copy;
}

static size_t global_len_array(char **array, size_t start, size_t nb_words)
{
    size_t len = 1;

    for (size_t i = start; i < nb_words; i++) {
        len += strlen(array[i]);
        if (i + 1 < nb_words)
            len++;
    }
    return len;
}

char *array_to_string(char **array, size_t start)
{
    char *str = NULL;
    size_t nb_words = 0;
    size_t space_to_allocate = 0;

    if (array == NULL)
        return NULL;
    nb_words = string_array_len(array);
    if (start >= nb_words)
        return NULL;
    space_to_allocate = global_len_array(array, start, nb_words);
    str = malloc(sizeof(char) * space_to_allocate);
    if (!str)
        return NULL;
    str[0] = '\0';
    for (size_t y = start; y < nb_words; y++) {
        strcat(str, array[y]);
        if (y + 1 < nb_words)
            strcat(str, " ");
    }
    return str;
}
