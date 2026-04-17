/*
** EPITECH PROJECT, 2026
** parse_free.c
** File description:
** free functions for parser structures
*/

#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

void free_tokens(token_t *tokens)
{
    token_t *next = NULL;

    for (token_t *current = tokens; current != NULL; current = next) {
        next = current->next;
        free(current);
    }
}

static void free_redirs(redir_t *redirs)
{
    redir_t *next = NULL;

    for (redir_t *r = redirs; r != NULL; r = next) {
        next = r->next;
        if (r->read_fd >= 0)
            close(r->read_fd);
        free(r->target);
        free(r);
    }
}

static void free_argv(char **argv)
{
    if (argv == NULL)
        return;
    for (size_t i = 0; argv[i] != NULL; i++)
        free(argv[i]);
    free(argv);
}

void free_command_groups(command_group_t *groups)
{
    command_group_t *next_grp = NULL;
    command_t *next_cmd = NULL;

    for (command_group_t *grp = groups; grp != NULL; grp = next_grp) {
        next_grp = grp->next;
        for (command_t *cmd = grp->pipeline; cmd != NULL; cmd = next_cmd) {
            next_cmd = cmd->next;
            free_argv(cmd->argv);
            free_redirs(cmd->redirs);
            free(cmd);
        }
        free(grp);
    }
}
