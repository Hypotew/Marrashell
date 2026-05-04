/*
** EPITECH PROJECT, 2026
** history_utils.c
** File description:
** This file contains certains utils functions to handle history in minishell.
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

    history->fd = fopen(HISTORY_FILE, "r");
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

static bool is_separator(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' ||
        c == '\v' || c == '\f' || c == '\r');
}

static void close_history_file(history_t *history)
{
    fclose(history->fd);
    history->fd = NULL;
}

static int update_last_command(history_t *history, const char *line)
{
    char *content = strstr(line, "  ");

    if (!content)
        return SUCCESS_EXIT;
    free(history->last_cmd);
    history->last_cmd = strdup(content + 2);
    if (!history->last_cmd)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int get_last_history_command(history_t *history)
{
    char *line = NULL;
    size_t len = 0;

    history->fd = fopen(HISTORY_FILE, "r");
    if (!history->fd)
        return SUCCESS_EXIT;
    while (getline(&line, &len, history->fd) != -1)
        if (update_last_command(history, line) == FAILURE_EXIT) {
            free(line);
            close_history_file(history);
            return FAILURE_EXIT;
        }
    free(line);
    close_history_file(history);
    return SUCCESS_EXIT;
}

static bool is_blank_line(const char *line)
{
    if (!line)
        return true;
    for (int i = 0; line[i] != '\0'; i++) {
        if (!is_separator(line[i]))
            return false;
    }
    return true;
}

static bool should_skip_history(shell_t *shell, history_t *history)
{
    if (is_blank_line(shell->line))
        return true;
    if (history->last_cmd && strcmp(history->last_cmd, shell->line) == 0)
        return true;
    return false;
}

int collect_history_infos(history_t *history)
{
    if (get_history_size(history) == FAILURE_EXIT)
        return FAILURE_EXIT;
    if (get_last_history_command(history) == FAILURE_EXIT) {
        free(history->last_cmd);
        return FAILURE_EXIT;
    }
    return SUCCESS_EXIT;
}

int add_to_history(shell_t *shell)
{
    history_t history = {0};

    if (collect_history_infos(&history) == FAILURE_EXIT)
        return FAILURE_EXIT;
    if (should_skip_history(shell, &history)) {
        free(history.last_cmd);
        return SUCCESS_EXIT;
    }
    history.fd = fopen(HISTORY_FILE, "a");
    if (!history.fd) {
        free(history.last_cmd);
        return FAILURE_EXIT;
    }
    history.nb_lines++;
    fprintf(history.fd, "%llu  %s", history.nb_lines, shell->line);
    close_history_file(&history);
    free(history.last_cmd);
    return SUCCESS_EXIT;
}
