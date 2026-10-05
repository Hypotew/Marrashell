/*
** EPITECH PROJECT, 2026
** unalias.c
** File description:
** unalias builtin
*/

#include "builtins.h"
#include "env.h"
#include "mysh.h"

#include <stdio.h>
#include <string.h>

int my_unalias(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    if (argv[1] == NULL) {
        if (fprintf(stderr, "unalias: Too few arguments.\n") < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    for (int i = 1; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "*") == 0)
            alias_unset_all(shell);
        else
            alias_unset(shell, argv[i]);
    }
    return SUCCESS_EXIT;
}
