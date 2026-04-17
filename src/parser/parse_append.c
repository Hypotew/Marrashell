/*
** EPITECH PROJECT, 2026
** parse_append.c
** File description:
** append functions for parser linked lists
*/

#include <stdlib.h>

#include "parser.h"

void append_cmd_to_grp_pipeline(command_t *cmd, command_group_t *grp)
{
    command_t *tail = NULL;

    if (cmd == NULL || grp == NULL)
        return;
    if (grp->pipeline == NULL) {
        grp->pipeline = cmd;
        return;
    }
    tail = grp->pipeline;
    for (; tail->next != NULL; tail = tail->next);
    tail->next = cmd;
}

void append_grp_to_grp_list(command_group_t *grp, command_group_t *head)
{
    command_group_t *tail = NULL;

    if (grp == NULL || head == NULL || grp == head)
        return;
    tail = head;
    for (; tail->next != NULL; tail = tail->next);
    tail->next = grp;
}

void append_redir_to_cmd(command_t *cmd, redir_t *redir)
{
    redir_t *tail = cmd->redirs;

    if (tail == NULL) {
        cmd->redirs = redir;
        return;
    }
    for (; tail->next != NULL; tail = tail->next);
    tail->next = redir;
}

parse_status_t append_word_to_argv(command_t *cmd, char *word)
{
    size_t count = 0;
    char **new_argv = NULL;

    if (cmd->argv != NULL)
        for (; cmd->argv[count] != NULL; count++);
    new_argv = malloc(sizeof(char *) * (count + 2));
    if (new_argv == NULL)
        return PARSE_ERR_FATAL;
    for (size_t i = 0; i < count; i++)
        new_argv[i] = cmd->argv[i];
    new_argv[count] = word;
    new_argv[count + 1] = NULL;
    free(cmd->argv);
    cmd->argv = new_argv;
    return PARSE_OK;
}
