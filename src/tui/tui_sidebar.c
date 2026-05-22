/*
** EPITECH PROJECT, 2026
** tui_sidebar.c
** File description:
** sidebar drawing — banner, theme, git, PWD, last cmd
*/

#include "tui.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

static const char *SIDEBAR_BANNER[] = {
    " __  __   _   ___ ___ _   ",
    "|  \\/  | /_\\ | _ \\ _ \\ |  ",
    "| |\\/| |/ _ \\|   /   / |__",
    "|_|  |_/_/ \\_\\_|_\\_|_\\___|",
    "  S H E L L               ",
    NULL
};

static void sidebar_draw_banner(WINDOW *win)
{
    wattron(win, COLOR_PAIR(7) | A_BOLD);
    for (int i = 0; SIDEBAR_BANNER[i]; i++)
        mvwprintw(win, i + 1, 1, "%-*s",
            TUI_SIDEBAR_W - 2, SIDEBAR_BANNER[i]);
    wattroff(win, COLOR_PAIR(7) | A_BOLD);
    mvwhline(win, 7, 1, ACS_HLINE, TUI_SIDEBAR_W - 2);
}

static void sidebar_draw_theme_info(WINDOW *win, int theme_id)
{
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, 9, 1, "Theme:");
    wattron(win, COLOR_PAIR(7) | A_BOLD);
    mvwprintw(win, 10, 2, "%-*s",
        TUI_SIDEBAR_W - 3, G_THEMES[theme_id].name);
    wattroff(win, COLOR_PAIR(7) | A_BOLD);
    wattron(win, COLOR_PAIR(5) | A_DIM);
    mvwprintw(win, 11, 2, "/theme to change");
    wattroff(win, COLOR_PAIR(5) | A_DIM);
    mvwhline(win, 12, 1, ACS_HLINE, TUI_SIDEBAR_W - 2);
}

static void sidebar_draw_status(WINDOW *win, const char *last_cmd,
    int last_status)
{
    char cwd[PATH_MAX] = {0};
    char *branch = get_branch_name();

    getcwd(cwd, PATH_MAX);
    if (branch) {
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, LINES - 8, 1, " %-*s", TUI_SIDEBAR_W - 3, branch);
        free(branch);
        wattroff(win, COLOR_PAIR(1));
    }
    wattron(win, COLOR_PAIR(5));
    mvwprintw(win, LINES - 6, 1, "PWD:");
    mvwprintw(win, LINES - 5, 2, "%-*.*s",
        TUI_SIDEBAR_W - 3, TUI_SIDEBAR_W - 3, cwd);
    mvwprintw(win, LINES - 3, 1, "Last cmd:");
    wattroff(win, COLOR_PAIR(5));
    wattron(win, last_status ? COLOR_PAIR(2) : COLOR_PAIR(1));
    mvwprintw(win, LINES - 2, 2, "%-*.*s",
        TUI_SIDEBAR_W - 3, TUI_SIDEBAR_W - 3,
        last_cmd ? last_cmd : "");
    wattroff(win, last_status ? COLOR_PAIR(2) : COLOR_PAIR(1));
}

void tui_update_sidebar(tui_t *tui, const char *last_cmd, int last_status)
{
    pthread_mutex_lock(&tui->lock);
    werase(tui->win_sidebar);
    box(tui->win_sidebar, 0, 0);
    sidebar_draw_banner(tui->win_sidebar);
    sidebar_draw_theme_info(tui->win_sidebar, tui->theme_id);
    sidebar_draw_status(tui->win_sidebar, last_cmd, last_status);
    wrefresh(tui->win_sidebar);
    pthread_mutex_unlock(&tui->lock);
}
