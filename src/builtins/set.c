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

static int parse_and_set(shell_t *shell, const char *arg)
{
    char *eq = strchr(arg, '=');
    char key[256] = {0};
    size_t key_len = 0;

    if (!eq) {
        return local_set_value(shell, arg, "");
    }
    key_len = (size_t)(eq - arg);
    if (key_len == 0 || key_len >= sizeof(key)) {
        fprintf(stderr, "set: invalid variable name\n");
        return FAILURE_EXIT;
    }
    memcpy(key, arg, key_len);
    key[key_len] = '\0';
    return local_set_value(shell, key, eq + 1);
}

int my_set(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    if (argv[1] == NULL) {
        local_print_all(shell->locals);
        return SUCCESS_EXIT;
    }
    for (int i = 1; argv[i] != NULL; i++) {
        if (parse_and_set(shell, argv[i]) != SUCCESS_EXIT)
            return FAILURE_EXIT;
    }
    return SUCCESS_EXIT;
}
