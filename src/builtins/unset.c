/*
** EPITECH PROJECT, 2026
** unset.c
** File description:
** unset builtin for local variables
*/

#include "builtins.h"
#include "env.h"
#include "mysh.h"

#include <stdio.h>

int my_unset(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    if (argv[1] == NULL) {
        fprintf(stderr, "unset: Too few arguments.\n");
        return FAILURE_EXIT;
    }
    for (int i = 1; argv[i] != NULL; i++)
        local_unset(shell, argv[i]);
    return SUCCESS_EXIT;
}
