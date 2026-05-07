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

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

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
    bool is_interactive = isatty(STDIN_FILENO);

    if (display_marrashell() == FAILURE_EXIT)
        return FAILURE_EXIT;
    while (status == SHELL_CONTINUE) {
        if (!read_input(shell, is_interactive))
            break;
        status = run_command(shell, shell->line);
        add_to_history(shell);
    }
    return shell->last_status;
}
