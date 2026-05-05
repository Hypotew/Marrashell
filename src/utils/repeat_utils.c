/*
** EPITECH PROJECT, 2025
** repeat_utils.c
** File description:
** Utils functions for repeat builtin.
*/

#include "builtins.h"
#include "mysh.h"
#include <stdlib.h>
#include <limits.h>

static int add_digit(unsigned int *value, char digit)
{
    *value = *value * 10 + (digit - '0');
    if (*value > UINT_MAX)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int parse_digits(char const *str, unsigned int *value)
{
    char const *start = str;

    for (; *str >= '0' && *str <= '9'; str++)
        if (add_digit(value, *str) == FAILURE_EXIT)
            return FAILURE_EXIT;
    if (str == start)
        return FAILURE_EXIT;
    if (*str != '\0')
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

int get_positive_nbr(char const *str, unsigned int *out)
{
    unsigned int value = 0;

    if (!str || !out)
        return FAILURE_EXIT;
    if (parse_digits(str, &value) == FAILURE_EXIT)
        return FAILURE_EXIT;
    *out = (unsigned int)(value);
    return SUCCESS_EXIT;
}
