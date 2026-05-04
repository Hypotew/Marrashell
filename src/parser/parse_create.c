/*
** EPITECH PROJECT, 2026
** parse_create.c
** File description:
** allocation functions for parser structures
*/

#include <stdlib.h>

#include "parser.h"

command_t *create_command(void)
{
    command_t *cmd = malloc(sizeof(command_t));

    if (cmd == NULL)
        return NULL;
    cmd->argv = NULL;
    cmd->redirs = NULL;
    cmd->next = NULL;
    return cmd;
}

command_group_t *create_group(void)
{
    command_group_t *grp = malloc(sizeof(command_group_t));

    if (grp == NULL)
        return NULL;
    grp->pipeline = NULL;
    grp->next = NULL;
    return grp;
}

redir_t *create_redir(redir_type_t type, char *target)
{
    redir_t *redir = malloc(sizeof(redir_t));

    if (redir == NULL)
        return NULL;
    redir->type = type;
    redir->target = target;
    redir->read_fd = -1;
    redir->next = NULL;
    return redir;
}
