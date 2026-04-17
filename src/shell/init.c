/*
** EPITECH PROJECT, 2026
** init.c
** File description:
** initialize shell state
*/

#include "shell.h"
#include "mysh.h"
#include "my.h"

#include <stdbool.h>
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

void display_marrashell(void)
{
    for (size_t i = 0; BANNER[i] != NULL; i++)
        printf("%s", BANNER[i]);
    printf("\n\n");
}

bool shell_init(shell_t *shell, char **envp)
{
    shell->env = dup_string_array(envp);
    if (shell->env == NULL)
        return false;
    shell->last_status = SUCCESS_EXIT;
    shell->line = NULL;
    shell->last_pid = -1;
    return true;
}
