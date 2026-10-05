/*
** EPITECH PROJECT, 2026
** where.c
** File description:
** where builtin
*/

#include "builtins.h"
#include "mysh.h"

#include <stdio.h>

int my_where(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    if (argv[1] == NULL) {
        if (fprintf(stderr, "%s", "where: Too few arguments.\n") < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    for (int i = 1; argv[i] != NULL; i++)
        if (print_path_matches(shell, argv[i], true, false) == FAILURE_EXIT)
            return FAILURE_EXIT;
    return SUCCESS_EXIT;
}
