/*
** EPITECH PROJECT, 2026
** utils.h
** File description:
** utils functions
*/

#ifndef UTILS_H_
    #define UTILS_H_

    #include <stddef.h>

size_t string_array_len(char **array);
void free_string_array(char **array);
char **dup_string_array(char **array);
char **my_split_words(char const *str, char const *delimiters);
int get_positive_nbr(char const *str, unsigned int *out);
char *array_to_string(char **array, size_t start);

#endif // !UTILS_H_
