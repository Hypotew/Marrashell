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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

static void handle_sigint(__attribute__((unused)) int signum)
{
    (void)!write(STDOUT_FILENO, "\n", 1);
}

static int setup_interactive_signals(void)
{
    struct sigaction sa = {0};

    sa.sa_handler = handle_sigint;
    if (sigemptyset(&sa.sa_mask) < 0)
        return FAILURE_EXIT;
    if (sigaction(SIGINT, &sa, NULL) < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static bool interrupted_input(shell_t *shell, ssize_t nread)
{
    if (nread != -1 || errno != EINTR)
        return false;
    clearerr(stdin);
    shell->last_status = 130;
    return true;
}

static bool read_input(shell_t *shell, bool is_interactive)
{
    size_t cap = 0;
    ssize_t nread;

    if (is_interactive) {
        free(shell->line);
        shell->line = read_line(shell);
        return shell->line != NULL;
    }
    free(shell->line);
    shell->line = NULL;
    nread = getline(&shell->line, &cap, stdin);
    if (interrupted_input(shell, nread))
        return true;
    return nread != -1;
}

static enum shell_status process_line(shell_t *shell)
{
    char *expanded = expand_line(shell->line);
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
    if (is_interactive && setup_interactive_signals() == FAILURE_EXIT)
        return FAILURE_EXIT;
    while (status == SHELL_CONTINUE) {
        if (!read_input(shell, is_interactive))
            break;
        status = process_line(shell);
    }
    return shell->last_status;
}
