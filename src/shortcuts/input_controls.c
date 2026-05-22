/*
** EPITECH PROJECT, 2026
** input_controls
** File description:
** control-key shortcuts for interactive input
*/

#include "readline.h"
#include <stdio.h>
#include <unistd.h>

static int handle_ctrl_c(shell_t *shell, rl_ctx_t *ctx)
{
    if (write(STDOUT_FILENO, "\n", 1) < 0)
        return -1;
    ctx->len = 0;
    ctx->buf[0] = '\0';
    shell->last_status = 130;
    return 1;
}

static int handle_ctrl_d(rl_ctx_t *ctx)
{
    if (ctx->len == 0) {
        if (write(STDOUT_FILENO, "exit\n", 5) < 0)
            return -1;
        return -1;
    }
    return 0;
}

static int handle_ctrl_l(shell_t *shell, rl_ctx_t *ctx)
{
    (void)shell;
    if (clear_terminal() < 0)
        return -1;
    fflush(stdout);
    if (write(STDOUT_FILENO, ctx->buf, ctx->len) < 0)
        return -1;
    return 0;
}

int handle_control_char(shell_t *shell, rl_ctx_t *ctx, unsigned char c)
{
    if (c == CTRL_C)
        return handle_ctrl_c(shell, ctx);
    if (c == CTRL_D)
        return handle_ctrl_d(ctx);
    if (c == CTRL_L)
        return handle_ctrl_l(shell, ctx);
    if (c < 32)
        return 0;
    return 2;
}
