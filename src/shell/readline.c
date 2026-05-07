/*
** EPITECH PROJECT, 2026
** readling
** File description:
** 42sh
*/
#include "my.h"
#include "mysh.h"
#include "shell.h"
#include "readline.h"
#include "builtins.h"
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct termios raw_mode(void)
{
    struct termios orig;
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig);
    raw = orig;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    return orig;
}

void disable_raw_mode(struct termios *orig)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig);
}

static void redraw(shell_t *shell, rl_ctx_t *ctx)
{
    char cwd[4096];
    const char *sym;

    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return;
    if (write(STDOUT_FILENO, "\r\033[K", 4) < 0)
        return;
    if (write(STDOUT_FILENO, cwd, strlen(cwd)) < 0)
        return;
    if (write(STDOUT_FILENO, " ", 1) < 0)
        return;
    sym = (shell->last_status == 0) ? SUCCESS_PROMPT : FAILURE_PROMPT;
    if (write(STDOUT_FILENO, sym, strlen(sym)) < 0)
        return;
    if (write(STDOUT_FILENO, ctx->buf, ctx->len) < 0)
        return;
}

static void navigate(shell_t *shell, rl_ctx_t *ctx, int dir)
{
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
    redraw(shell, ctx);
}

static void handle_arrow(shell_t *shell, rl_ctx_t *ctx)
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

static void handle_backspace(rl_ctx_t *ctx)
{
    if (ctx->len == 0)
        return;
    ctx->len--;
    ctx->buf[ctx->len] = '\0';
    if (write(STDOUT_FILENO, "\b \b", 3) < 0)
        return;
}

static int dispatch_char(shell_t *shell, rl_ctx_t *ctx, char c)
{
    if (c == '\n' || c == '\r') {
        if (write(STDOUT_FILENO, "\n", 1) < 0)
            return -1;
        return 1;
    }
    if (c == ARROW_UP) {
        handle_arrow(shell, ctx);
        return 0;
    }
    if (c == 127) {
        handle_backspace(ctx);
        return 0;
    }
    if (ctx->len + 2 < 1024) {
        ctx->buf[ctx->len] = c;
        ctx->len++;
        if (write(STDOUT_FILENO, &c, 1) < 0)
            return -1;
    }
    return 0;
}

static char *finalize_line(rl_ctx_t *ctx)
{
    ctx->buf[ctx->len] = '\n';
    ctx->len++;
    ctx->buf[ctx->len] = '\0';
    free_history(ctx->hist, ctx->hist_count);
    return ctx->buf;
}

char *read_line(shell_t *shell)
{
    rl_ctx_t ctx = {malloc(1024), 0, NULL, 0, 0};
    struct termios orig;
    char c;

    if (!ctx.buf)
        return NULL;
    ctx.hist = load_history(&ctx.hist_count);
    ctx.hist_idx = ctx.hist_count;
    display_prompt(shell->last_status);
    fflush(stdout);
    orig = raw_mode();
    while (read(STDIN_FILENO, &c, 1) == 1) {
        if (dispatch_char(shell, &ctx, c) == 1)
            break;
    }
    disable_raw_mode(&orig);
    return finalize_line(&ctx);
}
