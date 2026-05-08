/*
** EPITECH PROJECT, 2026
** loop.c
** File description:
** prompt/read/parse/execute loop
*/

#include "mysh.h"
#include "exec.h"
#include "shell.h"
#include "builtins.h"
#include "readline.h"
#include "expand.h"
#include "env.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

static void run_hook(shell_t *shell, const char *name)
{
    char *val = local_get_value(shell->locals, name);

    if (val && val[0] != '\0')
        run_command(shell, val);
}

static bool handle_eof(shell_t *shell, bool is_interactive)
{
    if (!is_interactive)
        return false;
    if (!local_get_value(shell->locals, "ignoreof"))
        return false;
    fprintf(stderr, "\nUse \"exit\" to leave %s.\n", "42sh");
    return true;
}

static bool read_input(shell_t *shell, bool is_interactive)
{
    size_t cap = 0;

    if (is_interactive) {
        free(shell->line);
        shell->line = read_line(shell);
        if (shell->line != NULL)
            return true;
        return handle_eof(shell, is_interactive);
    }
    free(shell->line);
    shell->line = NULL;
    return getline(&shell->line, &cap, stdin) != -1;
}

static enum shell_status process_line(shell_t *shell)
{
    char *expanded = expand_line(shell);
    enum shell_status status;
    char *save;

    if (!expanded)
        return SHELL_CONTINUE;
    status = run_command(shell, expanded);
    save = shell->line;
    shell->line = expanded;
    add_to_history(shell);
    shell->line = save;
    free(expanded);
    return status;
}

int shell_loop(shell_t *shell)
{
    enum shell_status status = SHELL_CONTINUE;
    bool is_interactive = isatty(STDIN_FILENO);

    if (display_marrashell() == FAILURE_EXIT)
        return FAILURE_EXIT;
    while (status == SHELL_CONTINUE) {
        run_hook(shell, "precmd");
        if (!read_input(shell, is_interactive))
            break;
        status = process_line(shell);
    }
    return shell->last_status;
}
