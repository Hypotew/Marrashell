/*
** EPITECH PROJECT, 2026
** unsetenv.c
** File description:
** unsetenv replica
*/

#include "builtins.h"
#include "mysh.h"
#include "env.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

int my_unsetenv(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    size_t arg_nb = string_array_len(argv);
    int value_index = -1;

    if (arg_nb == 1) {
        fprintf(stderr, "%s", "unsetenv: Too few arguments.\n");
        return FAILURE_EXIT;
    }
    for (size_t i = 1; i < arg_nb - 1; ++i) {
        value_index = env_get_value_index(shell->env, argv[i]);
        if (value_index == -1) {
            continue;
        }
        free(shell->env[value_index]);
        for (size_t j = value_index; shell->env[j] != NULL; ++j) {
            shell->env[j] = shell->env[j + 1];
        }
    }
    return SUCCESS_EXIT;
}
