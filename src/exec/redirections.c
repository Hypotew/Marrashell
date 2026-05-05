/*
** EPITECH PROJECT, 2026
** redirections.c
** File description:
** apply file redirections for a command
*/

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "exec.h"
#include "parser.h"

static int open_redir(redir_t *redir)
{
    if (redir->type == REDIR_IN)
        return open(redir->target, O_RDONLY);
    if (redir->type == REDIR_OUT)
        return open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (redir->type == REDIR_APPEND)
        return open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
    return -1;
}

static int get_redir_fd(redir_type_t type)
{
    if (type == REDIR_IN)
        return STDIN_FILENO;
    return STDOUT_FILENO;
}

static bool apply_redir(redir_t *redir)
{
    int fd = open_redir(redir);

    if (fd < 0) {
        if (fprintf(stderr, "%s", redir->target) < 0)
            return false;
        if (fprintf(stderr, "%s", ": No such file or directory.\n") < 0)
            return false;
        return false;
    }
    if (dup2(fd, get_redir_fd(redir->type)) < 0) {
        close(fd);
        return false;
    }
    if (close(fd) < 0) {
        return false;
    }
    return true;
}

static bool apply_one(redir_t *r)
{
    if (r->type == REDIR_HEREDOC)
        return apply_heredoc(r);
    return apply_redir(r);
}

bool apply_redirections(command_t *cmd)
{
    for (redir_t *r = cmd->redirs; r != NULL; r = r->next) {
        if (!apply_one(r))
            return false;
    }
    return true;
}
