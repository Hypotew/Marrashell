/*
** EPITECH PROJECT, 2026
** parse_utils.c
** File description:
** utility functions for command parsing
*/

#include <stdbool.h>

#include "parser.h"

bool token_is_redir(token_t *token)
{
    token_type_t type = token->type;

    return (type == TOK_REDIR_OUT || type == TOK_REDIR_IN ||
        type == TOK_HEREDOC || type == TOK_APPEND);
}

redir_type_t get_tok_to_redir(token_type_t tok_type)
{
    if (tok_type == TOK_REDIR_IN)
        return REDIR_IN;
    if (tok_type == TOK_REDIR_OUT)
        return REDIR_OUT;
    if (tok_type == TOK_APPEND)
        return REDIR_APPEND;
    if (tok_type == TOK_HEREDOC)
        return REDIR_HEREDOC;
    return REDIR_NONE;
}
