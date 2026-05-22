# Marrashell — ncurses TUI Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remplacer la couche I/O du shell par un TUI ncurses à 3 zones (sidebar gauche, output scrollable, barre input) avec 5 thèmes et overlay `/theme`.

**Architecture:** Toute la couche d'exécution (parser/exec/expand/builtins/env) reste intacte. Seule la couche I/O change : `src/shortcuts/` est supprimé et remplacé par `src/tui/`. `stdout` est redirigé vers un pipe interne lu par un thread qui écrit dans un ncurses pad scrollable.

**Tech Stack:** C, ncurses, pthreads, Criterion (tests)

---

## Fichiers — vue d'ensemble

### Créés
- `include/tui.h` — types (`tui_t`, `theme_t`, `input_ctx_t`) + prototypes
- `src/tui/tui_init.c` — `initscr()`, création WINDOWs, pipe stdout, démarrage thread
- `src/tui/tui_theme.c` — `G_THEMES[5]`, color pairs, lookup par nom
- `src/tui/tui_output.c` — thread reader pipe→pad, scroll
- `src/tui/tui_sidebar.c` — dessin bannière, thème, git, PWD, dernière cmd
- `src/tui/tui_input.c` — boucle `wgetch`, rendu prompt, init contexte
- `src/tui/tui_input_keys.c` — handlers touches (historique, édition, ctrl, scroll)
- `src/tui/tui_theme_menu.c` — overlay `/theme` (↑↓ Enter Esc), save `~/.42shrc`
- `src/tui/tui_resize.c` — handler SIGWINCH, recalcul WINDOWs
- `src/utils/history_load.c` — `load_history()`, `free_history()` (déplacés depuis shortcuts)
- `tests/test_tui.c` — tests Criterion pour fonctions pures TUI

### Modifiés
- `include/shell.h` — struct `s_shell` (tag pour forward-decl) + champ `tui_t *tui`
- `include/builtins.h` — déclarations `load_history()`, `free_history()`
- `src/shell/init.c` — appel `tui_init()` + lecture `~/.42shrc`
- `src/shell/loop.c` — supprime `display_marrashell()`, utilise TUI
- `src/git_analyse/git_check.c` — ajoute `get_branch_name()` (retourne `char *`)
- `Makefile` — ajoute `src/tui/*.c` + `src/utils/history_load.c`, `-lncurses -lpthread`, retire `src/shortcuts/*.c`

### Supprimés
- `src/shortcuts/terminal_mode.c`
- `src/shortcuts/input_controls.c`
- `src/shortcuts/interactive_inputs.c`
- `src/shortcuts/history_navigation.c`
- `include/readline.h`

---

## Task 1 : `include/tui.h`

**Files:**
- Create: `include/tui.h`

- [ ] **Écrire `include/tui.h`**

```c
/*
** EPITECH PROJECT, 2026
** tui.h
** File description:
** TUI ncurses — types et API
*/

#ifndef TUI_H_
    #define TUI_H_

    #include <ncurses.h>
    #include <pthread.h>
    #include <stdbool.h>
    #include <stddef.h>

    #define TUI_SIDEBAR_W   28
    #define TUI_INPUT_H     3
    #define TUI_PAD_H       2000
    #define THEMES_COUNT    5

// Forward-déclaration pour éviter l'include circulaire avec shell.h
typedef struct s_shell shell_t;

typedef struct {
    const char *name;
    short bg_main;
    short bg_sidebar;
    short bg_input;
    short fg_accent;
    short fg_error;
    short fg_info;
    short fg_cmd;
} theme_t;

typedef struct {
    char buf[4096];
    int len;
    int cursor;
    char **hist;
    int hist_count;
    int hist_idx;
} input_ctx_t;

typedef struct s_tui {
    WINDOW *win_sidebar;
    WINDOW *win_output;
    WINDOW *win_input;
    WINDOW *pad_output;
    int scroll_offset;
    int pad_lines;
    int theme_id;
    int pipe_fds[2];
    int orig_stdout_fd;
    pthread_t reader_thread;
    pthread_mutex_t lock;
    bool running;
} tui_t;

extern const theme_t G_THEMES[THEMES_COUNT];

// tui_init.c
tui_t *tui_init(int theme_id);
void tui_destroy(tui_t *tui);

// tui_theme.c
void tui_init_color_pairs(const theme_t *theme);
void tui_apply_theme(tui_t *tui, int theme_id);
int tui_get_theme_by_name(const char *name);

// tui_output.c
void tui_output_scroll(tui_t *tui, int delta);
void tui_output_reset_scroll(tui_t *tui);
bool tui_start_reader_thread(tui_t *tui);

// tui_sidebar.c
void tui_update_sidebar(tui_t *tui, const char *last_cmd, int last_status);

// tui_input.c
char *tui_read_line(shell_t *shell);

// tui_theme_menu.c
void tui_open_theme_menu(shell_t *shell);

// tui_resize.c
void tui_register_resize_handler(tui_t *tui);

// git_check.c (ajout)
char *get_branch_name(void);

#endif // !TUI_H_
```

