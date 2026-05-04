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

    if (ctx->pending_redir != REDIR_NONE) {
        redir = create_redir(ctx->pending_redir, token->value);
        if (redir == NULL)
            return PARSE_ERR_FATAL;
        append_redir_to_cmd(ctx->current_cmd, redir);
        ctx->pending_redir = REDIR_NONE;
        return PARSE_OK;
    }
    return append_word_to_argv(ctx->current_cmd, token->value);
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

static parse_status_t handle_semi(parse_ctx_t *ctx)
{
    command_group_t *new_grp = NULL;
    command_t *new_cmd = NULL;

    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    if (ctx->current_cmd->argv == NULL && ctx->current_grp->pipeline == NULL) {
        if (ctx->current_grp == ctx->groups)
            return PARSE_ERR_SYNTAX;
        return PARSE_OK;
    }
    append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    append_grp_to_grp_list(ctx->current_grp, ctx->groups);
    new_grp = create_group();
    new_cmd = create_command();
    if (new_grp == NULL || new_cmd == NULL)
        return PARSE_ERR_FATAL;
    ctx->current_grp = new_grp;
    ctx->current_cmd = new_cmd;
    return PARSE_OK;
}

static parse_status_t handle_token(token_t *token, parse_ctx_t *ctx)
{
    if (token->type == TOK_WORD)
        return handle_word(token, ctx);
    if (token_is_redir(token))
        return handle_redir(token, ctx);
    if (token->type == TOK_PIPE)
        return handle_pipe(ctx);
    if (token->type == TOK_SEMI)
        return handle_semi(ctx);
    return PARSE_OK;
}

static parse_status_t init_ctx(parse_ctx_t *ctx, command_group_t **out)
{
    ctx->groups = create_group();
    ctx->current_grp = ctx->groups;
    ctx->current_cmd = create_command();
    ctx->pending_redir = REDIR_NONE;
    *out = ctx->groups;
    if (ctx->groups == NULL || ctx->current_cmd == NULL)
        return PARSE_ERR_FATAL;
    return PARSE_OK;
}

static parse_status_t finalize_parsing(parse_ctx_t *ctx)
{
    if (ctx->pending_redir != REDIR_NONE)
        return PARSE_ERR_SYNTAX;
    if (ctx->current_cmd->argv == NULL && ctx->current_grp->pipeline == NULL)
        return PARSE_OK;
    append_cmd_to_grp_pipeline(ctx->current_cmd, ctx->current_grp);
    if (ctx->current_grp != ctx->groups)
        append_grp_to_grp_list(ctx->current_grp, ctx->groups);
    return PARSE_OK;
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
        if (status != PARSE_OK)
            return status;
    }
    return finalize_parsing(&ctx);
}
