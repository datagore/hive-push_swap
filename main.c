#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// ANSI escape codes.
#define ANSI_GREEN  "\x1b[1;32m" // Set the text color to green.
#define ANSI_RED    "\x1b[1;31m" // Set the text color to red.
#define ANSI_RESET  "\x1b[0m"    // Reset to default color.
#define ANSI_CLEAR  "\x1b[2J"    // Clear the screen.

// Color-coded OK and KO strings.
#define GREEN_OK (ANSI_GREEN "[OK]" ANSI_RESET)
#define RED_KO   (ANSI_RED   "[KO]" ANSI_RESET)

void random_permutation(int *array, int length)
{
    for (int i = 0; i < length; i++)
        array[i] = i;
    for (int i = 0; i < length - 1; i++) {
        int j = rand() % (length - i) + i;
        int temporary = array[j];
        array[j] = array[i];
        array[i] = temporary;
    }
}

typedef struct s_stack {
	int *data;
	int cap;
	int len;
	int top;
} t_stack;

int stack_get(const t_stack *s, int index)
{
	return s->data[(s->top + s->len * (index < 0) + index) % s->cap];
}

void stack_set(t_stack *s, int index, int value)
{
	s->data[(s->top + s->len * (index < 0) + index) % s->cap] = value;
}

void stack_push(t_stack *dst, t_stack *src)
{
	if (src->len > 0) {
		int value = src->data[src->top];
		src->top = (src->top + 1) % src->cap;
		dst->top = (dst->top - 1 + dst->cap) % dst->cap;
		dst->data[dst->top] = value;
		dst->len++;
		src->len--;
	}
}

void stack_swap(t_stack *s)
{
	if (s->len > 1) {
		int temp = stack_get(s, 0);
		stack_set(s, 0, stack_get(s, 1));
		stack_set(s, 1, temp);
	}
}

void stack_rotate(t_stack *s, int step)
{
	if (step == -1)
		stack_set(s, s->len, stack_get(s, 0));
	s->top = (s->top - step + s->cap) % s->cap;
	if (step == +1)
		stack_set(s, 0, stack_get(s, s->len));
}

void stack_print(const t_stack *s)
{
	printf("[");
	for (int i = 0; i < s->len; i++)
		printf("%s%d", i ? ", " : "", stack_get(s, i));
	printf("]\n");
}

enum {sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr};

void print_move(int move_id)
{
	static const char* const move_names[] = {
		"sa\n", "sb\n", "ss\n", "pa\n", "pb\n", "ra\n", "rb\n", "rr\n", "rra\n",
		"rrb\n", "rrr\n"
	};
	fflush(stdout); // FIXME: Remove
	write(1, move_names[move_id], 3 + (move_id >= rra));
}

static int move_count;

void perform_move(t_stack *a, t_stack *b, int move)
{
	print_move(move);
	if (move == pa)
		stack_push(a, b);
	if (move == pb)
		stack_push(b, a);
	if (move == sa || move == ss)
		stack_swap(a);
	if (move == sb || move == ss)
		stack_swap(b);
	if (move == ra || move == rr)
		stack_rotate(a, -1);
	if (move == rb || move == rr)
		stack_rotate(b, -1);
	if (move == rra || move == rrr)
		stack_rotate(a, +1);
	if (move == rrb || move == rrr)
		stack_rotate(b, +1);
	move_count++;
}

void sort(t_stack *a, t_stack *b)
{
	const int length = a->len;
	while (a->len > 0)
		perform_move(a, b, pb);
	for (int i = length - 1; i >= 0; i--) {
		while (stack_get(b, 0) != i)
			perform_move(a, b, rb);
		perform_move(a, b, pa);
	}
}

int main(void)
{
	const int length = 100;
	int array[length * 4];
	random_permutation(array, length);

	t_stack stacks[2];
	for (int i = 0; i < 2; i++) {
		stacks[i].data = array + length * i * 2;
		stacks[i].len = length * (1 - i);
		stacks[i].cap = length * 2;
		stacks[i].top = 0;
	}

	printf("Input:\n");
	stack_print(&stacks[0]);
	stack_print(&stacks[1]);
	sort(&stacks[0], &stacks[1]);
	printf("\nOutput:\n");
	stack_print(&stacks[0]);
	stack_print(&stacks[1]);

	printf("\nMoves: %d\n", move_count);
}
