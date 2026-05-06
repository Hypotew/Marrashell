/*
** EPITECH PROJECT, 2026
** echo_last_status.c
** File description:
** This file contains all functions to handle echo $? in minishell.
*/

#include "builtins.h"
#include "mysh.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>

static int print_echo_arg(shell_t *shell, const char *arg)
{
    if (strcmp(arg, "$?") == 0)
        return printf("%d", shell->last_status) < 0 ?
            FAILURE_EXIT : SUCCESS_EXIT;
    if (printf("%s", arg) < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

int echo_last_status(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    for (int i = 1; argv[i] != NULL; i++) {
        if (i > 1 && printf(" ") < 0)
            return FAILURE_EXIT;
        if (print_echo_arg(shell, argv[i]) == FAILURE_EXIT)
            return FAILURE_EXIT;
    }
    if (printf("\n") < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}
