/*
** EPITECH PROJECT, 2026
** main.c
** File description:
** main entry file
*/

#include "shell.h"
#include "mysh.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(__attribute__((unused)) int argc,
    __attribute__((unused)) char **argv,
    char **envp)
{
    shell_t shell = {0};
    int exit_code = SUCCESS_EXIT;

    if (!shell_init(&shell, envp)) {
        if (fprintf(stderr, "shell init: %s\n", strerror(errno)) < 0)
            return FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    exit_code = shell_loop(&shell);
    shell_destroy(&shell);
    return exit_code;
}
