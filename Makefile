NAME		= matt_daemon

# Compiler and flags
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++17 -g3
INCLUDES	= -I./includes

# Directories
SRC_DIR		= src
OBJ_DIR		= obj
INC_DIR		= includes

SRCS		= main.cpp

# Object files
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

# Colors for output
GREEN		= \033[0;32m
RED			= \033[0;31m
RESET		= \033[0m

# Rules
all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN) $(NAME) created successfully$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJ_DIR)
	@echo "$(RED) Clean complete$(RESET)"

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@echo "$(RED) Clean complete$(RESET)"

re: fclean all

.PHONY: all clean fclean re
