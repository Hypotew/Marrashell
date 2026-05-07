/*
** EPITECH PROJECT, 2026
** alias.c
** File description:
** alias builtin
*/

#include "builtins.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int print_one_alias(shell_t *shell, const char *name)
{
    char *val = alias_get_value(shell->aliases, name);

    if (val)
        printf("%s\t(%s)\n", name, val);
    return SUCCESS_EXIT;
}

static int set_alias(shell_t *shell, char **argv)
{
    char *value = array_to_string(argv, 2);

    if (!value)
        return FAILURE_EXIT;
    if (alias_set_value(shell, argv[1], value) != SUCCESS_EXIT) {
        free(value);
        return FAILURE_EXIT;
    }
    free(value);
    return SUCCESS_EXIT;
}

int my_alias(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    size_t argc = string_array_len(argv);

    if (argc == 1) {
        alias_print_all(shell->aliases);
        return SUCCESS_EXIT;
    }
    if (argc == 2)
        return print_one_alias(shell, argv[1]);
    return set_alias(shell, argv);
}
