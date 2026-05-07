/*
** EPITECH PROJECT, 2026
** history_expand.c
** File description:
** parse and expand !-patterns in input strings
*/

#include "expand.h"
#include "mysh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static bool should_skip_bang(char next)
{
    return (next == ' ' || next == '\t' || next == '\n'
        || next == '\0' || next == '=' || next == '('
        || next == ';' || next == '|' || next == '&'
        || next == '<' || next == '>');
}

static int read_number(const char *s, unsigned long long *out)
{
    int i = 0;

    *out = 0;
    while (isdigit(s[i])) {
        *out = *out * 10 + (s[i] - '0');
        i++;
    }
    return i;
}

void buf_append(buf_t *b, const char *s)
{
    size_t slen = strlen(s);
    char *tmp = NULL;

    while (b->len + slen + 1 > b->cap) {
        b->cap = (b->cap == 0) ? 128 : b->cap * 2;
        tmp = realloc(b->data, b->cap);
        if (!tmp)
            return;
        b->data = tmp;
    }
    memcpy(b->data + b->len, s, slen);
    b->len += slen;
    b->data[b->len] = '\0';
}

static char *resolve_substr(const char *input, int pos, int *consumed)
{
    const char *start = input + pos + 2;
    const char *end = strchr(start, '?');
    size_t raw_len = end ? (size_t)(end - start) : strlen(start);
    size_t trim_len = raw_len;
    char *pattern = NULL;
    char *result = NULL;

    while (trim_len > 0 && isspace(start[trim_len - 1]))
        trim_len--;
    pattern = strndup(start, trim_len);
    if (!pattern)
        return NULL;
    result = history_get_by_substr(pattern);
    *consumed = 2 + (int)raw_len + (end ? 1 : 0);
    free(pattern);
    return result;
}

static char *resolve_prefix(const char *input, int pos, int *consumed)
{
    const char *start = input + pos + 1;
    int len = 0;
    char *pattern = NULL;
    char *result = NULL;

    while (start[len] && !isspace(start[len])
        && start[len] != ';' && start[len] != '|'
        && start[len] != '&' && start[len] != '\n')
        len++;
    pattern = strndup(start, len);
    if (!pattern)
        return NULL;
    result = history_get_by_prefix(pattern);
    *consumed = 1 + len;
    free(pattern);
    return result;
}

static char *resolve_numeric(const char *input, int pos, int *consumed)
{
    unsigned long long num = 0;
    int digits = 0;

    if (input[pos + 1] == '-' && isdigit(input[pos + 2])) {
        digits = read_number(input + pos + 2, &num);
        *consumed = 2 + digits;
        return history_get_by_offset(num);
    }
    digits = read_number(input + pos + 1, &num);
    *consumed = 1 + digits;
    return history_get_by_number(num);
}

static char *resolve_event(const char *input, int pos, int *consumed)
{
    char next = input[pos + 1];

    if (next == '!') {
        *consumed = 2;
        return history_get_last();
    }
    if (next == '-' || isdigit(next))
        return resolve_numeric(input, pos, consumed);
    if (next == '?')
        return resolve_substr(input, pos, consumed);
    return resolve_prefix(input, pos, consumed);
}

static int handle_bang(const char *in, int *i, buf_t *b)
{
    int consumed = 0;
    const char *event_name = in + *i;
    char *event = resolve_event(in, *i, &consumed);

    if (!event) {
        if (fprintf(stderr, "%.*s: Event not found.\n", consumed,
                event_name) < 0)
            return -1;
        return -1;
    }
    buf_append(b, event);
    free(event);
    *i += consumed;
    return 0;
}

static int expand_char(const char *input, int *i, buf_t *b, bool *sq)
{
    char tmp[2] = {input[*i], '\0'};

    if (input[*i] == '\'')
        *sq = !(*sq);
    if (input[*i] != '!' || *sq || should_skip_bang(input[*i + 1])) {
        buf_append(b, tmp);
        return 0;
    }
    if (handle_bang(input, i, b) == -1)
        return -1;
    (*i)--;
    return 0;
}

char *history_expand(const char *input)
{
    buf_t b = {NULL, 0, 0};
    bool in_squote = false;

    for (int i = 0; input[i] != '\0'; i++) {
        if (expand_char(input, &i, &b, &in_squote) == -1) {
            free(b.data);
            return NULL;
        }
    }
    return b.data ? b.data : strdup("");
}
