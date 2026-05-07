/*
** EPITECH PROJECT, 2026
** locals.c
** File description:
** local variable helpers (set/unset/get)
*/

#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *local_get_value(char **locals, const char *key)
{
    size_t key_len = strlen(key);

    for (size_t i = 0; locals[i] != NULL; ++i) {
        if (strncmp(locals[i], key, key_len) == 0
            && locals[i][key_len] == '=')
            return locals[i] + key_len + 1;
    }
    return NULL;
}

static int append_local(shell_t *shell, char *entry)
{
    size_t len = string_array_len(shell->locals);
    char **new_arr = malloc(sizeof(char *) * (len + 2));
    char **old = shell->locals;

    if (!new_arr) {
        free(entry);
        return FAILURE_EXIT;
    }
    for (size_t i = 0; i < len; i++)
        new_arr[i] = old[i];
    new_arr[len] = entry;
    new_arr[len + 1] = NULL;
    shell->locals = new_arr;
    free(old);
    return SUCCESS_EXIT;
}

int local_set_value(shell_t *shell, const char *key, const char *value)
{
    size_t key_len = strlen(key);
    char *entry = make_env_entry(key, value);

    if (!entry)
        return FAILURE_EXIT;
    for (size_t i = 0; shell->locals[i] != NULL; i++) {
        if (strncmp(shell->locals[i], key, key_len) == 0
            && shell->locals[i][key_len] == '=') {
            free(shell->locals[i]);
            shell->locals[i] = entry;
            return SUCCESS_EXIT;
        }
    }
    return append_local(shell, entry);
}

int local_unset(shell_t *shell, const char *key)
{
    size_t key_len = strlen(key);
    bool found = false;

    for (size_t i = 0; shell->locals[i] != NULL; i++) {
        if (!found && strncmp(shell->locals[i], key, key_len) == 0
            && shell->locals[i][key_len] == '=') {
            free(shell->locals[i]);
            found = true;
        }
        if (found)
            shell->locals[i] = shell->locals[i + 1];
    }
    return SUCCESS_EXIT;
}

void local_print_all(char **locals)
{
    for (size_t i = 0; locals[i] != NULL; i++)
        printf("%s\n", locals[i]);
}
