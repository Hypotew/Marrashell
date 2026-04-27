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

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int shell_loop(shell_t *shell)
{
    size_t cap = 0;
    ssize_t nread;
    enum shell_status status = SHELL_CONTINUE;
    bool is_interactive = isatty(STDIN_FILENO);

    if (display_marrashell() == FAILURE_EXIT)
        return FAILURE_EXIT;
    while (status == SHELL_CONTINUE) {
        if (is_interactive && display_prompt() == FAILURE_EXIT)
            return FAILURE_EXIT;
        nread = getline(&shell->line, &cap, stdin);
        if (nread == -1)
            break;
        status = run_command(shell, shell->line);
        add_to_history(shell);
    }
    return shell->last_status;
}
