/*
** EPITECH PROJECT, 2026
** readline_hist.c
** File description:
** 42sh
*/
#include "readline.h"
#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *parse_hist_entry(char *line)
{
    char *content = strstr(line, "  ");
    size_t len;

    if (!content)
        return NULL;
    content += 2;
    len = strlen(content);
    if (len > 0 && content[len - 1] == '\n')
        content[len - 1] = '\0';
    return strdup(content);
}

static int count_lines(FILE *f)
{
    char *line = NULL;
    size_t n = 0;
    int count = 0;

    while (getline(&line, &n, f) != -1)
        count++;
    free(line);
    rewind(f);
    return count;
}

static void fill_entries(FILE *f, char **entries, int *count, int total)
{
    char *line = NULL;
    size_t n = 0;
    char *content;

    while (getline(&line, &n, f) != -1 && *count < total) {
        content = parse_hist_entry(line);
        if (content) {
            entries[*count] = content;
            (*count)++;
        }
    }
    free(line);
}

char **load_history(int *count)
{
    FILE *f = fopen(HISTORY_FILE, "r");
    int total;
    char **entries;

    *count = 0;
    if (!f)
        return NULL;
    total = count_lines(f);
    entries = malloc(sizeof(char *) * (total + 1));
    if (!entries) {
        fclose(f);
        return NULL;
    }
    fill_entries(f, entries, count, total);
    fclose(f);
    return entries;
}

void free_history(char **entries, int count)
{
    int i;

    for (i = 0; i < count; i++)
        free(entries[i]);
    free(entries);
}
