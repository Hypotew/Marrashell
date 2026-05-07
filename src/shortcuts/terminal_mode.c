/*
** EPITECH PROJECT, 2026
** terminal_mode
** File description:
** terminal mode helpers for interactive input
*/

#include "readline.h"
#include <termios.h>
#include <unistd.h>

struct termios raw_mode(void)
{
    struct termios orig;
    struct termios raw;

    tcgetattr(STDIN_FILENO, &orig);
    raw = orig;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    return orig;
}

void disable_raw_mode(struct termios *orig)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig);
}

void clear_terminal(void)
{
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
}
