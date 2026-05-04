/*
** EPITECH PROJECT, 2026
** parser.h
** File description:
** parser tokenize and command parsing API
*/

#ifndef PARSER_H_
    #define PARSER_H_

    #include <stddef.h>
    #include <stdbool.h>

typedef enum {
    TOK_NONE,
    TOK_WORD,
    TOK_SEMI,
    TOK_PIPE,
    TOK_REDIR_IN,
    TOK_REDIR_OUT,
    TOK_APPEND,
    TOK_HEREDOC
} token_type_t;

typedef struct {
    const char *text;
    token_type_t type;
    size_t op_len;
} op_def_t;

static const op_def_t G_OPS[] = {
    {"<<", TOK_HEREDOC, 2},
    {">>", TOK_APPEND, 2},
    {"|", TOK_PIPE, 1},
    {";", TOK_SEMI, 1},
    {"<", TOK_REDIR_IN, 1},
    {">", TOK_REDIR_OUT, 1},
    {NULL, TOK_NONE, 1}
};

typedef enum {
    REDIR_IN,
    REDIR_OUT,
    REDIR_APPEND,
    REDIR_HEREDOC,
    REDIR_NONE
} redir_type_t;

typedef struct s_token {
    token_type_t type;
    char *value;
    struct s_token *next;
} token_t;

typedef struct s_redir {
    redir_type_t type;
    char *target;
    int read_fd;
    struct s_redir *next;
} redir_t;

typedef struct s_command {
    char **argv;
    redir_t *redirs;
    struct s_command *next;
} command_t;

typedef struct s_command_group {
    command_t *pipeline;
    struct s_command_group *next;
} command_group_t;

typedef enum {
    PARSE_OK = -1,
    PARSE_ERR_FATAL = 0,
    PARSE_ERR_SYNTAX = 1
} parse_status_t;

typedef struct {
    command_t *current_cmd;
    command_group_t *current_grp;
    command_group_t *groups;
    redir_type_t pending_redir;
} parse_ctx_t;

token_t *tokenize_input(char *line);
void free_tokens(token_t *tokens);

parse_status_t parse_command_groups(token_t *tokens, command_group_t **out);
void free_command_groups(command_group_t *groups);

// utils

bool token_is_redir(token_t *token);
redir_type_t get_tok_to_redir(token_type_t tok_type);

// create
command_t *create_command(void);
command_group_t *create_group(void);
redir_t *create_redir(redir_type_t type, char *target);

// append
void append_cmd_to_grp_pipeline(command_t *cmd, command_group_t *grp);
void append_grp_to_grp_list(command_group_t *grp, command_group_t *head);
void append_redir_to_cmd(command_t *cmd, redir_t *redir);
parse_status_t append_word_to_argv(command_t *cmd, char *word);

#endif // !PARSER_H_
