/*
** EPITECH PROJECT, 2026
** alias_expand.c
** File description:
** replace first word with alias value if it matches
*/

#include "expand.h"
#include "env.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int skip_whitespace(const char *s)
{
    int i = 0;

    while (s[i] && isspace(s[i]))
        i++;
    return i;
}

static int read_word_len(const char *s)
{
    int i = 0;

    while (s[i] && !isspace(s[i]) && s[i] != ';'
        && s[i] != '|' && s[i] != '&'
        && s[i] != '<' && s[i] != '>')
        i++;
    return i;
}

static char *build_expanded(const char *pre, int pre_len,
    const char *val, const char *rest)
{
    size_t vlen = strlen(val);
    size_t rlen = strlen(rest);
    char *result = malloc(pre_len + vlen + rlen + 1);

    if (!result)
        return NULL;
    memcpy(result, pre, pre_len);
    memcpy(result + pre_len, val, vlen);
    memcpy(result + pre_len + vlen, rest, rlen);
    result[pre_len + vlen + rlen] = '\0';
    return result;
}

char *alias_expand(shell_t *shell, const char *input)
{
    int ws = skip_whitespace(input);
    int wlen = read_word_len(input + ws);
    char name[256] = {0};
    char *val = NULL;

    if (wlen == 0 || wlen >= (int)sizeof(name))
        return strdup(input);
    memcpy(name, input + ws, wlen);
    name[wlen] = '\0';
    val = alias_get_value(shell->aliases, name);
    if (!val)
        return strdup(input);
    return build_expanded(input, ws, val, input + ws + wlen);
}
