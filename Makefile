NAME := push_swap
SRC := main.c stack.c
OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.d)
CFLAGS := -Wall -Wextra -Werror -MMD -MP

$(NAME): $(OBJ)
	$(CC) $^ -o $@

all: $(NAME)

clean:
	$(RM) $(OBJ) $(DEP)

fclean: clean
	$(RM) $(NAME)

re: fclean all

test: all
	python3 rng.py 500 > numbers.txt
	./push_swap `cat numbers.txt` | wc -l
	./push_swap `cat numbers.txt` | ./checker_linux `cat numbers.txt`
	rm numbers.txt

.PHONY: all clean fclean re run
.SECONDARY: $(OBJ) $(DEP)
.SILENT:

-include $(DEP)
