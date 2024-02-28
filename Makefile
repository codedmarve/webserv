# # Project name
# NAME = webserv 

# # Compiler and flags
# CC = c++
# CFLAGS = -Werror -Wall -Wextra -std=c++14 -pedantic -fsanitize=address
# RM = rm -rf

# # Directories
# SRC_DIR = src
# OBJ_DIR = obj
# INC_DIR = inc

# # Source files
# # SRC = $(wildcard $(SRC_DIR)/*.cpp)
# SRC =	$(wildcard *.cpp **/*.cpp **/*/*.cpp)
# TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)
# OBJS = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# # Targets
# all: $(NAME)

# $(NAME): $(OBJS)
# 	@$(CC) $(OBJS) -o $@ $(CFLAGS)
# 	@echo "\033[0;32m- Compiled $(NAME) -\033[0m"

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	@echo "\033[2;37m     - Building $<\033[0m"
# 	@mkdir -p $(OBJ_DIR)
# 	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@


# $(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
# 	@echo "\033[2;37m     - Building test $<\033[0m"
# 	@mkdir -p $(OBJ_DIR)
# 	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# clean:
# 	@$(RM) $(OBJ_DIR)/*.o $(NAME) test
# 	@echo "\033[2;37m     - Object files removed -\033[0m"

# fclean: clean
# 	@echo "\033[2;37m     - $(NAME) removed -\033[0m"

# re: fclean all

# .PHONY: all clean fclean re 





# Project name
NAME = webserv 

# Compiler and flags
CC = c++
CFLAGS = -Werror -Wall -Wextra -std=c++14 -pedantic -fsanitize=address -I/usr/include/curl
LDFLAGS = -lgtest -lgtest_main -pthread -lcurl
RM = rm -rf

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = inc
TEST_DIR = tests

# Source files
# SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRC =	$(wildcard *.cpp **/*.cpp **/*/*.cpp)
TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)
OBJS = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS = $(TEST_SRC:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets
all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS)
	@echo "\033[0;32m- Compiled $(NAME) -\033[0m"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "\033[2;37m     - Building $<\033[0m"
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

test: $(TEST_OBJS) $(OBJS)
	@$(CC) $(TEST_OBJS) $(OBJS) -o test $(LDFLAGS) $(CFLAGS)
	@echo "\033[0;32m- Compiled test executable -\033[0m"
	@./test

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@echo "\033[2;37m     - Building test $<\033[0m"
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	@$(RM) $(OBJ_DIR)/*.o $(NAME) test
	@echo "\033[2;37m     - Object files removed -\033[0m"

fclean: clean
	@echo "\033[2;37m     - $(NAME) removed -\033[0m"

re: fclean all

.PHONY: all clean fclean re test
