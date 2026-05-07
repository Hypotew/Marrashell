/*
** EPITECH PROJECT, 2026
** pwd.c
** File description:
** env helpers for PWD/OLDPWD sync
*/

#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

char *env_get_value(char **env, const char *key)
{
    size_t key_len = strlen(key);

    for (size_t i = 0; env[i] != NULL; ++i) {
        if (strncmp(env[i], key, key_len) == 0)
            return env[i] + key_len;
    }
    return NULL;
}

int env_get_value_index(char **env, const char *key)
{
    int index = -1;
    size_t key_len = strlen(key);

    for (size_t i = 0; env[i] != NULL; ++i) {
        if (strncmp(env[i], key, key_len) == 0)
            index = i;
    }
    return index;
}

char *make_env_entry(const char *key, const char *value)
{
    size_t key_len = strlen(key);
    size_t value_len = 0;
    char *entry = NULL;

    if (value != NULL)
        value_len = strlen(value);
    entry = malloc(key_len + value_len + 2);
    if (entry == NULL)
        return NULL;
    strcpy(entry, key);
    strcat(entry, "=");
    if (value != NULL)
        strcat(entry, value);
    return entry;
}

static int append_env_entry(shell_t *shell, char *entry)
{
    size_t env_len = string_array_len(shell->env);
    char **new_env = malloc(sizeof(char *) * (env_len + 2));
    char **old_env = shell->env;

    if (new_env == NULL) {
        free(entry);
        return FAILURE_EXIT;
    }
    for (size_t i = 0; i < env_len; ++i)
        new_env[i] = old_env[i];
    new_env[env_len] = entry;
    new_env[env_len + 1] = NULL;
    shell->env = new_env;
    free(old_env);
    return SUCCESS_EXIT;
}

static int append_entry_or_free(shell_t *shell, char *entry)
{
    if (append_env_entry(shell, entry) == FAILURE_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

bool valid_env_key(const char *key)
{
    char c = '\0';

    if (key == NULL || key[0] == '\0')
        return false;
    for (int i = 0; key[i] != '\0'; ++i) {
        c = key[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            continue;
        if (c >= '0' && c <= '9')
            continue;
        if (c == '_' || c == '.')
            continue;
        return false;
    }
    return true;
}

int env_set_value(shell_t *shell, const char *key, const char *value)
{
    size_t key_len = 0;
    char *entry = NULL;
    char *old_entry = NULL;

    if (!valid_env_key(key))
        return FAILURE_EXIT;
    key_len = strlen(key);
    entry = make_env_entry(key, value);
    if (entry == NULL)
        return FAILURE_EXIT;
    for (size_t i = 0; shell->env[i] != NULL; ++i) {
        if (strncmp(shell->env[i], key, key_len) == 0) {
            old_entry = shell->env[i];
            shell->env[i] = entry;
            free(old_entry);
            return SUCCESS_EXIT;
        }
    }
    return append_entry_or_free(shell, entry);
}
