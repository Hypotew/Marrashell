/*
** EPITECH PROJECT, 2026
** tui_input.c
** File description:
** boucle wgetch, rendu prompt, lecture ligne interactive
*/

#include "shell.h"
#include "tui.h"
#include "builtins.h"
#include <stdlib.h>
#include <string.h>

static void input_ctx_init(input_ctx_t *ctx)
{
    memset(ctx->buf, 0, sizeof(ctx->buf));
    ctx->len = 0;
    ctx->cursor = 0;
    ctx->hist = load_history(&ctx->hist_count);
    ctx->hist_idx = ctx->hist_count;
}

static void input_render(tui_t *tui, input_ctx_t *ctx, int last_status)
{
    int pair = last_status ? 2 : 1;
    const char *prompt = last_status ? "✗ ❯ " : "✓ ❯ ";

    pthread_mutex_lock(&tui->lock);
    werase(tui->win_input);
    box(tui->win_input, 0, 0);
    wattron(tui->win_input, COLOR_PAIR(pair) | A_BOLD);
    mvwprintw(tui->win_input, 1, 2, "%s", prompt);
    wattroff(tui->win_input, COLOR_PAIR(pair) | A_BOLD);
    wattron(tui->win_input, COLOR_PAIR(6));
    mvwprintw(tui->win_input, 1, 8, "%s", ctx->buf);
    wmove(tui->win_input, 1, 8 + ctx->cursor);
    wrefresh(tui->win_input);
    pthread_mutex_unlock(&tui->lock);
}

static bool input_handle_printable(input_ctx_t *ctx, int ch)
{
    if (ch == '\n' || ch == '\r')
        return true;
    if (ch >= 32 && ch < 127 && ctx->len < 4093) {
        memmove(ctx->buf + ctx->cursor + 1,
            ctx->buf + ctx->cursor,
            ctx->len - ctx->cursor + 1);
        ctx->buf[ctx->cursor] = (char)ch;
        ctx->cursor++;
        ctx->len++;
    }
    return false;
}

static void input_dispatch(shell_t *shell, input_ctx_t *ctx, int ch)
{
    if (ch == KEY_UP)
        input_handle_history(ctx, -1);
    if (ch == KEY_DOWN)
        input_handle_history(ctx, 1);
    input_handle_edit(ctx, ch);
    input_handle_ctrl(shell, ctx, ch);
    input_handle_scroll(shell, ch);
}

char *tui_read_line(shell_t *shell)
{
    input_ctx_t ctx;
    int ch;

    input_ctx_init(&ctx);
    while (1) {
        input_render(shell->tui, &ctx, shell->last_status);
        ch = wgetch(shell->tui->win_input);
        if (ch == 4 && ctx.len == 0) {
            free_history(ctx.hist, ctx.hist_count);
            return NULL;
        }
        input_dispatch(shell, &ctx, ch);
        if (input_handle_printable(&ctx, ch))
            break;
    }
    free_history(ctx.hist, ctx.hist_count);
    if (strcmp(ctx.buf, "/theme") == 0) {
        tui_open_theme_menu(shell);
        return strdup("");
    }
    tui_output_reset_scroll(shell->tui);
    return strdup(ctx.buf);
}
