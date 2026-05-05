/*
** EPITECH PROJECT, 2026
** run_command.c
** File description:
** decide builtin vs external
*/

#include "builtins.h"
#include "shell.h"
#include "exec.h"
#include "parser.h"
#include "mysh.h"

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static int run_single(shell_t *shell, command_t *command,
    bool *should_exit, bool in_child)
{
    if (run_builtin(shell, command->argv, should_exit))
        return SUCCESS_EXIT;
    if (in_child) {
        if (exec_external(shell, command->argv) == FAILURE_EXIT)
            return FAILURE_EXIT;
    } else {
        run_external(shell, command->argv);
    }
    return SUCCESS_EXIT;
}

static int handle_chile_piped(shell_t *shell, command_t *command,
    bool *should_exit, pipe_ctx_t *ctx)
{
    if (ctx->prev_read != -1)
        dup2(ctx->prev_read, STDIN_FILENO);
    if (command->next != NULL)
        dup2(ctx->fds[1], STDOUT_FILENO);
    if (ctx->prev_read != -1)
        close(ctx->prev_read);
    if (command->next != NULL) {
        close(ctx->fds[0]);
        close(ctx->fds[1]);
    }
    if (!apply_redirections(command))
        return FAILURE_EXIT;
    if (run_single(shell, command, should_exit, true) == FAILURE_EXIT)
        return FAILURE_EXIT;
    exit(shell->last_status);
    return SUCCESS_EXIT;
}

static void close_ctx_fds(pipe_ctx_t *ctx)
{
    if (ctx->fds[0] != -1)
        close(ctx->fds[0]);
    if (ctx->fds[1] != -1)
        close(ctx->fds[1]);
    if (ctx->prev_read != -1)
        close(ctx->prev_read);
}

static bool spawn_piped(shell_t *shell, command_t *current,
    bool *should_exit, pipe_ctx_t *ctx)
{
    pid_t pid = fork();

    if (pid < 0) {
        close_ctx_fds(ctx);
        return false;
    }
    if (pid == 0)
        if (handle_chile_piped(shell, current, should_exit, ctx)
            == FAILURE_EXIT)
            return FAILURE_EXIT;
    if (ctx->fds[1] != -1)
        close(ctx->fds[1]);
    if (ctx->prev_read != -1)
        close(ctx->prev_read);
    ctx->prev_read = ctx->fds[0];
    return true;
}

static void run_piped(shell_t *shell, command_t *pipeline, bool *should_exit)
{
    pipe_ctx_t ctx = {-1, {-1, -1}};
    int status = 0;

    for (command_t *current = pipeline; current != NULL;
        current = current->next) {
        if (current->next != NULL && pipe(ctx.fds) < 0) {
            close_ctx_fds(&ctx);
            return;
        }
        if (!spawn_piped(shell, current, should_exit, &ctx))
            return;
    }
    while (waitpid(-1, &status, 0) > 0);
    shell->last_status = WEXITSTATUS(status);
}

static int run_pipeline(shell_t *shell, command_t *pipeline, bool *should_exit)
{
    if (pipeline == NULL || shell == NULL) {
        *should_exit = true;
        return FAILURE_EXIT;
    }
    if (pipeline->next == NULL && pipeline->redirs == NULL) {
        if (run_single(shell, pipeline, should_exit, false) == FAILURE_EXIT)
            return FAILURE_EXIT;
        return SUCCESS_EXIT;
    }
    if (!prepare_heredocs(pipeline))
        return FAILURE_EXIT;
    run_piped(shell, pipeline, should_exit);
    return SUCCESS_EXIT;
}

static enum shell_status run_groups(shell_t *shell, command_group_t *groups)
{
    bool should_exit = false;

    for (command_group_t *grp = groups; grp != NULL; grp = grp->next) {
        if (run_pipeline(shell, grp->pipeline, &should_exit) == FAILURE_EXIT)
            return SHELL_EXIT;
        if (should_exit)
            return SHELL_EXIT;
    }
    return SHELL_CONTINUE;
}

static enum shell_status handle_parse_error(shell_t *shell,
    parse_status_t status, command_group_t *groups)
{
    free_command_groups(groups);
    if (status == PARSE_ERR_SYNTAX) {
        if (fprintf(stderr, "%s", "Invalid null command.\n") < 0) {
            shell->last_status = FAILURE_EXIT;
            return SHELL_EXIT;
        }
        shell->last_status = 1;
        return SHELL_CONTINUE;
    }
    return SHELL_EXIT;
}

enum shell_status run_command(shell_t *shell, char *input_buf)
{
    token_t *tokens = tokenize_input(input_buf);
    command_group_t *groups = NULL;
    parse_status_t status = PARSE_OK;
    enum shell_status result = SHELL_CONTINUE;

    if (tokens == NULL)
        return SHELL_CONTINUE;
    status = parse_command_groups(tokens, &groups);
    free_tokens(tokens);
    if (status != PARSE_OK)
        return handle_parse_error(shell, status, groups);
    result = run_groups(shell, groups);
    free_command_groups(groups);
    return result;
}
