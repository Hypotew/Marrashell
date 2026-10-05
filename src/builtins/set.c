/*
** EPITECH PROJECT, 2026
** set.c
** File description:
** set builtin for local variables
*/

#include "builtins.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

static int print_invalid_name(void)
{
    const char *message = "set: Variable name must contain"
        " alphanumeric characters.\n";

    if (fprintf(stderr, "%s", message) < 0)
        return FAILURE_EXIT;
    return FAILURE_EXIT;
}

static int validate_and_set(shell_t *shell, const char *key,
    const char *value)
{
    if (!valid_env_key(key))
        return print_invalid_name();
    return local_set_value(shell, key, value);
}

static int parse_and_set(shell_t *shell, const char *arg)
{
    char *eq = strchr(arg, '=');
    char key[256] = {0};
    size_t key_len = 0;

    if (!eq)
        return validate_and_set(shell, arg, "");
    key_len = (size_t)(eq - arg);
    if (key_len == 0 || key_len >= sizeof(key))
        return print_invalid_name();
    memcpy(key, arg, key_len);
    key[key_len] = '\0';
    return validate_and_set(shell, key, eq + 1);
}

int my_set(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    if (argv[1] == NULL)
        return local_print_all(shell->locals);
    for (int i = 1; argv[i] != NULL; i++) {
        if (parse_and_set(shell, argv[i]) != SUCCESS_EXIT)
            return FAILURE_EXIT;
    }
    return SUCCESS_EXIT;
}
