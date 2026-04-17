/*
** EPITECH PROJECT, 2026
** main.c
** File description:
** main entry file
*/

#include "shell.h"
#include "mysh.h"
#include "my.h"

#include <stdio.h>
#include <stdlib.h>

int main(__attribute__((unused)) int argc,
    __attribute__((unused)) char **argv,
    char **envp)
{
    shell_t shell = {0};
    int exit_code = SUCCESS_EXIT;

    if (!shell_init(&shell, envp)) {
        perror("shell init");
        return FAILURE_EXIT;
    }
    exit_code = shell_loop(&shell);
    free_string_array(shell.env);
    free(shell.line);
    return exit_code;
}
