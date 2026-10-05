/*
** EPITECH PROJECT, 2026
** var_expand.c
** File description:
** variable expansion ($VAR, ${VAR}, $?)
*/

#include "expand.h"
#include "env.h"
#include "mysh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static char *lookup_env(shell_t *shell, const char *name)
{
    size_t len = strlen(name);
    char *key = malloc(len + 2);
    char *val = NULL;

    if (!key)
        return NULL;
    memcpy(key, name, len);
    key[len] = '=';
    key[len + 1] = '\0';
    val = env_get_value(shell->env, key);
    free(key);
    return val;
}

static char *lookup_var(shell_t *shell, const char *name)
{
    char *val = NULL;

    val = local_get_value(shell->locals, name);
    if (val)
        return val;
    return lookup_env(shell, name);
}

static int read_var_name(const char *s, char *name, int max)
{
    int i = 0;

    while (i < max - 1 && (isalnum(s[i]) || s[i] == '_' || s[i] == '.')) {
        name[i] = s[i];
        i++;
    }
    name[i] = '\0';
    return i;
}

static int expand_braced(shell_t *shell, const char *in, int pos, buf_t *b)
{
    char name[256] = {0};
    const char *end = strchr(in + pos + 2, '}');
    size_t len = 0;
    char *val = NULL;

    if (!end) {
        if (buf_append(b, "${") == FAILURE_EXIT)
            return -1;
        return 2;
    }
    len = (size_t)(end - (in + pos + 2));
    if (len >= sizeof(name))
        len = sizeof(name) - 1;
    memcpy(name, in + pos + 2, len);
    name[len] = '\0';
    val = lookup_var(shell, name);
    if (val)
        if (buf_append(b, val) == FAILURE_EXIT)
            return -1;
    return (int)(len + 3);
}

static int append_status(shell_t *shell, buf_t *b)
{
    char num[16] = {0};
    int len = snprintf(num, sizeof(num), "%d", shell->last_status);

    if (len < 0 || (size_t)len >= sizeof(num))
        return -1;
    if (buf_append(b, num) == FAILURE_EXIT)
        return -1;
    return 0;
}

static int expand_simple(shell_t *shell, const char *in, int pos, buf_t *b)
{
    char name[256] = {0};
    int consumed = read_var_name(in + pos + 1, name, sizeof(name));
    char *val = NULL;

    if (consumed == 0) {
        if (buf_append(b, "$") == FAILURE_EXIT)
            return -1;
        return 1;
    }
    if (strcmp(name, "status") == 0)
        return append_status(shell, b) == -1 ? -1 : 1 + consumed;
    val = lookup_var(shell, name);
    if (val)
        if (buf_append(b, val) == FAILURE_EXIT)
            return -1;
    return 1 + consumed;
}

static int expand_dollar(shell_t *shell, const char *in, int pos, buf_t *b)
{
    if (in[pos + 1] == '?')
        return append_status(shell, b) == -1 ? -1 : 2;
    if (in[pos + 1] == '{')
        return expand_braced(shell, in, pos, b);
    return expand_simple(shell, in, pos, b);
}

static int expand_var_char(shell_t *shell, const char *input, int *i,
    buf_t *b, bool *in_squote)
{
    int consumed = 0;

    if (input[*i] == '\'')
        *in_squote = !(*in_squote);
    if (input[*i] == '$' && !(*in_squote)) {
        consumed = expand_dollar(shell, input, *i, b);
        if (consumed == -1)
            return FAILURE_EXIT;
        *i += consumed;
        return SUCCESS_EXIT;
    }
    if (buf_append(b, (char[]){input[*i], '\0'}) == FAILURE_EXIT)
        return FAILURE_EXIT;
    (*i)++;
    return SUCCESS_EXIT;
}

char *var_expand(shell_t *shell, const char *input)
{
    buf_t b = {NULL, 0, 0};
    bool in_squote = false;

    for (int i = 0; input[i] != '\0';) {
        if (expand_var_char(shell, input, &i, &b, &in_squote)
            == FAILURE_EXIT) {
            free(b.data);
            return NULL;
        }
    }
    return b.data ? b.data : strdup("");
}
