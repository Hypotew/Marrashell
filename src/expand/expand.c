/*
** EPITECH PROJECT, 2026
** expand.c
** File description:
** expansion orchestrator — chains all expansion passes
*/

#include "expand.h"

#include <stdlib.h>

char *expand_line(shell_t *shell)
{
    char *hist = history_expand(shell->line);
    char *result = NULL;

    if (!hist)
        return NULL;
    result = var_expand(shell, hist);
    free(hist);
    return result;
}
