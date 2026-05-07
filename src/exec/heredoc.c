/*
** EPITECH PROJECT, 2026
** heredoc.c
** File description:
** handle heredoc (<<) redirection
*/

#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "exec.h"
#include "parser.h"

#include <string.h>

static void write_lines_until(int write_fd, char *delim)
{
    char *line = NULL;
    size_t n = 0;
    ssize_t len = 0;

    while (1) {
        if (write(STDOUT_FILENO, "> ", 2) < 0)
            break;
        len = getline(&line, &n, stdin);
        if (len < 0)
            break;
        if (line[len - 1] == '\n')
            line[len - 1] = '\0';
        if (strcmp(line, delim) == 0)
            break;
        line[len - 1] = '\n';
        if (write(write_fd, line, len) < 0)
            break;
    }
    free(line);
}

static bool fill_heredoc(redir_t *redir)
{
    int fds[2];

    if (pipe(fds) < 0)
        return false;
    write_lines_until(fds[1], redir->target);
    close(fds[1]);
    redir->read_fd = fds[0];
    return true;
}

bool apply_heredoc(redir_t *redir)
{
    if (redir->read_fd < 0)
        return false;
    dup2(redir->read_fd, STDIN_FILENO);
    close(redir->read_fd);
    redir->read_fd = -1;
    return true;
}

static bool fill_cmd_heredocs(command_t *cmd)
{
    for (redir_t *r = cmd->redirs; r != NULL; r = r->next) {
        if (r->type == REDIR_HEREDOC && !fill_heredoc(r))
            return false;
    }
    return true;
}

bool prepare_heredocs(command_t *pipeline)
{
    for (command_t *cmd = pipeline; cmd != NULL; cmd = cmd->next) {
        if (!fill_cmd_heredocs(cmd))
            return false;
    }
    return true;
}
