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
	int length[2];
	int operation_count;
} t_state;

enum {sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr};

void display_state(const t_state *s)
{
	for (int i = 0; i < 2; i++) {
		printf("%c:", "AB"[i]);
		for (int j = 0; j < s->length[i]; j++)
			printf(" %d", s->stack[i][j]);
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

int array_is_sorted(const int *array, int length)
{
	for (int i = 1; i < length; i++)
		if (array[i - 1] >= array[i])
			return 0;
	return 1;
}

void array_rotate(int *array, int length, int step)
{
	int temp = array[(length - step) % length];
	for (int i = 1; i <= length; i++) {
		int j = (i * step + length - step) % length;
		int temp2 = array[j];
		array[j] = temp;
		temp = temp2;
	}
}

void array_push(int *dst, int *src, int dst_length, int src_length)
{
	int popped = src[0];
	array_rotate(src, src_length + 0, -1);
	array_rotate(dst, dst_length + 1, +1);
	dst[0] = popped;
}

void run_operation(t_state *s, int op)
{
	if ((op == sa || op == ss) && s->length[0] > 1)
		swap(&s->stack[0][0], &s->stack[0][1]);
	if ((op == sb || op == ss) && s->length[1] > 1)
		swap(&s->stack[1][0], &s->stack[1][1]);
	if (op == pa && s->length[1] > 0)
		array_push(s->stack[0], s->stack[1], ++s->length[0], s->length[1]--);
	if (op == pb && s->length[0] > 0)
		array_push(s->stack[1], s->stack[0], ++s->length[1], s->length[0]--);
	if (op == ra || op == rr)
		array_rotate(s->stack[0], s->length[0], -1);
	if (op == rb || op == rr)
		array_rotate(s->stack[1], s->length[1], -1);
	if (op == rra || op ==	rrr)
		array_rotate(s->stack[0], s->length[0], +1);
	if (op == rrb || op ==	rrr)
		array_rotate(s->stack[1], s->length[1], +1);
	s->operation_count++;
}

int index_of_value(const int *array, int length, int value)
{
	for (int i = 0; i < length; i++)
		if (array[i] == value)
			return i;
	return -1;
}

#if 1 // Dumb sort, rotates until finding the next value in the order.
void sort(t_state *s)
{
	const int input_length = s->length[0];
	for (int i = 0; i < input_length; i++) {
		int a = index_of_value(s->stack[0], s->length[0], i);
		int b = index_of_value(s->stack[1], s->length[1], i);
		if (a == 0) {
			run_operation(s, ra);
			continue;
		} else if (a > 0) {
			run_operation(s, pb);
		} else if (b == 0) {
			run_operation(s, pa);
		} else if (b > 0) {
			if (b * 2 > s->length[1]) {
				run_operation(s, rrb);
			} else {
				run_operation(s, rb);
			}
		}
		i--;
	}
}
#endif

int main(void)
{
	const int length = 500;
	int stacks[2][length * 2];
	random_permutation(stacks[0], length);
	t_state state = {
		.stack = {stacks[0], stacks[1]},
		.length[0] = length,
	};
	printf("Input:\n");
	display_state(&state);
	sort(&state);
	printf("Output:\n");
	display_state(&state);
	int is_sorted = array_is_sorted(state.stack[0], state.length[0]);
	printf("Total operations: %d\n", state.operation_count);
	printf("Order: %s\n", is_sorted ? GREEN_OK : RED_KO);
	printf("Moves: %d", state.operation_count);
	printf(" (%d%% of target)", state.operation_count * 100 / 5500);
	printf(" %s\n", state.operation_count < 5500 ? GREEN_OK : RED_KO);
}
