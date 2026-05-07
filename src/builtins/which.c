/*
** EPITECH PROJECT, 2026
** which.c
** File description:
** which builtin
*/

#include "builtins.h"
#include "mysh.h"

#include <stdio.h>

int my_which(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    int status = SUCCESS_EXIT;

    if (argv[1] == NULL) {
        if (fprintf(stderr, "%s", "which: Too few arguments.\n") < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    for (int i = 1; argv[i] != NULL; i++)
        if (print_path_matches(shell, argv[i], false, true) == FAILURE_EXIT)
            status = FAILURE_EXIT;
    return status;
}
