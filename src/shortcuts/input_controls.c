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
    write(STDOUT_FILENO, "\n", 1);
    ctx->len = 0;
    ctx->buf[0] = '\0';
    shell->last_status = 130;
    return 1;
}

static int handle_ctrl_d(rl_ctx_t *ctx)
{
    if (ctx->len == 0) {
        write(STDOUT_FILENO, "exit\n", 5);
        return -1;
    }
    return 0;
}

static int handle_ctrl_l(shell_t *shell, rl_ctx_t *ctx)
{
    clear_terminal();
    display_prompt(shell->last_status);
    fflush(stdout);
    write(STDOUT_FILENO, ctx->buf, ctx->len);
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
