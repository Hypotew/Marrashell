/*
** EPITECH PROJECT, 2026
** expand.h
** File description:
** expansion module header (history, aliases, variables)
*/

#ifndef EXPAND_H_
    #define EXPAND_H_

    #include <stddef.h>
    #include "shell.h"

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} buf_t;

int buf_append(buf_t *b, const char *s);

char *expand_line(shell_t *shell);
char *history_expand(const char *input);
char *var_expand(shell_t *shell, const char *input);
char *alias_expand(shell_t *shell, const char *input);

char *history_get_last(void);
char *history_get_by_number(unsigned long long n);
char *history_get_by_offset(unsigned long long offset);
char *history_get_by_prefix(const char *prefix);
char *history_get_by_substr(const char *substr);

#endif /* !EXPAND_H_ */
