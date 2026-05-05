/*
** EPITECH PROJECT, 2026
** repeat.c
** File description:
** This file contains all functions to handle repeat in minishell.
*/

#include "builtins.h"
#include "mysh.h"
#include "shell.h"
#include "utils.h"
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>

static int classic_repeat(shell_t *shell, char **argv, unsigned int nb,
    bool *should_exit)
{
    char *rebuilt_line = array_to_string(argv, 2);
    enum shell_status status = SHELL_CONTINUE;

    if (!rebuilt_line)
        return FAILURE_EXIT;
    for (unsigned int i = 0; i < nb; i++) {
        status = run_command(shell, rebuilt_line);
        if (status == SHELL_EXIT) {
            *should_exit = true;
            free(rebuilt_line);
            return shell->last_status;
        }
    }
    free(rebuilt_line);
    return shell->last_status;
}

int my_repeat(shell_t *shell, char **argv, bool *should_exit)
{
    unsigned int nb = 0;

    if (!argv[1] || !argv[2]) {
        fprintf(stderr, "repeat: Too few arguments.\n");
        return FAILURE_EXIT;
    }
    if (get_positive_nbr(argv[1], &nb) == FAILURE_EXIT) {
        fprintf(stderr, "repeat: Badly formed number.\n");
        return FAILURE_EXIT;
    }
    return classic_repeat(shell, argv, nb, should_exit);
}
