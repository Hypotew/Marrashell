/*
** EPITECH PROJECT, 2026
** exec.h
** File description:
** command execution API
*/

#ifndef EXEC_H_
    #define EXEC_H_

    #include "shell.h"
    #include "parser.h"
    #include "builtins.h"
    #include <stdbool.h>

enum shell_status {
    SHELL_EXIT,
    SHELL_CONTINUE,
};

typedef struct {
    int prev_read;
    int fds[2];
} pipe_ctx_t;

enum shell_status run_command(shell_t *shell, char *input_buf);
bool apply_redirections(command_t *cmd);
bool apply_heredoc(redir_t *redir);
bool prepare_heredocs(command_t *pipeline);
int run_external(shell_t *shell, char **argv);
int exec_external(shell_t *shell, char **argv);
bool contains_slash(const char *str);
char *find_path_value(char **env);
char *build_exec_path(char **env, char *exe);
int validate_exec_target(char *command, char *exe_path, bool from_path);
int print_path_not_set(void);

#endif // !EXEC_H_
