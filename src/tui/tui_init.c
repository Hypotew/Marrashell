/*
** EPITECH PROJECT, 2026
** tui_init.c
** File description:
** ncurses init — windows, pipe, reader thread
*/

#include "tui.h"
#include "mysh.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static bool tui_create_windows(tui_t *tui)
{
    int out_h = LINES - TUI_INPUT_H;
    int out_w = COLS - TUI_SIDEBAR_W;

    tui->win_sidebar = newwin(LINES, TUI_SIDEBAR_W, 0, 0);
    tui->win_output  = newwin(out_h, out_w, 0, TUI_SIDEBAR_W);
    tui->win_input   = newwin(TUI_INPUT_H, COLS, LINES - TUI_INPUT_H, 0);
    tui->pad_output  = newpad(TUI_PAD_H, out_w);
    if (!tui->win_sidebar || !tui->win_output
        || !tui->win_input || !tui->pad_output)
        return false;
    scrollok(tui->pad_output, TRUE);
    keypad(tui->win_input, TRUE);
    return true;
}

static bool tui_init_pipe(tui_t *tui)
{
    if (pipe(tui->pipe_fds) < 0)
        return false;
    tui->orig_stdout_fd = dup(STDOUT_FILENO);
    dup2(tui->pipe_fds[1], STDOUT_FILENO);
    dup2(tui->pipe_fds[1], STDERR_FILENO);
    close(tui->pipe_fds[1]);
    tui->pipe_fds[1] = -1;
    return true;
}

tui_t *tui_init(int theme_id)
{
    tui_t *tui = calloc(1, sizeof(tui_t));
    FILE *dbg = fopen("/tmp/marrashell_tui.log", "w");

    if (!tui)
        return NULL;
    if (dbg) fprintf(dbg, "initscr LINES=%d COLS=%d\n", LINES, COLS);
    initscr();
    if (dbg) fprintf(dbg, "after initscr LINES=%d COLS=%d\n", LINES, COLS);
    cbreak();
    noecho();
    start_color();
    use_default_colors();
    tui->pipe_fds[0] = -1;
    tui->pipe_fds[1] = -1;
    tui->orig_stdout_fd = -1;
    tui->running = true;
    pthread_mutex_init(&tui->lock, NULL);
    if (!tui_create_windows(tui) || !tui_init_pipe(tui)) {
        if (dbg) { fprintf(dbg, "windows/pipe failed\n"); fclose(dbg); }
        tui_destroy(tui);
        return NULL;
    }
    if (dbg) fprintf(dbg, "windows+pipe ok\n");
    tui_apply_theme(tui, theme_id);
    if (!tui_start_reader_thread(tui)) {
        if (dbg) { fprintf(dbg, "thread failed\n"); fclose(dbg); }
        tui_destroy(tui);
        return NULL;
    }
    if (dbg) { fprintf(dbg, "tui_init SUCCESS\n"); fclose(dbg); }
    return tui;
}

void tui_destroy(tui_t *tui)
{
    if (!tui)
        return;
    tui->running = false;
    if (tui->pipe_fds[0] != -1)
        close(tui->pipe_fds[0]);
    if (tui->orig_stdout_fd != -1) {
        dup2(tui->orig_stdout_fd, STDOUT_FILENO);
        dup2(tui->orig_stdout_fd, STDERR_FILENO);
        close(tui->orig_stdout_fd);
    }
    if (tui->reader_thread)
        pthread_join(tui->reader_thread, NULL);
    pthread_mutex_destroy(&tui->lock);
    delwin(tui->win_sidebar);
    delwin(tui->win_output);
    delwin(tui->win_input);
    delwin(tui->pad_output);
    endwin();
    free(tui);
}
