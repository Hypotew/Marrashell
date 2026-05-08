/*
** EPITECH PROJECT, 2026
** glob_expand.c
** File description:
** glob pattern expansion for argv
*/
#include "expand.h"
#include "utils.h"
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool has_glob(const char *s)
{
    return (strchr(s, '*') != NULL || strchr(s, '?') != NULL
        || strchr(s, '[') != NULL);
}

static void free_globs(glob_t *globs, int count)
{
    for (int i = 0; i < count; i++)
        if (globs[i].gl_pathv != NULL)
            globfree(&globs[i]);
}

static bool run_all_globs(char **argv, int argc, glob_t *globs)
{
    int ret;

    for (int i = 0; i < argc; i++) {
        globs[i].gl_pathc = 0;
        globs[i].gl_pathv = NULL;
        if (!has_glob(argv[i]))
            continue;
        ret = glob(argv[i], GLOB_TILDE, NULL, &globs[i]);
        if (ret == GLOB_NOMATCH)
            fprintf(stderr, "%s: No match.\n", argv[0]);
        if (ret != 0) {
            free_globs(globs, i + 1);
            return false;
        }
    }
    return true;
}

static int count_total(int argc, glob_t *globs)
{
    int total = 0;

    for (int i = 0; i < argc; i++)
        total += globs[i].gl_pathc > 0 ? (int)globs[i].gl_pathc : 1;
    return total;
}

static bool copy_glob_results(glob_t *g, char **res, int *k)
{
    for (size_t j = 0; j < g->gl_pathc; j++) {
        res[*k] = strdup(g->gl_pathv[j]);
        if (!res[*k])
            return false;
        (*k)++;
    }
    return true;
}

static bool fill_one(const char *arg, glob_t *g, char **res, int *k)
{
    if (g->gl_pathc == 0) {
        res[*k] = strdup(arg);
        if (!res[*k])
            return false;
        (*k)++;
        return true;
    }
    return copy_glob_results(g, res, k);
}

static bool fill_result(char **argv, int argc, glob_t *globs, char **res)
{
    int k = 0;

    for (int i = 0; i < argc; i++)
        if (!fill_one(argv[i], &globs[i], res, &k))
            return false;
    res[k] = NULL;
    return true;
}

char **glob_expand_argv(char **argv)
{
    int argc = (int)string_array_len(argv);
    glob_t *globs = calloc(argc ? argc : 1, sizeof(glob_t));
    int total = 0;
    char **result = NULL;

    if (!globs)
        return NULL;
    if (!run_all_globs(argv, argc, globs)) {
        free(globs);
        return NULL;
    }
    total = count_total(argc, globs);
    result = malloc(sizeof(char *) * (total + 1));
    if (result && !fill_result(argv, argc, globs, result)) {
        free_string_array(result);
        result = NULL;
    }
    free_globs(globs, argc);
    free(globs);
    return result;
}
