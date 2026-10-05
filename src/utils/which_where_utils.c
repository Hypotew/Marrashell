/*
** EPITECH PROJECT, 2026
** which_where_utils.c
** File description:
** helpers for which and where builtins
*/

#include "builtins.h"
#include "mysh.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static bool is_executable_command(const char *path)
{
    struct stat st;

    if (stat(path, &st) == -1)
        return false;
    if (S_ISDIR(st.st_mode))
        return false;
    return access(path, X_OK) == 0;
}

static char *join_path_command(const char *dir, const char *command)
{
    size_t dir_len = strlen(dir);
    size_t command_len = strlen(command);
    char *path = malloc(dir_len + command_len + 2);

    if (path == NULL)
        return NULL;
    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, command);
    return path;
}

static int print_match(const char *path, bool *found)
{
    if (printf("%s\n", path) < 0)
        return FAILURE_EXIT;
    *found = true;
    return SUCCESS_EXIT;
}

static int print_direct_match(const char *command, bool *found)
{
    if (!contains_slash(command))
        return SUCCESS_EXIT;
    if (!is_executable_command(command))
        return SUCCESS_EXIT;
    return print_match(command, found);
}

static int print_path_dirs(char **dirs, const char *command, bool print_all,
    bool *found)
{
    char *candidate = NULL;

    for (size_t i = 0; dirs[i] != NULL; i++) {
        candidate = join_path_command(dirs[i], command);
        if (candidate == NULL)
            return FAILURE_EXIT;
        if (is_executable_command(candidate)
            && print_match(candidate, found) == FAILURE_EXIT) {
            free(candidate);
            return FAILURE_EXIT;
        }
        if (*found && !print_all) {
            free(candidate);
            return SUCCESS_EXIT;
        }
        free(candidate);
    }
    return SUCCESS_EXIT;
}

static int print_missing_command(const char *command)
{
    if (fprintf(stderr, "%s: Command not found.\n", command) < 0)
        return FAILURE_EXIT;
    return FAILURE_EXIT;
}

static int get_command_dirs(shell_t *shell, char ***dirs)
{
    char *path_value = find_path_value(shell->env);

    *dirs = NULL;
    if (path_value == NULL)
        return SUCCESS_EXIT;
    *dirs = my_split_words(path_value, ":");
    if (*dirs == NULL)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int finish_match_status(const char *command, bool found,
    bool print_missing)
{
    if (!found && print_missing)
        return print_missing_command(command);
    if (!found)
        return SUCCESS_EXIT;
    return found ? SUCCESS_EXIT : FAILURE_EXIT;
}

int print_path_matches(shell_t *shell, const char *command, bool print_all,
    bool print_missing)
{
    char **dirs = NULL;
    bool found = false;
    int status = SUCCESS_EXIT;

    if (print_direct_match(command, &found) == FAILURE_EXIT)
        return FAILURE_EXIT;
    if (found || contains_slash(command))
        return finish_match_status(command, found, print_missing);
    if (get_command_dirs(shell, &dirs) == FAILURE_EXIT)
        return FAILURE_EXIT;
    if (dirs != NULL)
        status = print_path_dirs(dirs, command, print_all, &found);
    free_string_array(dirs);
    if (status == FAILURE_EXIT)
        return FAILURE_EXIT;
    return finish_match_status(command, found, print_missing);
}
