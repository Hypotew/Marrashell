/*
** EPITECH PROJECT, 2026
** test_parser.c
** File description:
** unit tests for tokenizer and parser
*/
#include <criterion/criterion.h>
#include "parser.h"

Test(tokenize, empty_string)
{
    char input[] = "";
    cr_assert_null(tokenize_input(input));
}

Test(tokenize, newline_only)
{
    char input[] = "\n";
    cr_assert_null(tokenize_input(input));
}

Test(tokenize, single_word)
{
    char input[] = "hello";
    token_t *tokens = tokenize_input(input);

    cr_assert_not_null(tokens);
    cr_assert_eq(tokens->type, TOK_WORD);
    cr_assert_str_eq(tokens->value, "hello");
    cr_assert_null(tokens->next);
    free_tokens(tokens);
}

Test(tokenize, two_words)
{
    char input[] = "ls -la";
    token_t *tokens = tokenize_input(input);

    cr_assert_eq(tokens->type, TOK_WORD);
    cr_assert_str_eq(tokens->value, "ls");
    cr_assert_eq(tokens->next->type, TOK_WORD);
    cr_assert_str_eq(tokens->next->value, "-la");
    cr_assert_null(tokens->next->next);
    free_tokens(tokens);
}

Test(tokenize, pipe_operator)
{
    char input[] = "ls | grep foo";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_PIPE); t = t->next;
    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, semicolon)
{
    char input[] = "ls; echo hi";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_SEMI); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, redir_out)
{
    char input[] = "echo hi > file";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_REDIR_OUT); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, redir_in)
{
    char input[] = "cat < file";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_REDIR_IN); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, append_operator)
{
    char input[] = "echo hi >> file";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_APPEND); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, heredoc_operator)
{
    char input[] = "cat << EOF";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_HEREDOC); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, and_operator)
{
    char input[] = "ls && echo ok";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_AND); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, or_operator)
{
    char input[] = "ls || echo ok";
    token_t *tokens = tokenize_input(input);
    token_t *t = tokens;

    cr_assert_eq(t->type, TOK_WORD); t = t->next;
    cr_assert_eq(t->type, TOK_OR); t = t->next;
    cr_assert_eq(t->type, TOK_WORD);
    free_tokens(tokens);
}

Test(tokenize, multiple_spaces_ignored)
{
    char input[] = "ls   -la";
    token_t *tokens = tokenize_input(input);

    cr_assert_eq(tokens->type, TOK_WORD);
    cr_assert_eq(tokens->next->type, TOK_WORD);
    cr_assert_null(tokens->next->next);
    free_tokens(tokens);
}

Test(parse_command, null_tokens)
{
    command_group_t *groups = NULL;
    parse_status_t status = parse_command_groups(NULL, &groups);

    cr_assert_eq(status, PARSE_OK);
    cr_assert_null(groups);
}

Test(parse_command, simple_command)
{
    char input[] = "ls";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->pipeline);
    cr_assert_str_eq(groups->pipeline->argv[0], "ls");
    cr_assert_null(groups->pipeline->argv[1]);
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, command_with_args)
{
    char input[] = "ls -la /tmp";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_str_eq(groups->pipeline->argv[0], "ls");
    cr_assert_str_eq(groups->pipeline->argv[1], "-la");
    cr_assert_str_eq(groups->pipeline->argv[2], "/tmp");
    cr_assert_null(groups->pipeline->argv[3]);
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, pipe_two_commands)
{
    char input[] = "ls | grep foo";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->pipeline);
    cr_assert_str_eq(groups->pipeline->argv[0], "ls");
    cr_assert_not_null(groups->pipeline->next);
    cr_assert_str_eq(groups->pipeline->next->argv[0], "grep");
    cr_assert_str_eq(groups->pipeline->next->argv[1], "foo");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, semicolon_two_groups)
{
    char input[] = "ls; echo hi";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->next);
    cr_assert_str_eq(groups->pipeline->argv[0], "ls");
    cr_assert_str_eq(groups->next->pipeline->argv[0], "echo");
    cr_assert_eq(groups->next->sep, SEP_SEMI);
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, redir_out)
{
    char input[] = "echo hi > out.txt";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->pipeline->redirs);
    cr_assert_eq(groups->pipeline->redirs->type, REDIR_OUT);
    cr_assert_str_eq(groups->pipeline->redirs->target, "out.txt");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, redir_in)
{
    char input[] = "cat < in.txt";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->pipeline->redirs);
    cr_assert_eq(groups->pipeline->redirs->type, REDIR_IN);
    cr_assert_str_eq(groups->pipeline->redirs->target, "in.txt");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, redir_append)
{
    char input[] = "echo hi >> out.txt";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_eq(groups->pipeline->redirs->type, REDIR_APPEND);
    cr_assert_str_eq(groups->pipeline->redirs->target, "out.txt");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, and_separator)
{
    char input[] = "ls && echo ok";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->next);
    cr_assert_eq(groups->next->sep, SEP_AND);
    cr_assert_str_eq(groups->next->pipeline->argv[0], "echo");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, or_separator)
{
    char input[] = "ls || echo ok";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->next);
    cr_assert_eq(groups->next->sep, SEP_OR);
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, three_groups_semicolon)
{
    char input[] = "a; b; c";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_not_null(groups->next);
    cr_assert_not_null(groups->next->next);
    cr_assert_str_eq(groups->pipeline->argv[0], "a");
    cr_assert_str_eq(groups->next->pipeline->argv[0], "b");
    cr_assert_str_eq(groups->next->next->pipeline->argv[0], "c");
    free_tokens(tokens);
    free_command_groups(groups);
}

Test(parse_command, pipe_three_commands)
{
    char input[] = "cat file | sort | uniq";
    token_t *tokens = tokenize_input(input);
    command_group_t *groups = NULL;

    cr_assert_eq(parse_command_groups(tokens, &groups), PARSE_OK);
    cr_assert_str_eq(groups->pipeline->argv[0], "cat");
    cr_assert_str_eq(groups->pipeline->next->argv[0], "sort");
    cr_assert_str_eq(groups->pipeline->next->next->argv[0], "uniq");
    free_tokens(tokens);
    free_command_groups(groups);
}
