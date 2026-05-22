/*
** EPITECH PROJECT, 2026
** builtins.h
** File description:
** builtin dispatch + builtin prototypes
*/

#ifndef BUILTINS_H_
    #define BUILTINS_H_

    #define HISTORY_FILE ".42sh_history"
    #define MAX_HIST 1000

    #include <stdbool.h>
    #include <stdio.h>
    #include "exec.h"

typedef int (*builtin_fn_t)(shell_t *shell, char **argv, bool *should_exit);

typedef struct {
    const char *name;
    builtin_fn_t fn;
} builtin_entry_t;

typedef struct {
    FILE *fd;
    unsigned long long nb_lines;
    char *last_cmd;
} history_t;

bool run_builtin(shell_t *shell, char **argv, bool *should_exit);

// builtin prototypes
int shell_exit(shell_t *shell, char **argv, bool *should_exit);
int my_cd(shell_t *shell, char **argv, bool *should_exit);
int my_setenv(shell_t *shell, char **argv, bool *should_exit);
int my_unsetenv(shell_t *shell, char **argv, bool *should_exit);
int my_env(shell_t *shell, char **argv, bool *should_exit);
int my_echo(shell_t *shell, char **argv, bool *should_exit);
int my_history(shell_t *shell, char **argv, bool *should_exit);
int my_repeat(shell_t *shell, char **argv, bool *should_exit);
int my_set(shell_t *shell, char **argv, bool *should_exit);
int my_unset(shell_t *shell, char **argv, bool *should_exit);
int my_alias(shell_t *shell, char **argv, bool *should_exit);
int my_unalias(shell_t *shell, char **argv, bool *should_exit);
int my_where(shell_t *shell, char **argv, bool *should_exit);
int my_which(shell_t *shell, char **argv, bool *should_exit);

// builtin utils prototypes
int add_to_history(shell_t *shell);
char *parse_hist_entry(char *line);
char **load_history(int *count);
void free_history(char **entries, int count);
int print_path_matches(shell_t *shell, const char *command, bool print_all,
    bool print_missing);

#endif // !BUILTINS_H_
