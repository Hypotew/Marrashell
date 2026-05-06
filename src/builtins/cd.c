/*
** EPITECH PROJECT, 2026
** cd.c
** File description:
** cd replica
*/

#include "builtins.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

static int sync_pwd_vars(shell_t *shell, const char *oldpwd)
{
    char newpwd[PATH_MAX];

    if (env_set_value(shell, "OLDPWD", oldpwd) != SUCCESS_EXIT)
        return FAILURE_EXIT;
    if (getcwd(newpwd, PATH_MAX) == NULL)
        return SUCCESS_EXIT;
    if (env_set_value(shell, "PWD", newpwd) != SUCCESS_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

bool is_a_dir(const char *target)
{
    struct stat st;

    if (stat(target, &st) == -1)
        return false;
    if (!S_ISDIR(st.st_mode)) {
        if (fprintf(stderr, "%s", target) < 0)
            return false;
        if (fprintf(stderr, "%s", ": Not a directory.") < 0)
            return false;
        return false;
    }
    return true;
}

static int handle_chdir_error(const char *target)
{
    int err = errno;

    perror(target);
    if (err == ENOENT)
        return SUCCESS_EXIT;
    return FAILURE_EXIT;
}

static int cd_to(shell_t *shell, const char *target)
{
    char oldpwd[PATH_MAX];

    if (getcwd(oldpwd, PATH_MAX) == NULL)
        return FAILURE_EXIT;
    if (!is_a_dir(target))
        return SUCCESS_EXIT;
    if (chdir(target) == -1)
        return handle_chdir_error(target);
    if (sync_pwd_vars(shell, oldpwd) != SUCCESS_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int cd_home(shell_t *shell)
{
    char *home_path = env_get_value(shell->env, "HOME=");

    if (home_path == NULL || home_path[0] == '\0') {
        if (fprintf(stderr, "cd: HOME not set\n") < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    return cd_to(shell, home_path);
}

int my_cd(shell_t *shell, char **argv,
    __attribute__((unused)) bool *should_exit)
{
    size_t arg_nb = string_array_len(argv);
    char *oldpwd;

    if (arg_nb > 2) {
        if (fprintf(stderr, "cd: Too many arguments\n") < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    if (arg_nb == 1)
        return cd_home(shell);
    if (strcmp(argv[1], "-") == 0) {
        oldpwd = env_get_value(shell->env, "OLDPWD=");
        if (!oldpwd) {
            fprintf(stderr, "cd: OLDPWD not set\n");
            return FAILURE_EXIT;
        }
        return cd_to(shell, oldpwd);
    }
    return cd_to(shell, argv[1]);
}
