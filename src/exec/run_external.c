/*
** EPITECH PROJECT, 2026
** run_external.c
** File description:
** fork/execve/wait + status propagation
*/

#include "shell.h"
#include "mysh.h"
#include "exec.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int print_execve_message(int err)
{
    if (err == ENOEXEC)
        return fprintf(stderr, "%s",
            "Exec format error. Binary file not executable.\n") < 0 ?
            FAILURE_EXIT : SUCCESS_EXIT;
    if (err == EACCES)
        return fprintf(stderr, "%s", "Permission denied.\n") < 0 ?
            FAILURE_EXIT : SUCCESS_EXIT;
    if (err == ENOENT)
        return fprintf(stderr, "%s", "No such file or directory.\n") < 0 ?
            FAILURE_EXIT : SUCCESS_EXIT;
    if (err == EISDIR)
        return fprintf(stderr, "%s", "Is a directory.\n") < 0 ?
            FAILURE_EXIT : SUCCESS_EXIT;
    if (fprintf(stderr, "%s\n", strerror(err)) < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int print_execve_error(char *command, int err)
{
    if (fprintf(stderr, "%s", command) < 0)
        return FAILURE_EXIT;
    if (fprintf(stderr, "%s", ": ") < 0)
        return FAILURE_EXIT;
    if (print_execve_message(err) == FAILURE_EXIT)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int print_signal_message(int sig)
{
    if (sig == SIGSEGV && fprintf(stderr, "%s", "Segmentation fault") < 0)
        return FAILURE_EXIT;
    if (sig == SIGFPE && fprintf(stderr, "%s", "Floating exception") < 0)
        return FAILURE_EXIT;
    if (sig == SIGABRT && fprintf(stderr, "%s", "Aborted") < 0)
        return FAILURE_EXIT;
    if (sig == SIGILL && fprintf(stderr, "%s", "Illegal instruction") < 0)
        return FAILURE_EXIT;
    if (sig == SIGBUS && fprintf(stderr, "%s", "Bus error") < 0)
        return FAILURE_EXIT;
    return SUCCESS_EXIT;
}

static int handle_child_segfault_status(int status)
{
    int sig = WTERMSIG(status);

    if (print_signal_message(sig) == FAILURE_EXIT)
        return FAILURE_EXIT;
#ifdef WCOREDUMP
    if (WCOREDUMP(status) && fprintf(stderr, "%s", " (core dumped)") < 0)
        return FAILURE_EXIT;
#endif
    if (fprintf(stderr, "%s", "\n") < 0)
        return FAILURE_EXIT;
    return 128 + sig;
}

static int handle_child_status(int status)
{
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    if (WIFSIGNALED(status))
        return handle_child_segfault_status(status);
    return 1;
}

static void run_child(char *exe_path, char **argv, char **env)
{
    int err = 0;

    execve(exe_path, argv, env);
    err = errno;
    if (print_execve_error(argv[0], err) == FAILURE_EXIT)
        exit(FAILURE_EXIT);
    free(exe_path);
    exit(FAILURE_EXIT);
}

static int spawn_and_wait(shell_t *shell, char *exe_path,
    char **argv, char **env)
{
    pid_t child_pid = fork();
    int code = 0;

    if (child_pid < 0) {
        if (fprintf(stderr, "%s", "fork failed\n") < 0)
            return FAILURE_EXIT;
        free(exe_path);
        return FAILURE_EXIT;
    }
    if (child_pid == 0)
        run_child(exe_path, argv, env);
    if (waitpid(child_pid, &code, 0) < 0) {
        free(exe_path);
        return FAILURE_EXIT;
    }
    shell->last_status = handle_child_status(code);
    free(exe_path);
    return SUCCESS_EXIT;
}

int exec_external(shell_t *shell, char **argv)
{
    bool from_path = !contains_slash(argv[0]);
    char *exe_path = build_exec_path(shell->env, argv[0]);

    if (from_path && find_path_value(shell->env) == NULL)
        if (print_path_not_set() == FAILURE_EXIT)
            return FAILURE_EXIT;
    if (validate_exec_target(argv[0], exe_path, from_path) != SUCCESS_EXIT) {
        free(exe_path);
        exit(FAILURE_EXIT);
    }
    execve(exe_path, argv, shell->env);
    if (print_execve_error(argv[0], errno) == FAILURE_EXIT)
        return FAILURE_EXIT;
    free(exe_path);
    exit(FAILURE_EXIT);
}

int run_external(shell_t *shell, char **argv)
{
    bool from_path = !contains_slash(argv[0]);
    char *exe_path = build_exec_path(shell->env, argv[0]);

    if (from_path && find_path_value(shell->env) == NULL)
        if (print_path_not_set() == FAILURE_EXIT)
            return FAILURE_EXIT;
    if (validate_exec_target(argv[0], exe_path, from_path) != SUCCESS_EXIT) {
        free(exe_path);
        shell->last_status = FAILURE_EXIT;
        return FAILURE_EXIT;
    }
    return spawn_and_wait(shell, exe_path, argv, shell->env);
}
