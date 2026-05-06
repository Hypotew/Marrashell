/*
** EPITECH PROJECT, 2026
** parse_command.c
** File description:
** transform token list into executable command structures
*/

#include <stdbool.h>
#include <stdlib.h>

#include "parser.h"

static parse_status_t handle_word(token_t *token, parse_ctx_t *ctx)
{
    redir_t *redir = NULL;
    parse_status_t status = PARSE_OK;

    if (ctx->pending_redir != REDIR_NONE) {
        redir = create_redir(ctx->pending_redir, token->value);
        if (redir == NULL)
            return PARSE_ERR_FATAL;
        append_redir_to_cmd(ctx->current_cmd, redir);
        ctx->pending_redir = REDIR_NONE;
        token->value = NULL;
        return PARSE_OK;
    }
    status = append_word_to_argv(ctx->current_cmd, token->value);
    if (status == PARSE_OK)
        token->value = NULL;
    return status;
}

static parse_status_t handle_redir(token_t *token, parse_ctx_t *ctx)
{
    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    ctx->pending_redir = get_tok_to_redir(token->type);
    return PARSE_OK;
}

static parse_status_t handle_pipe(parse_ctx_t *ctx)
{
    command_t *new_cmd = NULL;

    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    if (ctx->current_cmd->argv == NULL)
        return PARSE_ERR_SYNTAX;
    append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    new_cmd = create_command();
    if (new_cmd == NULL)
        return PARSE_ERR_FATAL;
    ctx->current_cmd = new_cmd;
    return PARSE_OK;
}

static parse_status_t create_next_group(parse_ctx_t *ctx)
{
    command_group_t *new_grp = create_group();
    command_t *new_cmd = create_command();

    if (new_grp == NULL || new_cmd == NULL) {
        free(new_grp);
        free(new_cmd);
        return PARSE_ERR_FATAL;
    }
    ctx->current_grp = new_grp;
    ctx->current_cmd = new_cmd;
    return PARSE_OK;
}

static parse_status_t handle_separator(token_t *token, parse_ctx_t *ctx)
{
    parse_status_t status;

    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    if (ctx->current_cmd->argv == NULL && ctx->current_grp->pipeline == NULL) {
        if (ctx->current_grp == ctx->groups)
            return PARSE_ERR_SYNTAX;
        return PARSE_OK;
    }
    append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    append_grp_to_grp_list(ctx->current_grp, ctx->groups);
    status = create_next_group(ctx);
    if (status != PARSE_OK)
        return status;
    ctx->current_grp->sep = get_tok_to_sep(token->type);
    return status;
}

static parse_status_t handle_token(token_t *token, parse_ctx_t *ctx)
{
    if (token->type == TOK_WORD)
        return handle_word(token, ctx);
    if (token_is_redir(token))
        return handle_redir(token, ctx);
    if (token->type == TOK_PIPE)
        return handle_pipe(ctx);
    if (token_is_separator(token))
        return handle_separator(token, ctx);
    return PARSE_OK;
}

static parse_status_t init_ctx(parse_ctx_t *ctx, command_group_t **out)
{
    ctx->groups = create_group();
    ctx->current_grp = ctx->groups;
    ctx->current_cmd = create_command();
    ctx->pending_redir = REDIR_NONE;
    *out = ctx->groups;
    if (ctx->groups == NULL || ctx->current_cmd == NULL) {
        free(ctx->groups);
        free(ctx->current_cmd);
        *out = NULL;
        return PARSE_ERR_FATAL;
    }
    return PARSE_OK;
}

static parse_status_t finalize_parsing(parse_ctx_t *ctx)
{
    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    if (ctx->current_cmd->argv == NULL && ctx->current_grp->pipeline == NULL) {
        free(ctx->current_cmd);
        if (ctx->current_grp != ctx->groups)
            free(ctx->current_grp);
        return PARSE_OK;
    }
    append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    if (ctx->current_grp != ctx->groups)
        append_grp_to_grp_list(ctx->current_grp, ctx->groups);
    return PARSE_OK;
}

static void cleanup_ctx(parse_ctx_t *ctx, command_group_t **out)
{
    bool grp_linked = false;
    bool cmd_linked = false;

    for (command_group_t *grp = ctx->groups; grp != NULL; grp = grp->next)
        if (grp == ctx->current_grp)
            grp_linked = true;
    if (ctx->current_grp != NULL)
        for (command_t *cmd = ctx->current_grp->pipeline;
            cmd != NULL; cmd = cmd->next)
            cmd_linked = cmd_linked || (cmd == ctx->current_cmd);
    if (!cmd_linked && ctx->current_cmd != NULL && ctx->current_grp != NULL)
        append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    if (!grp_linked && ctx->current_grp != NULL && ctx->groups != NULL)
        append_grp_to_grp_list(ctx->current_grp, ctx->groups);
    free_command_groups(ctx->groups);
    *out = NULL;
}

parse_status_t parse_command_groups(token_t *tokens, command_group_t **out)
{
    parse_ctx_t ctx;
    parse_status_t status;
    token_t *current = tokens;

    if (tokens == NULL)
        return PARSE_OK;
    status = init_ctx(&ctx, out);
    if (status != PARSE_OK)
        return status;
    for (; current != NULL; current = current->next) {
        status = handle_token(current, &ctx);
        if (status != PARSE_OK) {
            cleanup_ctx(&ctx, out);
            return status;
        }
    }
    return finalize_parsing(&ctx);
}
