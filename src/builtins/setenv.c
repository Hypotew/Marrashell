/*
** EPITECH PROJECT, 2026
** setenv.c
** File description:
** setenv replica
*/

#include "builtins.h"
#include "mysh.h"
#include "env.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

int my_setenv(shell_t *shell, char **argv, bool *should_exit)
{
    size_t arg_nb = string_array_len(argv);

    if (arg_nb > 3) {
        fprintf(stderr, "%s", "setenv: Too much arguments.\n");
        return FAILURE_EXIT;
    }
    if (arg_nb == 1) {
        my_env(shell, argv, should_exit);
        return SUCCESS_EXIT;
    }
    if (!valid_env_key(argv[1])) {
        fprintf(stderr, "%s",
            "setenv: Variable name must contain"
            " alphanumeric characters.\n");
        return SUCCESS_EXIT;
    }
    if (env_set_value(shell, argv[1], argv[2]) != SUCCESS_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}
