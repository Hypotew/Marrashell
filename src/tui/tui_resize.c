/*
** EPITECH PROJECT, 2026
** tui_resize.c
** File description:
** SIGWINCH handler — recalcul des fenêtres ncurses
*/

#include "tui.h"
#include <signal.h>
#include <stdlib.h>

static tui_t *g_tui_for_resize = NULL;

static void sigwinch_handler(int sig)
{
    int out_h;
    int out_w;

    (void)sig;
    if (!g_tui_for_resize)
        return;
    endwin();
    refresh();
    out_h = LINES - TUI_INPUT_H;
    out_w = COLS  - TUI_SIDEBAR_W;
    wresize(g_tui_for_resize->win_sidebar, LINES, TUI_SIDEBAR_W);
    wresize(g_tui_for_resize->win_output,  out_h, out_w);
    wresize(g_tui_for_resize->win_input,   TUI_INPUT_H, COLS);
    mvwin(g_tui_for_resize->win_input,  LINES - TUI_INPUT_H, 0);
    clearok(stdscr, TRUE);
    refresh();
}

void tui_register_resize_handler(tui_t *tui)
{
    g_tui_for_resize = tui;
    signal(SIGWINCH, sigwinch_handler);
}
