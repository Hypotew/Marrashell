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

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
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

static bool interrupted_input(shell_t *shell, ssize_t nread, bool interactive)
{
    if (nread != -1 || errno != EINTR || !interactive)
        return false;
    clearerr(stdin);
    shell->last_status = 130;
    return true;
}

static int run_read_eval_loop(shell_t *shell, bool interactive)
{
    size_t cap = 0;
    ssize_t nread;
    enum shell_status status = SHELL_CONTINUE;

    while (status == SHELL_CONTINUE) {
        if (interactive && display_prompt() == FAILURE_EXIT)
            return FAILURE_EXIT;
        nread = getline(&shell->line, &cap, stdin);
        if (interrupted_input(shell, nread, interactive))
            continue;
        if (nread == -1)
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
