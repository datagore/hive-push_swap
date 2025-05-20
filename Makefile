NAME := push_swap
SRC := main.c stack.c
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)
CFLAGS := -Wall -Wextra -Wpedantic -MMD -MP

$(NAME): $(OBJ)
	$(CC) $^ -o $@

all: $(NAME)

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re run

-include $(DEP)
