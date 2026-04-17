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

static int get_history_size(history_t *history)
{
    char *line = NULL;
    size_t len = 0;
    unsigned long long count = 0;

    history->fd = fopen("history", "r");
    if (!history->fd) {
        history->nb_lines = 0;
        return SUCCESS_EXIT;
    }
    while (getline(&line, &len, history->fd) != -1)
        count++;
    free(line);
    history->nb_lines = count;
    fclose(history->fd);
    history->fd = NULL;
    return SUCCESS_EXIT;
}

int add_to_history(char *line)
{
    history_t history = {0};

    if (get_history_size(&history) == FAILURE_EXIT)
        return FAILURE_EXIT;
    history.fd = fopen("history", "a");
    if (!history.fd)
        return FAILURE_EXIT;
    history.nb_lines++;
    fprintf(history.fd, "%llu  %s", history.nb_lines, line);
    fclose(history.fd);
    return SUCCESS_EXIT;
}

static int print_history(history_t *history)
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
    (void)argv;
    if (print_history(&history) == FAILURE_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}
