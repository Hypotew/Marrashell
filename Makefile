NAME = 42sh
CC = epiclang
CFLAGS = -Wall -Wextra -I./include -g3
LDFLAGS = -lncurses

SRCS_DIR = src
SRCS = $(SRCS_DIR)/main.c \
	   $(SRCS_DIR)/builtins/run_builtin.c \
	   $(SRCS_DIR)/builtins/exit.c \
	   $(SRCS_DIR)/builtins/cd.c \
	   $(SRCS_DIR)/builtins/setenv.c \
	   $(SRCS_DIR)/builtins/unsetenv.c \
	   $(SRCS_DIR)/builtins/env.c \
	   $(SRCS_DIR)/builtins/echo_last_status.c \
	   $(SRCS_DIR)/builtins/history.c \
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
	   $(SRCS_DIR)/utils/history_utils.c \
	   $(SRCS_DIR)/shell/readline.c \
	   $(SRCS_DIR)/shell/readline_hist.c \

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
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)


$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c include/*.h
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPS_DIR)/$*.d)
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEPS_DIR)/$*.d -MT $@ -c $< -o $@


clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

tests_run: all
	$(MAKE) -C tests re
	./tests/mysh_tests

compdb:
	@mkdir -p $(BUILD_DIR)
	bear --output $(BUILD_DIR)/compile_commands.json -- $(MAKE) -B $(OBJS)

.PHONY: all clean fclean re tests_run
