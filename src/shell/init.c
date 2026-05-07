/*
** EPITECH PROJECT, 2026
** init.c
** File description:
** initialize shell state
*/

#include "shell.h"
#include "mysh.h"
#include "utils.h"

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static const char *const BANNER[] = {
    "┌────────────"
    "────────────"
    "────────────"
    "────────────"
    "────────────"
    "────┐\n",
    "│                                                                │\n",
    "│     __    __     ______     ______     ______     ______       │\n",
    "│    /\\ \"-./  \\   /\\  __ \\   /\\  == \\   /\\  == \\   "
    "/\\  __ \\      │\n",
    "│    \\ \\ \\-./\\ \\  \\ \\  __ \\  \\ \\  __<   \\ \\  __<   "
    "\\ \\  __ \\     │\n",
    "│     \\ \\_\\ \\ \\_\\  \\ \\_\\ \\_\\  "
    "\\ \\_\\ \\_\\  \\ \\_\\ \\_\\  \\ \\_\\ \\_\\    │\n",
    "│      \\/_/  \\/_/   \\/_/\\/_/   \\/_/ /_/   \\/_/ /_/   "
    "\\/_/\\/_/    │\n",
    "│                                                                │\n",
    "│     ______     __  __     ______     __         __             │\n",
    "│    /\\  ___\\   /\\ \\_\\ \\   /\\  ___\\   /\\ \\       "
    "/\\ \\            │\n",
    "│    \\ \\___  \\  \\ \\  __ \\  \\ \\  __\\   \\ \\ \\____  "
    "\\ \\ \\____       │\n",
    "│     \\/\\_____\\  \\ \\_\\ \\_\\  \\ \\_____\\  \\ \\_____\\  "
    "\\ \\_____\\      │\n",
    "│      \\/_____/   \\/_/\\/_/   \\/_____/   \\/_____/   "
    "\\/_____/      │\n",
    "│                                                                │\n",
    "└────────────"
    "────────────"
    "────────────"
    "────────────"
    "────────────"
    "────┘\n",
    NULL
};

int display_marrashell(void)
{
    for (size_t i = 0; BANNER[i] != NULL; i++)
        if (printf("%s", BANNER[i]) < 0)
            return FAILURE_EXIT;
    if (printf("\n\n") < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

int display_prompt(int last_status)
{
    char cwd[4096];

    if (is_in_repository() == true)
        if (display_branch() == FAILURE_EXIT)
            return FAILURE_EXIT;
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return FAILURE_EXIT;
    if (printf("%s ", cwd) < 0)
        return FAILURE_EXIT;
    if (printf("%s", last_status == 0 ? SUCCESS_PROMPT : FAILURE_PROMPT) < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

bool shell_init(shell_t *shell, char **envp)
{
    shell->env = dup_string_array(envp);
    if (shell->env == NULL)
        return false;
    shell->locals = calloc(1, sizeof(char *));
    if (shell->locals == NULL)
        return false;
    shell->aliases = calloc(1, sizeof(char *));
    if (shell->aliases == NULL)
        return false;
    shell->last_status = SUCCESS_EXIT;
    shell->line = NULL;
    shell->last_pid = -1;
    return true;
}
