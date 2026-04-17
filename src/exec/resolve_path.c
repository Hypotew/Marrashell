/*
** EPITECH PROJECT, 2026
** resolve_path.c
** File description:
** external command path resolution helpers
*/

#include "exec.h"
#include "my.h"
#include "mysh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void print_command_not_found(const char *command)
{
    fprintf(stderr, "%s: Command not found.\n", command);
}

static void print_cmd_error(const char *command, const char *message)
{
    fprintf(stderr, "%s: %s\n", command, message);
}

void print_path_not_set(void)
{
    fprintf(stderr, "mysh: PATH not set\n");
}

bool contains_slash(const char *str)
{
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '/')
            return true;
    }
    return false;
}

char *find_path_value(char **env)
{
    for (size_t i = 0; env[i] != NULL; ++i) {
        if (strncmp(env[i], "PATH=", 5) == 0)
            return env[i] + 5;
    }
    return NULL;
}

static char *join_exec_path(const char *dir, const char *exe)
{
    size_t dir_len = strlen(dir);
    size_t exe_len = strlen(exe);
    char *full_path = malloc(dir_len + exe_len + 2);

    if (full_path == NULL)
        return NULL;
    strcpy(full_path, dir);
    strcat(full_path, "/");
    strcat(full_path, exe);
    return full_path;
}

static char *find_exe_in_dirs(char **dirs, char *exe)
{
    char *candidate = NULL;

    for (size_t i = 0; dirs[i] != NULL; ++i) {
        candidate = join_exec_path(dirs[i], exe);
        if (candidate == NULL)
            break;
        if (access(candidate, X_OK) == 0)
            return candidate;
        free(candidate);
    }
    return NULL;
}

char *build_exec_path(char **env, char *exe)
{
    char *path_value = find_path_value(env);
    char **dirs = NULL;
    char *resolved = NULL;

    if (contains_slash(exe))
        return strdup(exe);
    if (path_value == NULL)
        return NULL;
    dirs = my_split_words(path_value, ":");
    if (dirs == NULL)
        return NULL;
    resolved = find_exe_in_dirs(dirs, exe);
    free_string_array(dirs);
    return resolved;
}

static int handle_missing_target(char *command, bool from_path)
{
    if (from_path)
        print_command_not_found(command);
    else
        print_cmd_error(command, "No such file or directory");
    return FAILURE_EXIT;
}

int validate_exec_target(char *command, char *exe_path, bool from_path)
{
    struct stat st;

    if (exe_path == NULL)
        return handle_missing_target(command, from_path);
    if (stat(exe_path, &st) == -1) {
        print_cmd_error(command, "No such file or directory");
        return FAILURE_EXIT;
    }
    if (S_ISDIR(st.st_mode)) {
        print_cmd_error(command, "Permission denied.");
        return FAILURE_EXIT;
    }
    if (access(exe_path, X_OK) == -1) {
        print_cmd_error(command, "Permission denied.");
        return FAILURE_EXIT;
    }
    return SUCCESS_EXIT;
}
