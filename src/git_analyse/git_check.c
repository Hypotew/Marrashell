/*
** EPITECH PROJECT, 2026
** git_check.c
** File description:
** all functions that handle the display of the current branch
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mysh.h"
#include "shell.h"
#include "tui.h"

bool is_in_repository(void)
{
    FILE *fd = fopen("./.git/HEAD", "r");

    if (!fd)
        return false;
    fclose(fd);
    return true;
}

static FILE *open_branch_dir(void)
{
    FILE *fd = fopen("./.git/HEAD", "r");

    if (!fd)
        return NULL;
    return fd;
}

static void safe_clean(FILE **fd, char **line)
{
    if (line && *line) {
        free(*line);
        *line = NULL;
    }
    if (fd && *fd) {
        fclose(*fd);
        *fd = NULL;
    }
}

int display_branch(void)
{
    char *line = NULL;
    size_t len = 0;
    char *branch = NULL;
    FILE *fd = open_branch_dir();

    if (!fd)
        return FAILURE_EXIT;
    if (getline(&line, &len, fd) == -1) {
        safe_clean(&fd, &line);
        return FAILURE_EXIT;
    }
    if (strncmp(line, "ref: refs/heads/", strlen("ref: refs/heads/")) == 0) {
        branch = line + strlen("ref: refs/heads/");
        if (printf("current branch: %s", branch) < 0) {
            safe_clean(&fd, &line);
            return FAILURE_EXIT;
        }
    }
    safe_clean(&fd, &line);
    return SUCCESS_EXIT;
}

char *get_branch_name(void)
{
    char *line = NULL;
    size_t len = 0;
    char *branch = NULL;
    FILE *fd = open_branch_dir();

    if (!fd)
        return NULL;
    if (getline(&line, &len, fd) == -1) {
        safe_clean(&fd, &line);
        return NULL;
    }
    fclose(fd);
    if (strncmp(line, "ref: refs/heads/",
            strlen("ref: refs/heads/")) != 0) {
        free(line);
        return NULL;
    }
    branch = strdup(line + strlen("ref: refs/heads/"));
    free(line);
    if (branch && branch[0] != '\0' && branch[strlen(branch) - 1] == '\n')
        branch[strlen(branch) - 1] = '\0';
    return branch;
}
