/*
** EPITECH PROJECT, 2026
** test_tui.c
** File description:
** unit tests for pure TUI functions
*/

#include <criterion/criterion.h>
#include "tui.h"

Test(tui_theme, get_by_name_known)
{
    cr_assert_eq(tui_get_theme_by_name("Dark Blue"), 0);
    cr_assert_eq(tui_get_theme_by_name("Gruvbox"), 1);
    cr_assert_eq(tui_get_theme_by_name("Catppuccin"), 2);
    cr_assert_eq(tui_get_theme_by_name("Nord"), 3);
    cr_assert_eq(tui_get_theme_by_name("Dracula"), 4);
}

Test(tui_theme, get_by_name_unknown)
{
    cr_assert_eq(tui_get_theme_by_name("solarized"), -1);
    cr_assert_eq(tui_get_theme_by_name(NULL), -1);
    cr_assert_eq(tui_get_theme_by_name(""), -1);
}

Test(tui_theme, names_not_null)
{
    for (int i = 0; i < THEMES_COUNT; i++)
        cr_assert_not_null(G_THEMES[i].name);
}
