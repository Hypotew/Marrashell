/*
** EPITECH PROJECT, 2026
** readline
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
#include <unistd.h>

static int handle_backspace(rl_ctx_t *ctx)
{
    if (ctx->len == 0)
        return 0;
    ctx->len--;
    ctx->buf[ctx->len] = '\0';
    if (write(STDOUT_FILENO, "\b \b", 3) < 0)
        return -1;
    return 0;
}

static int append_printable_char(rl_ctx_t *ctx, char c)
{
    if (ctx->len + 2 >= 1024)
        return 0;
    ctx->buf[ctx->len] = c;
    ctx->len++;
    if (write(STDOUT_FILENO, &c, 1) < 0)
        return -1;
    return 0;
}

static int dispatch_char(shell_t *shell, rl_ctx_t *ctx, char c)
{
    int control_status;

    if (c == '\n' || c == '\r') {
        if (write(STDOUT_FILENO, "\n", 1) < 0)
            return -1;
        return 1;
    }
    if (c == ARROW_UP)
        return handle_arrow(shell, ctx);
    if (c == 127)
        return handle_backspace(ctx);
    control_status = handle_control_char(shell, ctx, (unsigned char)c);
    if (control_status != 2)
        return control_status;
    return append_printable_char(ctx, c);
}

static char *finalize_line(rl_ctx_t *ctx)
{
    ctx->buf[ctx->len] = '\n';
    ctx->len++;
    ctx->buf[ctx->len] = '\0';
    free_history(ctx->hist, ctx->hist_count);
    return ctx->buf;
}

static int read_chars(shell_t *shell, rl_ctx_t *ctx)
{
    char c;
    int status;

    while (read(STDIN_FILENO, &c, 1) == 1) {
        status = dispatch_char(shell, ctx, c);
        if (status != 0)
            return status;
    }
    return 1;
}

char *read_line(shell_t *shell)
{
    rl_ctx_t ctx = {malloc(1024), 0, NULL, 0, 0};
    struct termios orig;
    int status;

    if (!ctx.buf)
        return NULL;
    ctx.hist = load_history(&ctx.hist_count);
    ctx.hist_idx = ctx.hist_count;
    fflush(stdout);
    orig = raw_mode();
    status = read_chars(shell, &ctx);
    disable_raw_mode(&orig);
    if (status == -1) {
        free_history(ctx.hist, ctx.hist_count);
        free(ctx.buf);
        return NULL;
    }
    return finalize_line(&ctx);
}
