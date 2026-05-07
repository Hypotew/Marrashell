/*
** EPITECH PROJECT, 2026
** dispatch.c
** File description:
** map argv[0] -> builtin function
*/

#include "builtins.h"
#include "shell.h"

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

static const builtin_entry_t g_builtins[] = {
    {"exit", shell_exit},
    {"cd", my_cd},
    {"setenv", my_setenv},
    {"unsetenv", my_unsetenv},
    {"env", my_env},
    {"echo", echo_last_status},
    {"history", my_history},
    {"repeat", my_repeat},
    {NULL, NULL},
};

static bool is_builtin(char **argv)
{
    for (size_t i = 0; g_builtins[i].name != NULL; ++i) {
        if (strcmp(argv[0], g_builtins[i].name) == 0)
            return true;
    }
    return false;
}

bool run_builtin(shell_t *shell, char **argv, bool *should_exit)
{
    if (argv == NULL || argv[0] == NULL)
        return true;
    if (!is_builtin(argv))
        return false;
    for (size_t i = 0; g_builtins[i].name != NULL; ++i) {
        if (strcmp(argv[0], g_builtins[i].name) != 0)
            continue;
        shell->last_status = g_builtins[i].fn(shell, argv, should_exit);
        return true;
    }
    return true;
}
