#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

enum {sa, sb, ss, pa, pb, ra, rb, rr, rra, rrb, rrr};

static int move_count;

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
	move_count++;
}

typedef struct s_stack {
	int *data;
	int cap;
	int len;
	int top;
} t_stack;

int stack_get(const t_stack *s, int index)
{
	return s->data[(s->top + index) % s->cap];
}

void stack_set(t_stack *s, int index, int value)
{
	s->data[(s->top + index) % s->cap] = value;
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

int perform_move(t_stack *a, t_stack *b, int move)
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
	return (1);
}

int find_index_of_min_value(t_stack *a)
{
	int lo = 0, hi = a->len - 1;
	while (1) {
		int mid = (lo + hi) / 2;
		int value = stack_get(a, mid);
		if (stack_get(a, lo) > value) { hi = mid - 0; continue; }
		if (stack_get(a, hi) < value) { lo = mid + 1; continue; }
		return lo;
	}
}

int find_place(t_stack *a, t_stack *b, int b_index)
{
	// TODO: Use binary search somehow.
#if 0
	int base = find_index_of_min_value(a);
	int b_value = stack_get(b, b_index);
	int lo = 0, hi = a->len - 1;
	while (lo <= hi) {
		int mid = (lo + hi) / 2;
		int a_value = stack_get(a, base + mid);
		if (b_value > a_value) { hi = mid - 1; continue; }
		if (b_value < a_value) { lo = mid + 1; continue; }
		return mid;
	}
	return lo;
#else
	int b_value = stack_get(b, b_index);
	int a_index = find_index_of_min_value(a);
	for (int i = 0; i < a->len && stack_get(a, a_index) < b_value; i++)
		a_index = (a_index + 1) % a->len;
	return a_index;
#endif
}

int move_number(t_stack *a, t_stack *b, int b_index, int score_only)
{
	int score = 0;
	int a_index = find_place(a, b, b_index);
	int a_dist = a_index - a->len * (a_index > a->len / 2);
	int b_dist = b_index - b->len * (b_index > b->len / 2);
	while (a_dist * b_dist > 0) {
		score += score_only || perform_move(a, b, rr + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
	while (a_dist != 0) {
		score += score_only || perform_move(a, b, ra + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
	}
	while (b_dist != 0) {
		score += score_only || perform_move(a, b, rb + 3 * (b_dist < 0));
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
	return score + (score_only || perform_move(a, b, pa));
}

int get_index_of_best_move(t_stack *a, t_stack *b)
{
	int best_index;
	int best_score = INT_MAX;
	for (int index = 0; index < b->len; index++) {
		int score = move_number(a, b, index, 1);
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

void rotate_to_top(t_stack *a, t_stack *b, int index)
{
	int move = index < a->len / 2 ? ra : rra;
	while (stack_get(a, 0) != 0)
		perform_move(a, b, move);
}

void sort(t_stack *a, t_stack *b)
{
	const int half = a->len / 2;
	while (a->len > 1) {
		int value = stack_get(a, 0);
		if (value < half)
			perform_move(a, b, pb);
		else {
			perform_move(a, b, pb);
			perform_move(a, b, rb);
		}
	}
	while (b->len > 0) {
		int b_index = get_index_of_best_move(a, b);
		move_number(a, b, b_index, 0);
	}
	rotate_to_top(a, b, find_index_of_min_value(a));
}

////////////////////////////////////////////////////////////////////////////////
//
// TEST CODE BELOW
//
////////////////////////////////////////////////////////////////////////////////

// ANSI escape codes.
#define ANSI_GREEN  "\x1b[1;32m" // Set the text color to green.
#define ANSI_RED    "\x1b[1;31m" // Set the text color to red.
#define ANSI_RESET  "\x1b[0m"    // Reset to default color.
#define ANSI_CLEAR  "\x1b[2J"    // Clear the screen.

// Color-coded OK and KO strings.
#define GREEN_OK (ANSI_GREEN "[OK]" ANSI_RESET)
#define RED_KO   (ANSI_RED   "[KO]" ANSI_RESET)

static int stack_is_sorted(const t_stack *stack)
{
	for (int i = 0; i < stack->len - 1; i++)
		if (stack_get(stack, i) >= stack_get(stack, i + 1))
			return 0;
	return 1;
}

static void stack_randomize(int *array, int length)
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

static void stack_print(const t_stack *s)
{
	for (int i = 0; i < s->len; i++)
		printf("%s%d", i ? " " : "", stack_get(s, i));
	printf("\n");
}

int main(void)
{
	srand(time(NULL));
	rand();

	// TODO: Use minimal sequence for 3 and 5.
	const int length = 500;
	int array[length * 4];
	stack_randomize(array, length);

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

	int sorted = stack_is_sorted(&stacks[0]);
	printf("\n%s ", move_count < 5500 ? GREEN_OK : RED_KO);
	printf("%d moves\n", move_count);
	printf("%s %ssorted\n", sorted ? GREEN_OK : RED_KO, sorted ? "" : "not ");
}
