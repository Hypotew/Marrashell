/*
** EPITECH PROJECT, 2026
** tokenize.c
** File description:
** parser the input line into words and shell operators
*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static token_t *token_build(char *value, token_type_t type)
{
    token_t *token = malloc(sizeof(token_t));

    if (token == NULL)
        return NULL;
    token->value = value;
    token->type = type;
    token->next = NULL;
    return token;
}

static void token_append(token_t **head, token_t *token)
{
    token_t *tail = NULL;

    if (*head == NULL) {
        *head = token;
        return;
    }
    for (tail = *head; tail->next != NULL; tail = tail->next);
    tail->next = token;
}

static bool append_token(token_t **head, char *value, token_type_t type)
{
    token_t *token = token_build(value, type);

    if (token == NULL) {
        free(value);
        return false;
    }
    token_append(head, token);
    return true;
}

static void skip_whitespaces(char *line, size_t *cursor)
{
    while (strchr(" \t", line[*cursor]))
        ++(*cursor);
}

static bool compare_operator(char *line, size_t cursor, const char *op)
{
    for (size_t i = 0; op[i]; ++i) {
        if (op[i] != line[cursor + i])
            return false;
    }
    return true;
}

static parse_status_t read_operator(char *line, size_t *cursor, token_t **head)
{
    for (size_t i = 0; G_OPS[i].text != NULL; ++i) {
        if (!compare_operator(line, *cursor, G_OPS[i].text))
            continue;
        *cursor += G_OPS[i].op_len;
        if (!append_token(head, NULL, G_OPS[i].type))
            return PARSE_ERR_FATAL;
        return PARSE_OK;
    }
    return PARSE_ERR_SYNTAX;
}

static char *word_dup(char *line, size_t start, size_t end)
{
    size_t len = end - start;
    char *word = malloc(sizeof(char) * (len + 1));

    if (word == NULL)
        return NULL;
    for (size_t i = 0; i < len; ++i)
        word[i] = line[start + i];
    word[len] = '\0';
    return word;
}

static bool read_word(char *line, size_t *cursor, token_t **head)
{
    size_t start = *cursor;
    char *word = NULL;

    while (line[*cursor] != '\0' && !strchr(" \t\n<>;|", line[*cursor]))
        ++(*cursor);
    word = word_dup(line, start, *cursor);
    if (word == NULL)
        return false;
    if (!append_token(head, word, TOK_WORD))
        return false;
    return true;
}

static bool read_next_token(char *line, size_t *cursor, token_t **head)
{
    parse_status_t status = PARSE_ERR_SYNTAX;

    skip_whitespaces(line, cursor);
    if (line[*cursor] == '\0' || line[*cursor] == '\n')
        return true;
    status = read_operator(line, cursor, head);
    if (status == PARSE_ERR_FATAL)
        return false;
    if (status == PARSE_OK)
        return true;
    return read_word(line, cursor, head);
}

token_t *tokenize_input(char *line)
{
    token_t *head = NULL;
    size_t cursor = 0;

    while (line[cursor] != '\0' && line[cursor] != '\n') {
        if (!read_next_token(line, &cursor, &head)) {
            free_tokens(head);
            return NULL;
        }
    }
    return head;
}
