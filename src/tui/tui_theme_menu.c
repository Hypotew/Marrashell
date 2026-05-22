/*
** EPITECH PROJECT, 2026
** tui_theme_menu.c
** File description:
** overlay interactif /theme — sélection et persistance
*/

#include "shell.h"
#include "tui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void theme_save_to_rc(const char *name)
{
    char path[512];
    const char *home = getenv("HOME");
    FILE *f;

    if (!home)
        return;
    snprintf(path, sizeof(path), "%s/.42shrc", home);
    f = fopen(path, "w");
    if (!f)
        return;
    fprintf(f, "set theme=%s\n", name);
    fclose(f);
}

static void theme_menu_draw(int selected)
{
    int w = TUI_SIDEBAR_W + 16;
    int h = THEMES_COUNT + 4;
    int y0 = (LINES - h) / 2;
    int x0 = (COLS - w) / 2;
    WINDOW *win = newwin(h, w, y0, x0);

    if (!win)
        return;
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Choisir un theme  ↑↓ Enter Esc");
    mvwhline(win, 2, 1, ACS_HLINE, w - 2);
    for (int i = 0; i < THEMES_COUNT; i++) {
        if (i == selected)
            wattron(win, A_REVERSE);
        mvwprintw(win, 3 + i, 3, "%-*s",
            w - 6, G_THEMES[i].name);
        wattroff(win, A_REVERSE);
    }
    wrefresh(win);
    delwin(win);
}

static int theme_menu_handle_key(int *selected, int ch)
{
    if (ch == KEY_UP && *selected > 0)
        (*selected)--;
    if (ch == KEY_DOWN && *selected < THEMES_COUNT - 1)
        (*selected)++;
    if (ch == '\n' || ch == '\r')
        return 1;
    if (ch == 27)
        return -1;
    return 0;
}

void tui_open_theme_menu(shell_t *shell)
{
    int selected = shell->tui->theme_id;
    int done = 0;
    int ch;

    while (!done) {
        theme_menu_draw(selected);
        ch = wgetch(shell->tui->win_input);
        done = theme_menu_handle_key(&selected, ch);
    }
    if (done == 1) {
        tui_apply_theme(shell->tui, selected);
        theme_save_to_rc(G_THEMES[selected].name);
        tui_update_sidebar(shell->tui,
            shell->line, shell->last_status);
    }
    touchwin(stdscr);
    refresh();
}
