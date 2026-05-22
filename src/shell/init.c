/*
** EPITECH PROJECT, 2026
** init.c
** File description:
** initialize shell state + TUI
*/

#include "shell.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"
#include "tui.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static void init_special_vars(shell_t *shell)
{
    char cwd[PATH_MAX];
    char *term = env_get_value(shell->env, "TERM=");

    if (getcwd(cwd, PATH_MAX) != NULL)
        local_set_value(shell, "cwd", cwd);
    if (term)
        local_set_value(shell, "term", term);
}

static void load_rc_file(shell_t *shell)
{
    char path[512];
    char line[256];
    const char *home = getenv("HOME");
    FILE *f;

    if (!home)
        return;
    snprintf(path, sizeof(path), "%s/.42shrc", home);
    f = fopen(path, "r");
    if (!f)
        return;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "set theme=", 10) == 0) {
            line[strcspn(line, "\n")] = '\0';
            local_set_value(shell, "theme", line + 10);
        }
    }
    fclose(f);
}

static void init_tui(shell_t *shell)
{
    char *theme_name = local_get_value(shell->locals, "theme");
    int theme_id = 0;

    if (theme_name)
        theme_id = tui_get_theme_by_name(theme_name);
    if (!isatty(STDIN_FILENO))
        return;
    shell->tui = tui_init(theme_id < 0 ? 0 : theme_id);
    if (shell->tui)
        tui_register_resize_handler(shell->tui);
}

bool shell_init(shell_t *shell, char **envp)
{
    shell->env = dup_string_array(envp);
    shell->locals = calloc(1, sizeof(char *));
    shell->aliases = calloc(1, sizeof(char *));
    if (!shell->env || !shell->locals || !shell->aliases) {
        free_string_array(shell->env);
        free(shell->locals);
        free(shell->aliases);
        return false;
    }
    shell->last_status = 0;
    shell->line = NULL;
    shell->last_pid = -1;
    shell->tui = NULL;
    init_special_vars(shell);
    load_rc_file(shell);
    init_tui(shell);
    return true;
}

void shell_destroy(shell_t *shell)
{
    if (!shell)
        return;
    free_string_array(shell->env);
    free_string_array(shell->locals);
    free_string_array(shell->aliases);
    free(shell->line);
    tui_destroy(shell->tui);
    shell->env = NULL;
    shell->locals = NULL;
    shell->aliases = NULL;
    shell->line = NULL;
    shell->tui = NULL;
}
