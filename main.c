// TODO: Write operations to an array before outputting or counting them
// TODO: Optimize by merging or eliminating redundant operations
// FIXME: Input currently consists of all numbers between 0 and N

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ANSI escape codes.
#define ANSI_GREEN	"\x1b[1;32m" // Set the text color to green.
#define ANSI_RED	"\x1b[1;31m" // Set the text color to red.
#define ANSI_RESET	"\x1b[0m"	 // Reset to default color.
#define ANSI_CLEAR	"\x1b[2J"	 // Clear the screen.

// Color-coded OK and KO strings.
#define GREEN_OK (ANSI_GREEN "[OK]" ANSI_RESET)
#define RED_KO	 (ANSI_RED	 "[KO]" ANSI_RESET)

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

typedef struct s_state {
	int *stack[2];
	int top[2];
	int len[2];
	int operation_count;
	int wrap;
} t_state;

enum {sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr};

void display_state(const t_state *s)
{
	for (int i = 0; i < 2; i++) {
		printf("%c:", "AB"[i]);
		for (int j = 0; j < s->len[i]; j++)
			printf(" %d", s->stack[i][(s->top[i] + j) % s->wrap]);
		printf("\n");
	}
	printf("\n");
}

void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

int stack_is_sorted(t_state *s, int dst)
{
	for (int i = 1; i < s->len[dst]; i++)
		if (s->stack[dst][(s->top[dst] + i - 1) % s->wrap] >= s->stack[dst][(s->top[dst] + i) % s->wrap])
			return 0;
	return 1;
}

void stack_swap(t_state *s, int dst)
{
	if (s->len[dst] > 1)
	{
		int i = (s->top[dst] + 0) % s->wrap;
		int j = (s->top[dst] + 1) % s->wrap;
		int temp = s->stack[dst][i];
		s->stack[dst][i] = s->stack[dst][j];
		s->stack[dst][j] = temp;
	}
}

void stack_push(t_state *s, int dst, int src)
{
	if (s->len[src] > 0)
	{
		s->top[dst] = (s->top[dst] + s->wrap - 1) % s->wrap;
		s->stack[dst][s->top[dst]] = s->stack[src][s->top[src]];
		s->top[src] = (s->top[src] + 1) % s->wrap;
		s->len[dst]++;
		s->len[src]--;
	}
}

void stack_rotate(t_state *s, int dst, int step)
{
	if (step == 1)
	{
		s->top[dst] = (s->top[dst] + s->wrap - step) % s->wrap;
		s->stack[dst][s->top[dst]] = s->stack[dst][(s->top[dst] + s->len[dst]) % s->wrap];
	}
	else if (step == -1)
	{
		s->top[dst] = (s->top[dst] + s->wrap - step) % s->wrap;
		s->stack[dst][(s->top[dst] + s->len[dst] + step) % s->wrap] = s->stack[dst][(s->top[dst] + s->wrap + step) % s->wrap];
	}
}

void run_operation(t_state *s, int op)
{
	if (op == sa || op == ss)
		stack_swap(s, 0);
	if (op == sb || op == ss)
		stack_swap(s, 1);
	if (op == pa)
		stack_push(s, 0, 1);
	if (op == pb)
		stack_push(s, 1, 0);
	if (op == ra || op == rr)
		stack_rotate(s, 0, -1);
	if (op == rb || op == rr)
		stack_rotate(s, 1, -1);
	if (op == rra || op == rrr)
		stack_rotate(s, 0, +1);
	if (op == rrb || op == rrr)
		stack_rotate(s, 1, +1);
	s->operation_count++;
}

int index_of_value(t_state *s, int stack, int value)
{
	for (int i = 0; i < s->len[stack]; i++)
		if (s->stack[stack][(s->top[stack] + i) % s->wrap] == value)
			return i;
	return -1;
}

// Dumb sort, rotates until finding the next value in the order.
void sort(t_state *s)
{
	const int input_length = s->len[0];
	for (int i = 0; i < input_length; i++) {
		int a = index_of_value(s, 0, i);
		int b = index_of_value(s, 1, i);
		if (a == 0) {
			run_operation(s, ra);
			continue;
		} else if (a > 0) {
			run_operation(s, pb);
		} else if (b == 0) {
			run_operation(s, pa);
		} else if (b > 0) {
			if (b * 2 > s->len[1]) {
				run_operation(s, rrb);
			} else {
				run_operation(s, rb);
			}
		}
		i--;
	}
}

int main(void)
{
	const int length = 5;
	int stacks[2][length * 2];
	random_permutation(stacks[0], length);
	t_state state = {
		.stack = {stacks[0], stacks[1]},
		.len[0] = length,
		.wrap = length * 2,
	};
	printf("Input:\n");
	display_state(&state);
	sort(&state);
	printf("Output:\n");
	display_state(&state);
	int is_sorted = stack_is_sorted(&state, 0);
	printf("Total operations: %d\n", state.operation_count);
	printf("Order: %s\n", is_sorted ? GREEN_OK : RED_KO);
	printf("Moves: %d", state.operation_count);
	printf(" (%d%% of target)", state.operation_count * 100 / 5500);
	printf(" %s\n", state.operation_count < 5500 ? GREEN_OK : RED_KO);
}
