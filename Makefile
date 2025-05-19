NAME := test
CFLAGS := -Wall -Wextra -Wpedantic -O0 -ggdb3 \
	-fsanitize=address -fsanitize=undefined -fsanitize=leak

run: $(NAME)
	./$(NAME)

$(NAME): main.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	$(RM) $(NAME)

.PHONY: clean run
