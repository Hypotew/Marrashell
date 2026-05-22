/*
** EPITECH PROJECT, 2026
** tui_theme.c
** File description:
** theme data + color pair init
*/

#include "tui.h"
#include <string.h>

const theme_t G_THEMES[THEMES_COUNT] = {
    {"Dark Blue",  232, 17,  235, 156, 203, 51,  214},
    {"Gruvbox",    235, 236, 237, 142, 167, 208, 166},
    {"Catppuccin", 234, 238, 237, 183, 210, 147, 177},
    {"Nord",       236, 238, 240, 110, 174, 109, 109},
    {"Dracula",    234, 236, 237, 141, 203, 183, 212},
};

int tui_get_theme_by_name(const char *name)
{
    if (!name || !name[0])
        return -1;
    for (int i = 0; i < THEMES_COUNT; i++)
        if (strcmp(G_THEMES[i].name, name) == 0)
            return i;
    return -1;
}

void tui_init_color_pairs(const theme_t *theme)
{
    init_pair(1, theme->fg_accent, theme->bg_main);
    init_pair(2, theme->fg_error,  theme->bg_main);
    init_pair(3, theme->fg_info,   theme->bg_main);
    init_pair(4, theme->fg_cmd,    theme->bg_main);
    init_pair(5, COLOR_WHITE,      theme->bg_sidebar);
    init_pair(6, COLOR_WHITE,      theme->bg_input);
    init_pair(7, theme->fg_info,   theme->bg_sidebar);
    init_pair(8, theme->bg_main,   theme->fg_accent);
}

void tui_apply_theme(tui_t *tui, int theme_id)
{
    const theme_t *theme;

    if (theme_id < 0 || theme_id >= THEMES_COUNT)
        theme_id = 0;
    tui->theme_id = theme_id;
    theme = &G_THEMES[theme_id];
    tui_init_color_pairs(theme);
    wbkgd(tui->win_sidebar, COLOR_PAIR(5));
    wbkgd(tui->win_input,   COLOR_PAIR(6));
    wbkgd(tui->win_output,  COLOR_PAIR(3));
    wrefresh(tui->win_sidebar);
    wrefresh(tui->win_input);
    wrefresh(tui->win_output);
}
