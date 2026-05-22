/*
** EPITECH PROJECT, 2026
** tui_output.c
** File description:
** stdout pipe reader thread + pad scroll
*/

#include "tui.h"
#include <unistd.h>

// Appelé avec tui->lock déjà tenu par l'appelant
static void tui_pad_refresh(tui_t *tui)
{
    int out_h = LINES - TUI_INPUT_H;
    int start = tui->pad_lines - out_h - tui->scroll_offset;

    if (start < 0)
        start = 0;
    prefresh(tui->pad_output, start, 0,
        0, TUI_SIDEBAR_W, LINES - TUI_INPUT_H - 1, COLS - 1);
}

static void *output_reader_loop(void *arg)
{
    tui_t *tui = (tui_t *)arg;
    char buf[512];
    ssize_t n;
    ssize_t i;

    while (tui->running) {
        n = read(tui->pipe_fds[0], buf, sizeof(buf));
        if (n <= 0)
            break;
        pthread_mutex_lock(&tui->lock);
        for (i = 0; i < n; i++) {
            waddch(tui->pad_output, (unsigned char)buf[i]);
            if (buf[i] == '\n')
                tui->pad_lines++;
        }
        tui_pad_refresh(tui);
        pthread_mutex_unlock(&tui->lock);
    }
    return NULL;
}

bool tui_start_reader_thread(tui_t *tui)
{
    return pthread_create(&tui->reader_thread, NULL,
        output_reader_loop, tui) == 0;
}

void tui_output_scroll(tui_t *tui, int delta)
{
    int out_h = LINES - TUI_INPUT_H;
    int max_scroll;

    pthread_mutex_lock(&tui->lock);
    max_scroll = tui->pad_lines - out_h;
    if (max_scroll < 0)
        max_scroll = 0;
    tui->scroll_offset += delta;
    if (tui->scroll_offset < 0)
        tui->scroll_offset = 0;
    if (tui->scroll_offset > max_scroll)
        tui->scroll_offset = max_scroll;
    tui_pad_refresh(tui);
    pthread_mutex_unlock(&tui->lock);
}

void tui_output_reset_scroll(tui_t *tui)
{
    pthread_mutex_lock(&tui->lock);
    tui->scroll_offset = 0;
    tui_pad_refresh(tui);
    pthread_mutex_unlock(&tui->lock);
}
