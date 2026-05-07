/*
** EPITECH PROJECT, 2026
** readline.h
** File description:
** interactive line editor API
*/

#ifndef READLINE_H_
    #define READLINE_H_

    #include <termios.h>
    #include <unistd.h>
    #include "shell.h"

    #define ARROW_UP '\033'
    #define MAX_HIST 1000

typedef struct {
    char *buf;
    size_t len;
    char **hist;
    int hist_count;
    int hist_idx;
} rl_ctx_t;

struct termios raw_mode(void);
void disable_raw_mode(struct termios *orig);
char **load_history(int *count);
void free_history(char **entries, int count);
char *read_line(shell_t *shell);

#endif // !READLINE_H_
