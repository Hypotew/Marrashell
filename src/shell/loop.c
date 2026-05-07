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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

static bool read_input(shell_t *shell, bool is_interactive)
{
    size_t cap = 0;
    ssize_t nread;

    if (is_interactive) {
        free(shell->line);
        shell->line = read_line(shell);
        return shell->line != NULL;
    }
    nread = getline(&shell->line, &cap, stdin);
    return nread != -1;
}

int shell_loop(shell_t *shell)
{
    enum shell_status status = SHELL_CONTINUE;

    while (status == SHELL_CONTINUE) {
        if (!read_input(shell, is_interactive))
            break;
        status = run_command(shell, shell->line);
        if (add_to_history(shell) == FAILURE_EXIT)
            return FAILURE_EXIT;
    }
    return shell->last_status;
}

int shell_loop(shell_t *shell)
{
    bool interactive = isatty(STDIN_FILENO);

    if (display_marrashell() == FAILURE_EXIT)
        return FAILURE_EXIT;
    if (interactive && setup_interactive_signals() == FAILURE_EXIT)
        return FAILURE_EXIT;
    return run_read_eval_loop(shell, interactive);
}
