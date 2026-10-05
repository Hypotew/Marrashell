/*
** EPITECH PROJECT, 2026
** test_glob.c
** File description:
** unit tests for glob expansion
*/
#include <criterion/criterion.h>
#include "expand.h"
#include "utils.h"

Test(glob_expand, no_glob_pattern)
{
    char *argv[] = {"ls", "file.txt", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_eq(string_array_len(result), 2);
    cr_assert_str_eq(result[0], "ls");
    cr_assert_str_eq(result[1], "file.txt");
    free_string_array(result);
}

Test(glob_expand, empty_argv)
{
    char *argv[] = {NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_null(result[0]);
    free_string_array(result);
}

Test(glob_expand, no_match_returns_null)
{
    char *argv[] = {"ls", "*.xyzxyz_no_match_ever_42", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_null(result);
}

Test(glob_expand, match_dev_null)
{
    char *argv[] = {"ls", "/dev/nul?", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "ls");
    cr_assert_not_null(result[1]);
    free_string_array(result);
}

Test(glob_expand, star_in_dev)
{
    char *argv[] = {"ls", "/dev/null", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_str_eq(result[1], "/dev/null");
    free_string_array(result);
}

Test(glob_expand, bracket_pattern)
{
    char *argv[] = {"ls", "/dev/[nN]ull", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "ls");
    cr_assert_not_null(result[1]);
    free_string_array(result);
}

Test(glob_expand, preserves_all_non_glob_args)
{
    char *argv[] = {"echo", "hello", "world", "foo", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_eq(string_array_len(result), 4);
    cr_assert_str_eq(result[0], "echo");
    cr_assert_str_eq(result[1], "hello");
    cr_assert_str_eq(result[2], "world");
    cr_assert_str_eq(result[3], "foo");
    free_string_array(result);
}

Test(glob_expand, mixed_glob_and_literal)
{
    char *argv[] = {"ls", "/dev/null", "/dev/nul?", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_str_eq(result[0], "ls");
    cr_assert_str_eq(result[1], "/dev/null");
    free_string_array(result);
}

Test(glob_expand, result_is_null_terminated)
{
    char *argv[] = {"echo", "hello", NULL};
    char **result = glob_expand_argv(argv);

    cr_assert_not_null(result);
    cr_assert_null(result[string_array_len(result)]);
    free_string_array(result);
}
