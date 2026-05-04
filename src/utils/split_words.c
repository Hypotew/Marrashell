/*
** EPITECH PROJECT, 2026
** split_words.c
** File description:
** split string by delimiter characters
*/

#include "utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static size_t count_words(const char *str, const char *delims)
{
    size_t count = 0;
    bool in_word = false;

    for (size_t i = 0; str[i] != '\0'; ++i) {
        if (strchr(delims, str[i]))
            in_word = false;
        if (!strchr(delims, str[i]) && !in_word) {
            in_word = true;
            ++count;
        }
    }
    return count;
}

static char **fill_words(char *copy, const char *delims, char **result)
{
    char *token = strtok(copy, delims);
    size_t i = 0;

    while (token != NULL) {
        result[i] = strdup(token);
        if (result[i] == NULL) {
            free_string_array(result);
            return NULL;
        }
        ++i;
        token = strtok(NULL, delims);
    }
    result[i] = NULL;
    return result;
}

char **my_split_words(char const *str, char const *delimiters)
{
    char *copy = strdup(str);
    size_t word_count = count_words(str, delimiters);
    char **result = malloc(sizeof(char *) * (word_count + 1));

    if (copy == NULL || result == NULL) {
        free(copy);
        free(result);
        return NULL;
    }
    result = fill_words(copy, delimiters, result);
    free(copy);
    return result;
}
