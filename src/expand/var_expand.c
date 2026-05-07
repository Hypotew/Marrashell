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

static char *lookup_var(shell_t *shell, const char *name)
{
    char *key = NULL;
    char *val = NULL;
    size_t len = strlen(name);

    val = local_get_value(shell->locals, name);
    if (val)
        return val;
    key = malloc(len + 2);
    if (!key)
        return NULL;
    memcpy(key, name, len);
    key[len] = '=';
    key[len + 1] = '\0';
    val = env_get_value(shell->env, key);
    free(key);
    return val;
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
        buf_append(b, "${");
        return 2;
    }
    len = (size_t)(end - (in + pos + 2));
    if (len >= sizeof(name))
        len = sizeof(name) - 1;
    memcpy(name, in + pos + 2, len);
    name[len] = '\0';
    val = lookup_var(shell, name);
    if (val)
        buf_append(b, val);
    return (int)(len + 3);
}

static int expand_status(shell_t *shell, buf_t *b)
{
    char num[16] = {0};

    snprintf(num, sizeof(num), "%d", shell->last_status);
    buf_append(b, num);
    return 2;
}

static int expand_simple(shell_t *shell, const char *in, int pos, buf_t *b)
{
    char name[256] = {0};
    int consumed = read_var_name(in + pos + 1, name, sizeof(name));
    char *val = NULL;

    if (consumed == 0) {
        buf_append(b, "$");
        return 1;
    }
    val = lookup_var(shell, name);
    if (val)
        buf_append(b, val);
    return 1 + consumed;
}

static int expand_dollar(shell_t *shell, const char *in, int pos, buf_t *b)
{
    if (in[pos + 1] == '?')
        return expand_status(shell, b);
    if (in[pos + 1] == '{')
        return expand_braced(shell, in, pos, b);
    return expand_simple(shell, in, pos, b);
}

char *var_expand(shell_t *shell, const char *input)
{
    buf_t b = {NULL, 0, 0};
    bool in_squote = false;

    for (int i = 0; input[i] != '\0';) {
        if (input[i] == '\'')
            in_squote = !in_squote;
        if (input[i] == '$' && !in_squote) {
            i += expand_dollar(shell, input, i, &b);
            continue;
        }
        buf_append(&b, (char[]){input[i], '\0'});
        i++;
    }
    return b.data ? b.data : strdup("");
}
