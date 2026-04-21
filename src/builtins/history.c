/*
** EPITECH PROJECT, 2026
** history.c
** File description:
** This file contains all functions to handle history in minishell.
*/

#include "builtins.h"
#include "mysh.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static bool line_matches_target(const char *line, const char *target)
{
    char *content = strstr(line, "  ");

    if (!content)
        return false;
    return strstr(content + 2, target) != NULL;
}

static int print_history_after_target(history_t *history, char *target)
{
    char *line = NULL;
    size_t len = 0;
    bool target_found = false;

    history->fd = fopen("history", "r");
    if (!history->fd)
        return FAILURE_EXIT;
    while (getline(&line, &len, history->fd) != -1) {
        if (!target_found && line_matches_target(line, target))
            target_found = true;
        if (target_found)
            printf("%s", line);
    }
    free(line);
    fclose(history->fd);
    history->fd = NULL;
    return SUCCESS_EXIT;
}

static int print_all_history(history_t *history)
{
    char *line = NULL;
    size_t len = 0;

    history->fd = fopen("history", "r");
    if (!history->fd)
        return FAILURE_EXIT;
    while (getline(&line, &len, history->fd) != -1)
        printf("%s", line);
    free(line);
    fclose(history->fd);
    history->fd = NULL;
    return SUCCESS_EXIT;
}

int history(shell_t *shell, char **argv)
{
    history_t history = {0};

    (void)shell;
    if (argv[1] != NULL)
        return print_history_after_target(&history, argv[1]);
    return print_all_history(&history);
}
