NAME = 42sh
CC = epiclang
CFLAGS = -Wall -Wextra -I./include -g3

TEST_NAME = mysh_tests
TEST_LDFLAGS = -lcriterion -lncurses
TEST_SRCS = tests/test_utils.c \
	   tests/test_parser.c \
	   tests/test_glob.c \
	   tests/test_tui.c \
	   $(SRCS_DIR)/tui/tui_theme.c \
	   $(SRCS_DIR)/utils/string_array.c \
	   $(SRCS_DIR)/utils/split_words.c \
	   $(SRCS_DIR)/parser/tokenize.c \
	   $(SRCS_DIR)/parser/parse_command.c \
	   $(SRCS_DIR)/parser/parse_utils.c \
	   $(SRCS_DIR)/parser/parse_create.c \
	   $(SRCS_DIR)/parser/parse_append.c \
	   $(SRCS_DIR)/parser/parse_free.c \
	   $(SRCS_DIR)/expand/glob_expand.c

SRCS_DIR = src
SRCS = $(SRCS_DIR)/main.c \
	   $(SRCS_DIR)/builtins/run_builtin.c \
	   $(SRCS_DIR)/builtins/exit.c \
	   $(SRCS_DIR)/builtins/cd.c \
	   $(SRCS_DIR)/builtins/setenv.c \
	   $(SRCS_DIR)/builtins/unsetenv.c \
	   $(SRCS_DIR)/builtins/env.c \
	   $(SRCS_DIR)/builtins/echo.c \
	   $(SRCS_DIR)/builtins/history.c \
	   $(SRCS_DIR)/utils/history_utils.c \
	   $(SRCS_DIR)/utils/history_load.c \
	   $(SRCS_DIR)/builtins/repeat.c \
	   $(SRCS_DIR)/utils/repeat_utils.c \
	   $(SRCS_DIR)/exec/run_command.c \
	   $(SRCS_DIR)/exec/resolve_path.c \
	   $(SRCS_DIR)/exec/run_external.c \
	   $(SRCS_DIR)/exec/redirections.c \
	   $(SRCS_DIR)/exec/heredoc.c \
	   $(SRCS_DIR)/env/pwd.c \
	   $(SRCS_DIR)/shell/loop.c \
	   $(SRCS_DIR)/shell/init.c \
	   $(SRCS_DIR)/parser/tokenize.c \
	   $(SRCS_DIR)/parser/parse_command.c \
	   $(SRCS_DIR)/parser/parse_utils.c \
	   $(SRCS_DIR)/parser/parse_create.c \
	   $(SRCS_DIR)/parser/parse_append.c \
	   $(SRCS_DIR)/parser/parse_free.c \
	   $(SRCS_DIR)/utils/string_array.c \
	   $(SRCS_DIR)/utils/split_words.c \
	   $(SRCS_DIR)/git_analyse/git_check.c \
	   $(SRCS_DIR)/expand/expand.c \
	   $(SRCS_DIR)/expand/history_expand.c \
	   $(SRCS_DIR)/expand/history_lookup.c \
	   $(SRCS_DIR)/expand/var_expand.c \
	   $(SRCS_DIR)/builtins/set.c \
	   $(SRCS_DIR)/builtins/unset.c \
	   $(SRCS_DIR)/builtins/alias.c \
	   $(SRCS_DIR)/builtins/unalias.c \
	   $(SRCS_DIR)/builtins/where.c \
	   $(SRCS_DIR)/builtins/which.c \
	   $(SRCS_DIR)/utils/which_where_utils.c \
	   $(SRCS_DIR)/env/locals.c \
	   $(SRCS_DIR)/env/aliases.c \
	   $(SRCS_DIR)/expand/alias_expand.c \
	   $(SRCS_DIR)/expand/glob_expand.c \
	   $(SRCS_DIR)/tui/tui_init.c \
	   $(SRCS_DIR)/tui/tui_input.c \
	   $(SRCS_DIR)/tui/tui_input_keys.c \
	   $(SRCS_DIR)/tui/tui_output.c \
	   $(SRCS_DIR)/tui/tui_resize.c \
	   $(SRCS_DIR)/tui/tui_sidebar.c \
	   $(SRCS_DIR)/tui/tui_theme.c \
	   $(SRCS_DIR)/tui/tui_theme_menu.c

BUILD_DIR = build
OBJS_DIR = $(BUILD_DIR)/obj
DEPS_DIR = $(BUILD_DIR)/dep
BIN_DIR = $(BUILD_DIR)/bin
TARGET = $(BIN_DIR)/$(NAME)
OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)
DEPS = $(SRCS:$(SRCS_DIR)/%.c=$(DEPS_DIR)/%.d)

all: $(NAME)

$(NAME): $(TARGET)
	ln -sf $(TARGET) $(NAME)


$(TARGET):$(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(TARGET) $(OBJS) -lncurses -lpthread


$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c include/*.h
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPS_DIR)/$*.d)
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEPS_DIR)/$*.d -MT $@ -c $< -o $@


clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME) $(TEST_NAME)
	rm -f *.gcno *.gcda *.gcov

re: fclean all

$(TEST_NAME): $(TEST_SRCS)
	gcc -Wall -Wextra -I./include -g3 --coverage -o $(TEST_NAME) $(TEST_SRCS) $(TEST_LDFLAGS)

tests_run: $(TEST_NAME)
	./$(TEST_NAME)
	gcovr --exclude tests/

compdb:
	@mkdir -p $(BUILD_DIR)
	bear --output $(BUILD_DIR)/compile_commands.json -- $(MAKE) -B $(OBJS)

.PHONY: all clean fclean re tests_run