- [ ] **Vérifier la compilation du header isolément**

```bash
gcc -Wall -Wextra -I./include -c /dev/null -include include/tui.h -o /dev/null
```
Attendu : aucun warning, aucune erreur.

- [ ] **Commit**

```bash
git add include/tui.h
git commit -m "feat(tui): add tui.h — types and full API declarations"
```

---

## Task 2 : `src/tui/tui_theme.c`

**Files:**
- Create: `src/tui/tui_theme.c`
- Create: `tests/test_tui.c`

- [ ] **Écrire le test avant le code**

```c
// tests/test_tui.c
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
```

- [ ] **Ajouter `test_tui.c` au Makefile (TEST_SRCS) pour compilation**

Dans `Makefile`, ajouter `tests/test_tui.c` et `$(SRCS_DIR)/tui/tui_theme.c` à `TEST_SRCS`.

- [ ] **Lancer les tests → FAIL attendu**

```bash
make tests_run 2>&1 | head -20
```
Attendu : erreur de compilation (`tui_theme.c` n'existe pas encore).

- [ ] **Écrire `src/tui/tui_theme.c`**

```c
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
```

- [ ] **Lancer les tests → PASS attendu**

```bash
make tests_run 2>&1 | grep -E "PASSED|FAILED|tui_theme"
```
Attendu : 3 tests `tui_theme` PASSED.

- [ ] **Commit**

```bash
git add src/tui/tui_theme.c tests/test_tui.c Makefile
git commit -m "feat(tui): add theme data, color pairs, and unit tests"
```

---

## Task 3 : `src/utils/history_load.c` + mise à jour `builtins.h`

Ces fonctions sont déplacées depuis `src/shortcuts/history_navigation.c` qui sera supprimé.

**Files:**
- Create: `src/utils/history_load.c`
- Modify: `include/builtins.h`

- [ ] **Créer `src/utils/history_load.c`**

```c
/*
** EPITECH PROJECT, 2026
** history_load.c
** File description:
** load / free history array from .42sh_history
*/

#include "builtins.h"
#include "readline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *parse_hist_entry(char *line)
{
    char *content = strstr(line, "  ");
    size_t len;

    if (!content)
        return NULL;
    content += 2;
    len = strlen(content);
    if (len > 0 && content[len - 1] == '\n')
        content[len - 1] = '\0';
    return strdup(content);
}

static void fill_entries(FILE *f, char **entries, int *count)
{
    char *line = NULL;
    size_t n = 0;
    char *content;

    while (getline(&line, &n, f) != -1 && *count < MAX_HIST) {
        content = parse_hist_entry(line);
        if (content)
            entries[(*count)++] = content;
    }
    free(line);
}

char **load_history(int *count)
{
    FILE *f = fopen(HISTORY_FILE, "r");
    char **entries = malloc(sizeof(char *) * MAX_HIST);

    *count = 0;
    if (!f || !entries) {
        free(entries);
        if (f)
            fclose(f);
        return NULL;
    }
    fill_entries(f, entries, count);
    fclose(f);
    return entries;
}

void free_history(char **entries, int count)
{
    for (int i = 0; i < count; i++)
        free(entries[i]);
    free(entries);
}
```

> **Note :** `parse_hist_entry` reste ici car elle est déclarée dans `builtins.h` et utilisée par `src/builtins/history.c`. La définition précédente dans `history_navigation.c` est supprimée à la Task 13.

- [ ] **Ajouter les déclarations dans `include/builtins.h`**

Après la ligne `char *parse_hist_entry(char *line);`, ajouter :
```c
char **load_history(int *count);
void free_history(char **entries, int count);
```

- [ ] **Retirer l'include de `readline.h` dans `history_load.c`**

`history_load.c` inclut `readline.h` uniquement pour `MAX_HIST` et `HISTORY_FILE`. Ces constantes sont dans `builtins.h` (`#define HISTORY_FILE ".42sh_history"`) et `readline.h` (`#define MAX_HIST 1000`). À ce stade `readline.h` existe encore, donc pas de problème. À la Task 13 on remplacera cet include par les définitions directes dans `builtins.h`.

- [ ] **Commit**

```bash
git add src/utils/history_load.c include/builtins.h
git commit -m "feat(tui): add history_load.c, declare load/free_history in builtins.h"
```

---

## Task 4 : `src/git_analyse/git_check.c` — ajout `get_branch_name()`

**Files:**
- Modify: `src/git_analyse/git_check.c`

- [ ] **Ajouter `get_branch_name()` dans `git_check.c`**

À la fin du fichier (après `display_branch`), ajouter :

```c
char *get_branch_name(void)
{
    char *line = NULL;
    size_t len = 0;
    char *branch;
    FILE *fd = open_branch_dir();

    if (!fd)
        return NULL;
    if (getline(&line, &len, fd) == -1) {
        safe_clean(&fd, &line);
        return NULL;
    }
    fclose(fd);
    if (strncmp(line, "ref: refs/heads/",
            strlen("ref: refs/heads/")) != 0) {
        free(line);
        return NULL;
    }
    branch = strdup(line + strlen("ref: refs/heads/"));
    free(line);
    if (branch && branch[strlen(branch) - 1] == '\n')
        branch[strlen(branch) - 1] = '\0';
    return branch;
}
```

Ajouter la déclaration dans `include/tui.h` (déjà fait en Task 1).

- [ ] **Vérifier que `make` compile toujours (avant la suite)**

```bash
make 2>&1 | tail -5
```
Attendu : `42sh` se recompile sans erreur.

- [ ] **Commit**

```bash
git add src/git_analyse/git_check.c
git commit -m "feat(tui): add get_branch_name() returning malloc'd branch string"
```

---

## Task 5 : `src/tui/tui_init.c`

**Files:**
- Create: `src/tui/tui_init.c`

- [ ] **Écrire `src/tui/tui_init.c`**

```c
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

    if (!tui)
        return NULL;
    initscr();
    cbreak();
    noecho();
    start_color();
    use_default_colors();
    tui->pipe_fds[0] = -1;
    tui->pipe_fds[1] = -1;
    tui->running = true;
    pthread_mutex_init(&tui->lock, NULL);
    if (!tui_create_windows(tui) || !tui_init_pipe(tui)) {
        tui_destroy(tui);
        return NULL;
    }
    tui_apply_theme(tui, theme_id);
    if (!tui_start_reader_thread(tui)) {
        tui_destroy(tui);
        return NULL;
    }
    return tui;
}

void tui_destroy(tui_t *tui)
{
    if (!tui)
        return;
    tui->running = false;
    if (tui->pipe_fds[0] != -1)
        close(tui->pipe_fds[0]);
    if (tui->orig_stdout_fd != 0) {
        dup2(tui->orig_stdout_fd, STDOUT_FILENO);
        dup2(tui->orig_stdout_fd, STDERR_FILENO);
        close(tui->orig_stdout_fd);
    }
    pthread_join(tui->reader_thread, NULL);
    pthread_mutex_destroy(&tui->lock);
    delwin(tui->win_sidebar);
    delwin(tui->win_output);
    delwin(tui->win_input);
    delwin(tui->pad_output);
    endwin();
    free(tui);
}
```

- [ ] **Commit**

```bash
git add src/tui/tui_init.c
git commit -m "feat(tui): add tui_init/destroy — ncurses windows, pipe, mutex"
```

---

## Task 6 : `src/tui/tui_output.c`

**Files:**
- Create: `src/tui/tui_output.c`

- [ ] **Écrire `src/tui/tui_output.c`**

```c
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
        n = read(tui->pipe_fds[0], buf, sizeof(buf) - 1);
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
    int max_scroll = tui->pad_lines - out_h;

    if (max_scroll < 0)
        max_scroll = 0;
    tui->scroll_offset += delta;
    if (tui->scroll_offset < 0)
        tui->scroll_offset = 0;
    if (tui->scroll_offset > max_scroll)
        tui->scroll_offset = max_scroll;
    pthread_mutex_lock(&tui->lock);
    tui_pad_refresh(tui);
    pthread_mutex_unlock(&tui->lock);
}

void tui_output_reset_scroll(tui_t *tui)
{
    tui->scroll_offset = 0;
    pthread_mutex_lock(&tui->lock);
    tui_pad_refresh(tui);
    pthread_mutex_unlock(&tui->lock);
}
```

- [ ] **Commit**

```bash
git add src/tui/tui_output.c
git commit -m "feat(tui): add output reader thread and scroll functions"
```

---

## Task 7 : `src/tui/tui_sidebar.c`

**Files:**
- Create: `src/tui/tui_sidebar.c`

- [ ] **Écrire `src/tui/tui_sidebar.c`**

```c
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
    wattron(win, last_status ? COLOR_PAIR(2) : COLOR_PAIR(1));
    mvwprintw(win, LINES - 2, 2, "%-*.*s",
        TUI_SIDEBAR_W - 3, TUI_SIDEBAR_W - 3,
        last_cmd ? last_cmd : "");
    wattroff(win, COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(5));
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
```

- [ ] **Commit**

```bash
git add src/tui/tui_sidebar.c
git commit -m "feat(tui): add sidebar drawing — banner, theme info, git/PWD/last cmd"
```

---

## Task 8 : `src/tui/tui_input_keys.c`

**Files:**
- Create: `src/tui/tui_input_keys.c`

- [ ] **Écrire `src/tui/tui_input_keys.c`**

```c
/*
** EPITECH PROJECT, 2026
** tui_input_keys.c
** File description:
** key handlers pour la barre input (historique, édition, ctrl, scroll)
*/

#include "tui.h"
#include "shell.h"
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
```

- [ ] **Commit**

```bash
git add src/tui/tui_input_keys.c
git commit -m "feat(tui): add key handlers for history, editing, ctrl, scroll"
```

---

## Task 9 : `src/tui/tui_input.c`

**Files:**
- Create: `src/tui/tui_input.c`

- [ ] **Écrire `src/tui/tui_input.c`**

```c
/*
** EPITECH PROJECT, 2026
** tui_input.c
** File description:
** boucle wgetch, rendu prompt, lecture ligne interactive
*/

#include "tui.h"
#include "shell.h"
#include "builtins.h"
#include <stdlib.h>
#include <string.h>

static void input_ctx_init(input_ctx_t *ctx)
{
    memset(ctx->buf, 0, sizeof(ctx->buf));
    ctx->len = 0;
    ctx->cursor = 0;
    ctx->hist = NULL;
    ctx->hist_count = 0;
    ctx->hist_idx = 0;
}

static void input_ctx_load_hist(input_ctx_t *ctx)
{
    ctx->hist = load_history(&ctx->hist_count);
    ctx->hist_idx = ctx->hist_count;
}

static void input_render(tui_t *tui, input_ctx_t *ctx, int last_status)
{
    int pair = last_status ? 2 : 1;

    pthread_mutex_lock(&tui->lock);
    werase(tui->win_input);
    box(tui->win_input, 0, 0);
    wattron(tui->win_input, COLOR_PAIR(pair) | A_BOLD);
    mvwprintw(tui->win_input, 1, 2, "%s", last_status ? "✗ ❯ " : "✓ ❯ ");
    wattroff(tui->win_input, COLOR_PAIR(pair) | A_BOLD);
    wattron(tui->win_input, COLOR_PAIR(6));
    mvwprintw(tui->win_input, 1, 8, "%s", ctx->buf);
    wmove(tui->win_input, 1, 8 + ctx->cursor);
    wrefresh(tui->win_input);
    pthread_mutex_unlock(&tui->lock);
}

static bool input_handle_printable(input_ctx_t *ctx, int ch)
{
    if (ch == '\n' || ch == '\r')
        return true;
    if (ch >= 32 && ch < 127 && ctx->len < 4093) {
        memmove(ctx->buf + ctx->cursor + 1,
            ctx->buf + ctx->cursor,
            ctx->len - ctx->cursor + 1);
        ctx->buf[ctx->cursor] = (char)ch;
        ctx->cursor++;
        ctx->len++;
    }
    return false;
}

char *tui_read_line(shell_t *shell)
{
    input_ctx_t ctx;
    int ch;

    input_ctx_init(&ctx);
    input_ctx_load_hist(&ctx);
    while (1) {
        input_render(shell->tui, &ctx, shell->last_status);
        ch = wgetch(shell->tui->win_input);
        if (ch == 4 && ctx.len == 0) {
            free_history(ctx.hist, ctx.hist_count);
            return NULL;
        }
        if (ch == KEY_UP)   input_handle_history(&ctx, -1);
        if (ch == KEY_DOWN) input_handle_history(&ctx, 1);
        input_handle_edit(&ctx, ch);
        input_handle_ctrl(shell, &ctx, ch);
        input_handle_scroll(shell, ch);
        if (input_handle_printable(&ctx, ch))
            break;
    }
    free_history(ctx.hist, ctx.hist_count);
    if (strcmp(ctx.buf, "/theme") == 0) {
        tui_open_theme_menu(shell);
        return strdup("");
    }
    tui_output_reset_scroll(shell->tui);
    return strdup(ctx.buf);
}
```

- [ ] **Commit**

```bash
git add src/tui/tui_input.c
git commit -m "feat(tui): add tui_read_line — wgetch loop, history, /theme intercept"
```

---

## Task 10 : `src/tui/tui_theme_menu.c`

**Files:**
- Create: `src/tui/tui_theme_menu.c`

- [ ] **Écrire `src/tui/tui_theme_menu.c`**

```c
/*
** EPITECH PROJECT, 2026
** tui_theme_menu.c
** File description:
** overlay interactif /theme — sélection et persistance
*/

#include "tui.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>

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
    if (ch == KEY_UP   && *selected > 0)              (*selected)--;
    if (ch == KEY_DOWN && *selected < THEMES_COUNT - 1) (*selected)++;
    if (ch == '\n' || ch == '\r') return 1;
    if (ch == 27)                 return -1;
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
```

- [ ] **Commit**

```bash
git add src/tui/tui_theme_menu.c
git commit -m "feat(tui): add /theme overlay menu with persist to ~/.42shrc"
```

---

## Task 11 : `src/tui/tui_resize.c`

**Files:**
- Create: `src/tui/tui_resize.c`

- [ ] **Écrire `src/tui/tui_resize.c`**

```c
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
```

- [ ] **Commit**

```bash
git add src/tui/tui_resize.c
git commit -m "feat(tui): add SIGWINCH handler for terminal resize"
```

---

## Task 12 : Mise à jour de `include/shell.h`

**Files:**
- Modify: `include/shell.h`

- [ ] **Remplacer le contenu de `include/shell.h`**

```c
/*
** EPITECH PROJECT, 2026
** shell.h
** File description:
** shell state + main loop API
*/

#ifndef LOOP_H_
    #define LOOP_H_

    #include <unistd.h>
    #include <stdbool.h>
    #include "tui.h"

typedef struct s_shell {
    char **env;
    char **locals;
    char **aliases;
    int last_status;
    char *line;
    pid_t last_pid;
    tui_t *tui;
} shell_t;

bool shell_init(shell_t *shell, char **envp);
void shell_destroy(shell_t *shell);
int display_marrashell(void);
int display_branch(void);
int display_prompt(int last_status);
int shell_loop(shell_t *sh);
char *read_line(shell_t *shell);
bool is_in_repository(void);

#endif // !LOOP_H_
```

> **Note :** `tui_t` est défini dans `tui.h` qui est inclus ici. `tui.h` forward-déclare `shell_t` via `typedef struct s_shell shell_t`, compatible avec la définition complète ci-dessus (C11).

- [ ] **Vérifier que `include/builtins.h` a `MAX_HIST` accessible**

`builtins.h` inclut `exec.h` → `shell.h` → `tui.h`. `MAX_HIST` est dans `readline.h` qui sera supprimé. Ajouter dans `builtins.h` :
```c
#define MAX_HIST 1000
```
Placer cette ligne après `#define HISTORY_FILE ".42sh_history"`.

- [ ] **Mettre à jour `src/utils/history_load.c`** — retirer l'include `readline.h` :

Remplacer `#include "readline.h"` par rien (MAX_HIST et HISTORY_FILE viennent maintenant de `builtins.h`).

- [ ] **Commit**

```bash
git add include/shell.h include/builtins.h src/utils/history_load.c
git commit -m "feat(tui): update shell.h — named struct + tui_t field; move MAX_HIST to builtins.h"
```

---

## Task 13 : Mise à jour de `src/shell/init.c`

**Files:**
- Modify: `src/shell/init.c`

- [ ] **Remplacer le contenu de `src/shell/init.c`**

```c
/*
** EPITECH PROJECT, 2026
** init.c
** File description:
** initialize shell state + TUI
*/

#include "shell.h"
#include "env.h"
#include "mysh.h"
#include "utils.h"
#include "tui.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

static void init_special_vars(shell_t *shell)
{
    char cwd[PATH_MAX];
    char *term = env_get_value(shell->env, "TERM=");

    if (getcwd(cwd, PATH_MAX) != NULL)
        local_set_value(shell, "cwd", cwd);
    if (term)
        local_set_value(shell, "term", term);
}

static void load_rc_file(shell_t *shell)
{
    char path[512];
    char line[256];
    const char *home = getenv("HOME");
    FILE *f;

    if (!home)
        return;
    snprintf(path, sizeof(path), "%s/.42shrc", home);
    f = fopen(path, "r");
    if (!f)
        return;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "set theme=", 10) == 0) {
            line[strcspn(line, "\n")] = '\0';
            local_set_value(shell, "theme", line + 10);
        }
    }
    fclose(f);
}

static void shell_cleanup_partial(shell_t *shell)
{
    free_string_array(shell->env);
    free(shell->locals);
    free(shell->aliases);
    shell->env = NULL;
    shell->locals = NULL;
    shell->aliases = NULL;
}

bool shell_init(shell_t *shell, char **envp)
{
    char *theme_name;
    int theme_id = 0;

    shell->env = dup_string_array(envp);
    shell->locals = calloc(1, sizeof(char *));
    shell->aliases = calloc(1, sizeof(char *));
    if (!shell->env || !shell->locals || !shell->aliases) {
        shell_cleanup_partial(shell);
        return false;
    }
    shell->last_status = 0;
    shell->line = NULL;
    shell->last_pid = -1;
    shell->tui = NULL;
    init_special_vars(shell);
    load_rc_file(shell);
    theme_name = local_get_value(shell->locals, "theme");
    if (theme_name)
        theme_id = tui_get_theme_by_name(theme_name);
    if (isatty(STDIN_FILENO)) {
        shell->tui = tui_init(theme_id < 0 ? 0 : theme_id);
        if (shell->tui)
            tui_register_resize_handler(shell->tui);
    }
    return true;
}

void shell_destroy(shell_t *shell)
{
    if (!shell)
        return;
    free_string_array(shell->env);
    free_string_array(shell->locals);
    free_string_array(shell->aliases);
    free(shell->line);
    tui_destroy(shell->tui);
    shell->env = NULL;
    shell->locals = NULL;
    shell->aliases = NULL;
    shell->line = NULL;
    shell->tui = NULL;
}
```

> **Note :** `display_marrashell`, `display_prompt`, `display_branch` restent déclarés dans `shell.h` mais ne sont plus appelés. Ils seront retirés lors d'un nettoyage futur.

- [ ] **Commit**

```bash
git add src/shell/init.c
git commit -m "feat(tui): update init.c — tui_init, load ~/.42shrc theme"
```

---

## Task 14 : Mise à jour de `src/shell/loop.c`

**Files:**
- Modify: `src/shell/loop.c`

- [ ] **Remplacer le contenu de `src/shell/loop.c`**

```c
/*
** EPITECH PROJECT, 2026
** loop.c
** File description:
** prompt/read/parse/execute loop
*/

#include "mysh.h"
#include "exec.h"
#include "shell.h"
#include "builtins.h"
#include "tui.h"
#include "expand.h"
#include "env.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

static void run_hook(shell_t *shell, const char *name)
{
    char *val = local_get_value(shell->locals, name);

    if (val && val[0] != '\0')
        run_command(shell, val);
}

static bool handle_eof(shell_t *shell, bool is_interactive)
{
    if (!is_interactive)
        return false;
    if (!local_get_value(shell->locals, "ignoreof"))
        return false;
    fprintf(stderr, "\nUse \"exit\" to leave 42sh.\n");
    return true;
}

static bool read_input(shell_t *shell, bool is_interactive)
{
    size_t cap = 0;

    free(shell->line);
    shell->line = NULL;
    if (is_interactive && shell->tui) {
        shell->line = tui_read_line(shell);
        if (shell->line != NULL)
            return true;
        return handle_eof(shell, is_interactive);
    }
    return getline(&shell->line, &cap, stdin) != -1;
}

static enum shell_status process_line(shell_t *shell)
{
    char *expanded = expand_line(shell);
    enum shell_status status;
    char *save;

    if (!expanded || expanded[0] == '\0') {
        free(expanded);
        return SHELL_CONTINUE;
    }
    status = run_command(shell, expanded);
    save = shell->line;
    shell->line = expanded;
    add_to_history(shell);
    shell->line = save;
    free(expanded);
    if (shell->tui)
        tui_update_sidebar(shell->tui, shell->line, shell->last_status);
    return status;
}

int shell_loop(shell_t *shell)
{
    enum shell_status status = SHELL_CONTINUE;
    bool is_interactive = isatty(STDIN_FILENO);

    while (status == SHELL_CONTINUE) {
        run_hook(shell, "precmd");
        if (!read_input(shell, is_interactive))
            break;
        status = process_line(shell);
    }
    return shell->last_status;
}
```

- [ ] **Commit**

```bash
git add src/shell/loop.c
git commit -m "feat(tui): update loop.c — use tui_read_line, update sidebar after each cmd"
```

---

## Task 15 : Mise à jour du Makefile + suppression des anciens fichiers

**Files:**
- Modify: `Makefile`
- Delete: `src/shortcuts/*.c`, `include/readline.h`

- [ ] **Remplacer la liste `SRCS` dans le Makefile**

Dans le Makefile, ajouter les nouveaux fichiers et retirer les shortcuts :

Remplacer le bloc `SRCS_DIR)/shortcuts/...` par les lignes suivantes, et ajouter le bloc tui + history_load :

```makefile
# Dans SRCS, remplacer :
#   $(SRCS_DIR)/shortcuts/terminal_mode.c \
#   $(SRCS_DIR)/shortcuts/input_controls.c \
#   $(SRCS_DIR)/shortcuts/interactive_inputs.c \
#   $(SRCS_DIR)/shortcuts/history_navigation.c \
# Par :
	   $(SRCS_DIR)/utils/history_load.c \
	   $(SRCS_DIR)/tui/tui_init.c \
	   $(SRCS_DIR)/tui/tui_theme.c \
	   $(SRCS_DIR)/tui/tui_output.c \
	   $(SRCS_DIR)/tui/tui_sidebar.c \
	   $(SRCS_DIR)/tui/tui_input.c \
	   $(SRCS_DIR)/tui/tui_input_keys.c \
	   $(SRCS_DIR)/tui/tui_theme_menu.c \
	   $(SRCS_DIR)/tui/tui_resize.c \
```

Modifier aussi la règle de linkage pour ajouter `-lncurses -lpthread` :

```makefile
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(TARGET) $(OBJS) -lncurses -lpthread
```

Ajouter `tests/test_tui.c` et `$(SRCS_DIR)/tui/tui_theme.c` à `TEST_SRCS` :

```makefile
TEST_SRCS = tests/test_utils.c \
            tests/test_parser.c \
            tests/test_glob.c \
            tests/test_tui.c \
            $(SRCS_DIR)/tui/tui_theme.c \
            $(SRCS_DIR)/utils/string_array.c \
            ...
```

- [ ] **Supprimer les fichiers obsolètes**

```bash
rm src/shortcuts/terminal_mode.c \
   src/shortcuts/input_controls.c \
   src/shortcuts/interactive_inputs.c \
   src/shortcuts/history_navigation.c \
   include/readline.h
```

- [ ] **Compiler**

```bash
make re 2>&1 | tail -20
```
Attendu : compilation sans erreur, binaire `./42sh` créé.

- [ ] **Si erreurs de compilation** — diagnostiquer et corriger :
  - `readline.h not found` → vérifier que tous les `#include "readline.h"` sont retirés
  - `undefined reference to load_history` → vérifier que `history_load.c` est dans `SRCS`
  - `undefined reference to tui_*` → vérifier que tous les `src/tui/*.c` sont dans `SRCS`

- [ ] **Commit**

```bash
git add Makefile
git rm src/shortcuts/terminal_mode.c src/shortcuts/input_controls.c \
       src/shortcuts/interactive_inputs.c src/shortcuts/history_navigation.c \
       include/readline.h
git commit -m "feat(tui): update Makefile, remove shortcuts/ — TUI fully wired"
```

---

## Task 16 : Tests unitaires + test manuel TUI

**Files:**
- Modify: `Makefile` (TEST_SRCS finalisé)

- [ ] **Lancer les tests unitaires**

```bash
make tests_run 2>&1 | grep -E "PASSED|FAILED|RUN"
```
Attendu : tous les tests passent, dont les 3 tests `tui_theme`.

- [ ] **Test manuel TUI — lancement**

```bash
./42sh
```
Attendu :
- Interface ncurses s'ouvre
- Sidebar gauche : bannière MARRA, thème "Dark Blue", PWD, dernière commande
- Zone output vide
- Barre input avec prompt `✓ ❯`

- [ ] **Test commandes de base**

Taper dans la barre input :
```
ls
pwd
echo "hello world"
ls -la /tmp | grep tmp
```
Attendu : sortie apparaît dans la zone output à droite, sidebar met à jour la dernière commande.

- [ ] **Test scroll**

Taper `make re` ou une commande longue. Appuyer sur `PgUp` pour remonter dans l'output, `PgDn` pour redescendre.

- [ ] **Test `/theme`**

Taper `/theme`. Attendu : overlay centré avec 5 thèmes. Naviguer ↑↓. Enter applique, Esc annule. Vérifier `~/.42shrc` contient le bon `set theme=`.

- [ ] **Test persistance thème**

Quitter (`exit` ou Ctrl+D). Relancer `./42sh`. Attendu : thème précédent chargé.

- [ ] **Test resize**

Redimensionner le terminal. Attendu : les 3 zones se recalculent sans artefacts.

- [ ] **Test mode non-interactif**

```bash
echo "ls" | ./42sh
```
Attendu : sortie texte classique, pas de TUI.

- [ ] **Commit final**

```bash
git add -u
git commit -m "feat(tui): ncurses TUI — sidebar, output, input, themes, resize complete"
```
