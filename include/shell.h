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

typedef struct {
    char **env;
    char **locals;
    char **aliases;
    int last_status;
    char *line;
    pid_t last_pid;
} shell_t;

bool shell_init(shell_t *shell, char **envp);
int display_marrashell(void);
int display_branch(void);
int display_prompt(int last_status);
int shell_loop(shell_t *sh);
char *read_line(shell_t *shell);
bool is_in_repository(void);

#endif // !LOOP_H_
