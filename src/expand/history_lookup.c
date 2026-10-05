/*
** EPITECH PROJECT, 2026
** history_lookup.c
** File description:
** history file lookup functions for expansion
*/

#include "builtins.h"
#include "mysh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *extract_command(const char *line)
{
    char *sep = strstr(line, "  ");
    char *cmd = NULL;
    size_t len = 0;

    if (!sep)
        return NULL;
    cmd = strdup(sep + 2);
    if (!cmd)
        return NULL;
    len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n')
        cmd[len - 1] = '\0';
    return cmd;
}

static unsigned long long count_lines(void)
{
    FILE *fp = fopen(HISTORY_FILE, "r");
    char *line = NULL;
    size_t len = 0;
    unsigned long long count = 0;

    if (!fp)
        return 0;
    while (getline(&line, &len, fp) != -1)
        count++;
    free(line);
    fclose(fp);
    return count;
}

char *history_get_last(void)
{
    FILE *fp = fopen(HISTORY_FILE, "r");
    char *line = NULL;
    size_t len = 0;
    char *result = NULL;
    char *cmd = NULL;

    if (!fp)
        return NULL;
    while (getline(&line, &len, fp) != -1) {
        cmd = extract_command(line);
        if (cmd) {
            free(result);
            result = cmd;
        }
    }
    free(line);
    fclose(fp);
    return result;
}

static char *find_history_number(FILE *fp, unsigned long long n)
{
    char *line = NULL;
    size_t len = 0;
    unsigned long long num = 0;
    char *result = NULL;

    while (getline(&line, &len, fp) != -1) {
        num++;
        if (num == n) {
            result = extract_command(line);
            break;
        }
    }
    free(line);
    return result;
}

char *history_get_by_number(unsigned long long n)
{
    FILE *fp = NULL;
    char *result = NULL;

    if (n == 0)
        return NULL;
    fp = fopen(HISTORY_FILE, "r");
    if (!fp)
        return NULL;
    result = find_history_number(fp, n);
    fclose(fp);
    return result;
}

char *history_get_by_offset(unsigned long long offset)
{
    unsigned long long total = count_lines();

    if (offset == 0 || offset > total)
        return NULL;
    return history_get_by_number(total - offset + 1);
}

char *history_get_by_prefix(const char *prefix)
{
    FILE *fp = fopen(HISTORY_FILE, "r");
    char *line = NULL;
    size_t len = 0;
    char *cmd = NULL;
    char *match = NULL;

    if (!fp)
        return NULL;
    while (getline(&line, &len, fp) != -1) {
        cmd = extract_command(line);
        if (cmd && strncmp(cmd, prefix, strlen(prefix)) == 0) {
            free(match);
            match = cmd;
        } else
            free(cmd);
    }
    free(line);
    fclose(fp);
    return match;
}

char *history_get_by_substr(const char *substr)
{
    FILE *fp = fopen(HISTORY_FILE, "r");
    char *line = NULL;
    size_t len = 0;
    char *cmd = NULL;
    char *match = NULL;

    if (!fp)
        return NULL;
    while (getline(&line, &len, fp) != -1) {
        cmd = extract_command(line);
        if (cmd && strstr(cmd, substr)) {
            free(match);
            match = cmd;
        } else
            free(cmd);
    }
    free(line);
    fclose(fp);
    return match;
}
