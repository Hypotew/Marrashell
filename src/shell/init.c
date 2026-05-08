/*
** EPITECH PROJECT, 2026
** init.c
** File description:
** initialize shell state
*/

#include "shell.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <limits.h>
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

static void init_special_vars(shell_t *shell)
{
    char cwd[PATH_MAX];
    char *term = env_get_value(shell->env, "TERM=");

    if (getcwd(cwd, PATH_MAX) != NULL)
        local_set_value(shell, "cwd", cwd);
    if (term)
        local_set_value(shell, "term", term);
}

static void shell_cleanup_partial(shell_t *shell)
{
    free_string_array(shell->env);
    free(shell->locals);
    free(shell->aliases);
    shell->env = NULL;
    shell->locals = NULL;
    shell->aliases = NULL;
}

bool shell_init(shell_t *shell, char **envp)
{
    shell->env = dup_string_array(envp);
    shell->locals = NULL;
    shell->aliases = NULL;
    if (shell->env == NULL)
        return false;
    shell->locals = calloc(1, sizeof(char *));
    shell->aliases = calloc(1, sizeof(char *));
    if (!shell->locals || !shell->aliases) {
        shell_cleanup_partial(shell);
        return false;
    }
    shell->last_status = SUCCESS_EXIT;
    shell->line = NULL;
    shell->last_pid = -1;
    init_special_vars(shell);
    return true;
}

void shell_destroy(shell_t *shell)
{
    if (shell == NULL)
        return;
    free_string_array(shell->env);
    free_string_array(shell->locals);
    free_string_array(shell->aliases);
    free(shell->line);
    shell->env = NULL;
    shell->locals = NULL;
    shell->aliases = NULL;
    shell->line = NULL;
}
