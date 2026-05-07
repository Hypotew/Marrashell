/*
** EPITECH PROJECT, 2026
** aliases.c
** File description:
** alias storage helpers (get/set/unset/print)
*/

#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *alias_get_value(char **aliases, const char *name)
{
    size_t name_len = strlen(name);

    for (size_t i = 0; aliases[i] != NULL; i++) {
        if (strncmp(aliases[i], name, name_len) == 0
            && aliases[i][name_len] == '=')
            return aliases[i] + name_len + 1;
    }
    return NULL;
}

static int append_alias(shell_t *shell, char *entry)
{
    size_t len = string_array_len(shell->aliases);
    char **new_arr = malloc(sizeof(char *) * (len + 2));
    char **old = shell->aliases;

    if (!new_arr) {
        free(entry);
        return FAILURE_EXIT;
    }
    for (size_t i = 0; i < len; i++)
        new_arr[i] = old[i];
    new_arr[len] = entry;
    new_arr[len + 1] = NULL;
    shell->aliases = new_arr;
    free(old);
    return SUCCESS_EXIT;
}

int alias_set_value(shell_t *shell, const char *name, const char *value)
{
    size_t name_len = strlen(name);
    char *entry = make_env_entry(name, value);

    if (!entry)
        return FAILURE_EXIT;
    for (size_t i = 0; shell->aliases[i] != NULL; i++) {
        if (strncmp(shell->aliases[i], name, name_len) == 0
            && shell->aliases[i][name_len] == '=') {
            free(shell->aliases[i]);
            shell->aliases[i] = entry;
            return SUCCESS_EXIT;
        }
    }
    return append_alias(shell, entry);
}

int alias_unset(shell_t *shell, const char *name)
{
    size_t name_len = strlen(name);
    bool found = false;

    for (size_t i = 0; shell->aliases[i] != NULL; i++) {
        if (!found && strncmp(shell->aliases[i], name, name_len) == 0
            && shell->aliases[i][name_len] == '=') {
            free(shell->aliases[i]);
            found = true;
        }
        if (found)
            shell->aliases[i] = shell->aliases[i + 1];
    }
    return SUCCESS_EXIT;
}

void alias_unset_all(shell_t *shell)
{
    for (size_t i = 0; shell->aliases[i] != NULL; i++)
        free(shell->aliases[i]);
    free(shell->aliases);
    shell->aliases = calloc(1, sizeof(char *));
}

void alias_print_all(char **aliases)
{
    char *eq = NULL;

    for (size_t i = 0; aliases[i] != NULL; i++) {
        eq = strchr(aliases[i], '=');
        if (eq)
            printf("%.*s\t(%s)\n", (int)(eq - aliases[i]), aliases[i],
                eq + 1);
    }
}
