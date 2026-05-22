/*
** EPITECH PROJECT, 2026
** tui_input_keys.c
** File description:
** key handlers pour la barre input (historique, édition, ctrl, scroll)
*/

#include "shell.h"
#include "tui.h"
#include <string.h>
#include <stdlib.h>

void input_handle_history(input_ctx_t *ctx, int dir)
{
    if (dir < 0 && ctx->hist_idx == 0)
        return;
    if (dir > 0 && ctx->hist_idx >= ctx->hist_count)
        return;
    ctx->hist_idx += dir;
    if (ctx->hist_idx == ctx->hist_count) {
        ctx->buf[0] = '\0';
        ctx->len = 0;
        ctx->cursor = 0;
    } else {
        strncpy(ctx->buf, ctx->hist[ctx->hist_idx], 4094);
        ctx->buf[4094] = '\0';
        ctx->len = strlen(ctx->buf);
        ctx->cursor = ctx->len;
    }
}

void input_handle_edit(input_ctx_t *ctx, int ch)
{
    if ((ch == KEY_BACKSPACE || ch == 127) && ctx->cursor > 0) {
        memmove(ctx->buf + ctx->cursor - 1,
            ctx->buf + ctx->cursor,
            ctx->len - ctx->cursor + 1);
        ctx->cursor--;
        ctx->len--;
    }
    if (ch == KEY_LEFT && ctx->cursor > 0)
        ctx->cursor--;
    if (ch == KEY_RIGHT && ctx->cursor < ctx->len)
        ctx->cursor++;
    if (ch == KEY_HOME)
        ctx->cursor = 0;
    if (ch == KEY_END)
        ctx->cursor = ctx->len;
}

void input_handle_ctrl(shell_t *shell, input_ctx_t *ctx, int ch)
{
    if (ch == 3) {
        ctx->buf[0] = '\0';
        ctx->len = 0;
        ctx->cursor = 0;
        shell->last_status = 130;
    }
    if (ch == 12) {
        clear();
        refresh();
    }
}

void input_handle_scroll(shell_t *shell, int ch)
{
    int step = LINES - TUI_INPUT_H - 1;

    if (ch == KEY_PPAGE)
        tui_output_scroll(shell->tui, step);
    if (ch == KEY_NPAGE)
        tui_output_scroll(shell->tui, -step);
}
