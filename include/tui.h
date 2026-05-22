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
#ifndef LOOP_H_
typedef struct s_shell shell_t;
#endif

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
