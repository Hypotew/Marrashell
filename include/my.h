/*
** EPITECH PROJECT, 2026
** my.h
** File description:
** string array utilities
*/

#ifndef MY_H_
    #define MY_H_

    #include <stddef.h>
    #include <stdbool.h>

size_t string_array_len(char **array);
void free_string_array(char **array);
char **dup_string_array(char **array);
char **my_split_words(char const *str, char const *delimiters);

#endif
