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
#include <unistd.h>

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

static void fill_entries(FILE *f, char **entries, int *count)
{
    char *line = NULL;
    size_t n = 0;
    char *content;

    while (getline(&line, &n, f) != -1 && *count < MAX_HIST) {
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
    char **entries = malloc(sizeof(char *) * MAX_HIST);

    *count = 0;
    if (!f || !entries) {
        free(entries);
        if (f)
            fclose(f);
        return NULL;
    }
    fill_entries(f, entries, count);
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

static void erase_input(size_t len)
{
    for (size_t i = 0; i < len; i++)
        write(STDOUT_FILENO, "\b \b", 3);
}

static void print_input(rl_ctx_t *ctx, size_t old_len)
{
    erase_input(old_len);
    write(STDOUT_FILENO, ctx->buf, ctx->len);
}

static void navigate(shell_t *shell, rl_ctx_t *ctx, int dir)
{
    size_t old_len = ctx->len;

    (void)shell;
    if (dir < 0 && (ctx->hist_count == 0 || ctx->hist_idx == 0))
        return;
    if (dir > 0 && ctx->hist_idx >= ctx->hist_count)
        return;
    ctx->hist_idx += dir;
    if (dir > 0 && ctx->hist_idx == ctx->hist_count) {
        ctx->len = 0;
        ctx->buf[0] = '\0';
    } else {
        strncpy(ctx->buf, ctx->hist[ctx->hist_idx], 1022);
        ctx->buf[1022] = '\0';
        ctx->len = strlen(ctx->buf);
    }
    print_input(ctx, old_len);
}

void handle_arrow(shell_t *shell, rl_ctx_t *ctx)
{
    char seq[2];

    if (read(STDIN_FILENO, &seq[0], 1) != 1 || seq[0] != '[')
        return;
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
        return;
    if (seq[1] == 'A')
        navigate(shell, ctx, -1);
    if (seq[1] == 'B')
        navigate(shell, ctx, 1);
}
