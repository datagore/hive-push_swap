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
	for (int i = 0; i < s->len; i++)
		printf("%s%d", i ? " " : "", stack_get(s, i));
	printf("\n");
}

enum {sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr};

void print_move(int move_id)
{
	static const char* const move_names[] = {
		"sa\n", "sb\n", "ss\n", "pa\n", "pb\n", "ra\n", "rb\n", "rr\n", "rra\n",
		"rrb\n", "rrr\n"
	};
#if 0
	write(1, move_names[move_id], 3 + (move_id >= rra));
#else
	(void) move_names, (void) move_id;
#endif
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

int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

int find_index_of_min_value(t_stack *a)
{
	int min_index = 0;
	int min_value = INT_MAX;
	for (int index = 0; index < a->len; index++) {
		int value = stack_get(a, index);
		if (value < min_value) {
			min_value = value;
			min_index = index;
		}
	}
	return min_index;
}

int find_place(t_stack *a, t_stack *b, int b_index)
{
	// TODO: Use binary search somehow.
	int b_value = stack_get(b, b_index);
	int a_index = find_index_of_min_value(a);
	for (int i = 0; i < a->len && stack_get(a, a_index) < b_value; i++)
		a_index = (a_index + 1) % a->len;
	return a_index;
}

int move_number(t_stack *a, t_stack *b, int b_index, int move)
{
	int rb_score = b_index;
	int ra_score = find_place(a, b, b_index);
	int rra_score = a->len - ra_score;
	int rrb_score = b->len - rb_score;
	int score = 0;
#if 1
	if (ra_score < rra_score && rb_score < rrb_score)
		while (ra_score > 0 && rb_score > 0) {
			if (move) perform_move(a, b, rr);
			ra_score--;
			rb_score--;
			score++;
	}
	if (rra_score < ra_score && rrb_score < rb_score)
		while (rra_score > 0 && rrb_score > 0) {
			if (move) perform_move(a, b, rrr);
			rra_score--;
			rrb_score--;
			score++;
	}
#endif
	if (ra_score < rra_score) {
		while (ra_score-- > 0) {
			if (move) perform_move(a, b, ra);
			score++;
		}
	} else {
		while (rra_score-- > 0) {
			if (move) perform_move(a, b, rra);
			score++;
		}
	}
	if (rb_score < rrb_score) {
		while (rb_score-- > 0) {
			if (move) perform_move(a, b, rb);
			score++;
		}
	} else {
		while (rrb_score-- > 0) {
			if (move) perform_move(a, b, rrb);
			score++;
		}
	}
	if (move) perform_move(a, b, pa);
	return score;
}

int get_index_of_best_move(t_stack *a, t_stack *b)
{
	int best_index;
	int best_score = INT_MAX;
	for (int index = 0; index < b->len; index++) {
		int score = move_number(a, b, index, 0);
		if (score <= best_score) {
			best_score = score;
			best_index = index;
		}
	}
	return best_index;
}

int get_index_of_value(t_stack *s, int value)
{
	int index = 0;
	while (index < s->len && stack_get(s, index) != value)
		index++;
	return index;
}

void rotate_to_index(t_stack *a, t_stack *b, int index)
{
	if (index < b->len / 2)
		for (int i = 0; i < index; i++)
			perform_move(a, b, rb);
	else
		for (int i = b->len; i > index; i--)
			perform_move(a, b, rrb);
}

void sort(t_stack *a, t_stack *b)
{
	const int length = a->len;
	while (a->len > 1) {
		int value = stack_get(a, 0);
		if (value < length / 2)
			perform_move(a, b, pb);
		else {
			perform_move(a, b, pb);
			perform_move(a, b, rb);
		}
	}
	while (b->len > 0) {
		int b_index = get_index_of_best_move(a, b);
		move_number(a, b, b_index, 1);
	}
}

int main(void)
{
#if 1
	srand(time(NULL));
	rand();
#endif

	const int length = 500;
	int array[length * 4];
	random_permutation(array, length);

	t_stack stacks[2];
	for (int i = 0; i < 2; i++) {
		stacks[i].data = array + length * i * 2;
		stacks[i].len = length * (1 - i);
		stacks[i].cap = length * 2;
		stacks[i].top = 0;
	}

	printf(ANSI_CLEAR);
	printf("Input:\n");
	stack_print(&stacks[0]);
	stack_print(&stacks[1]);
	printf("\n");
	fflush(stdout);
	sort(&stacks[0], &stacks[1]);
	printf("\nOutput:\n");
	stack_print(&stacks[0]);
	stack_print(&stacks[1]);

	printf("\nMoves: %d\n", move_count);
}
