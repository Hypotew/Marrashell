/*
** EPITECH PROJECT, 2026
** test_utils.c
** File description:
** unit tests for string_array and split_words
*/
#include <criterion/criterion.h>
#include "utils.h"

Test(string_array_len, null_array)
{
    cr_assert_eq(string_array_len(NULL), 0);
}

Test(string_array_len, empty_array)
{
    char *arr[] = {NULL};
    cr_assert_eq(string_array_len(arr), 0);
}

Test(string_array_len, single_element)
{
    char *arr[] = {"hello", NULL};
    cr_assert_eq(string_array_len(arr), 1);
}

Test(string_array_len, multiple_elements)
{
    char *arr[] = {"a", "b", "c", "d", NULL};
    cr_assert_eq(string_array_len(arr), 4);
}

Test(dup_string_array, null_input)
{
    char **copy = dup_string_array(NULL);

    cr_assert_not_null(copy);
    cr_assert_null(copy[0]);
    free_string_array(copy);
}

Test(dup_string_array, empty_array)
{
    char *arr[] = {NULL};
    char **copy = dup_string_array(arr);

    cr_assert_not_null(copy);
    cr_assert_null(copy[0]);
    free_string_array(copy);
}

Test(dup_string_array, normal_dup)
{
    char *arr[] = {"hello", "world", NULL};
    char **copy = dup_string_array(arr);

    cr_assert_not_null(copy);
    cr_assert_str_eq(copy[0], "hello");
    cr_assert_str_eq(copy[1], "world");
    cr_assert_null(copy[2]);
    cr_assert_neq((void *)copy[0], (void *)arr[0]);
    free_string_array(copy);
}

Test(dup_string_array, three_elements)
{
    char *arr[] = {"a", "b", "c", NULL};
    char **copy = dup_string_array(arr);

    cr_assert_eq(string_array_len(copy), 3);
    cr_assert_str_eq(copy[2], "c");
    free_string_array(copy);
}

Test(array_to_string, null_input)
{
    cr_assert_null(array_to_string(NULL, 0));
}

Test(array_to_string, start_out_of_bounds)
{
    char *arr[] = {"a", NULL};
    cr_assert_null(array_to_string(arr, 5));
}

Test(array_to_string, single_word)
{
    char *arr[] = {"hello", NULL};
    char *str = array_to_string(arr, 0);

    cr_assert_str_eq(str, "hello");
    free(str);
}

Test(array_to_string, join_two_words)
{
    char *arr[] = {"hello", "world", NULL};
    char *str = array_to_string(arr, 0);

    cr_assert_str_eq(str, "hello world");
    free(str);
}

Test(array_to_string, join_three_words)
{
    char *arr[] = {"a", "b", "c", NULL};
    char *str = array_to_string(arr, 0);

    cr_assert_str_eq(str, "a b c");
    free(str);
}

Test(array_to_string, start_offset)
{
    char *arr[] = {"a", "b", "c", NULL};
    char *str = array_to_string(arr, 1);

    cr_assert_str_eq(str, "b c");
    free(str);
}

Test(array_to_string, start_at_last)
{
    char *arr[] = {"a", "b", "c", NULL};
    char *str = array_to_string(arr, 2);

    cr_assert_str_eq(str, "c");
    free(str);
}

Test(my_split_words, simple_space)
{
    char **result = my_split_words("hello world", " ");

    cr_assert_str_eq(result[0], "hello");
    cr_assert_str_eq(result[1], "world");
    cr_assert_null(result[2]);
    free_string_array(result);
}

Test(my_split_words, multiple_consecutive_spaces)
{
    char **result = my_split_words("a   b   c", " ");

    cr_assert_eq(string_array_len(result), 3);
    cr_assert_str_eq(result[0], "a");
    cr_assert_str_eq(result[1], "b");
    cr_assert_str_eq(result[2], "c");
    free_string_array(result);
}

Test(my_split_words, leading_trailing_delimiters)
{
    char **result = my_split_words("  hello  ", " ");

    cr_assert_eq(string_array_len(result), 1);
    cr_assert_str_eq(result[0], "hello");
    free_string_array(result);
}

Test(my_split_words, multiple_delimiter_chars)
{
    char **result = my_split_words("a:b,c", ":,");

    cr_assert_eq(string_array_len(result), 3);
    cr_assert_str_eq(result[0], "a");
    cr_assert_str_eq(result[1], "b");
    cr_assert_str_eq(result[2], "c");
    free_string_array(result);
}

Test(my_split_words, single_word)
{
    char **result = my_split_words("hello", " ");

    cr_assert_eq(string_array_len(result), 1);
    cr_assert_str_eq(result[0], "hello");
    free_string_array(result);
}

Test(my_split_words, tab_delimiter)
{
    char **result = my_split_words("a\tb\tc", "\t");

    cr_assert_eq(string_array_len(result), 3);
    cr_assert_str_eq(result[0], "a");
    cr_assert_str_eq(result[1], "b");
    cr_assert_str_eq(result[2], "c");
    free_string_array(result);
}

Test(my_split_words, mixed_delimiters_space_tab)
{
    char **result = my_split_words("a b\tc", " \t");

    cr_assert_eq(string_array_len(result), 3);
    free_string_array(result);
}
