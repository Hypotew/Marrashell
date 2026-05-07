/*
** EPITECH PROJECT, 2026
** echo.c
** File description:
** echo builtin
*/

#include "builtins.h"
#include "mysh.h"
#include "shell.h"
#include <stdio.h>

int my_echo(__attribute__((unused)) shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    for (int i = 1; argv[i] != NULL; i++) {
        if (i > 1 && printf(" ") < 0)
            return FAILURE_EXIT;
        if (printf("%s", argv[i]) < 0)
            return FAILURE_EXIT;
    }
    if (printf("\n") < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}
