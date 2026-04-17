/*
** EPITECH PROJECT, 2026
** env.h
** File description:
** environment helpers
*/

#ifndef ENV_H_
    #define ENV_H_

    #include <stdbool.h>
    #include "shell.h"

char *env_get_value(char **env, const char *key);
int env_set_value(shell_t *shell, const char *key, const char *value);
int env_get_value_index(char **env, const char *key);
char *make_env_entry(const char *key, const char *value);
bool valid_env_key(const char *key);

#endif // !ENV_H_
