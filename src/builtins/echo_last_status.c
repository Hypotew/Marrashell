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

int echo_last_status(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    for (int i = 1; argv[i] != NULL; i++) {
        if (i > 1)
            printf(" ");
        if (strcmp(argv[i], "$?") == 0)
            printf("%d", shell->last_status);
        else
            printf("%s", argv[i]);
    }
    printf("\n");
    return SUCCESS_EXIT;
}
