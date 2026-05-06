/*
** EPITECH PROJECT, 2026
** expand.c
** File description:
** expansion orchestrator — chains all expansion passes
*/

#include "expand.h"

char *expand_line(const char *input)
{
    return history_expand(input);
}
