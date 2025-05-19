#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

enum
{
	sa,
	sb,
	ss,
	pa,
	pb,
	ra,
	rb,
	rr,
	rra,
	rrb,
	rrr
};

static int move_count;

void	print_move(int move_id)
{
	static const char *const	move_names[] = {
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

typedef struct s_stack
{
	int	*data;
	int	capacity;
	int	length;
	int	top;
}	t_stack;

int	stack_get(const t_stack *s, int index)
{
	return (s->data[(s->top + index) % s->capacity]);
}

void	stack_push(t_stack *dst, t_stack *src)
{
	const int	value = src->data[src->top];

	if (src->length == 0)
		return ;
	src->top = (src->top + 1 + dst->capacity) % src->capacity;
	dst->top = (dst->top - 1 + dst->capacity) % dst->capacity;
	dst->data[dst->top] = value;
	dst->length++;
	src->length--;
}

void	stack_swap(t_stack *s)
{
	const int	next = (s->top + 1) % s->capacity;
	const int	temp = s->data[s->top];

	if (s->length == 0)
		return ;
	s->data[s->top] = s->data[next];
	s->data[next] = temp;
}

void	stack_rotate(t_stack *s, int step)
{
	if (step == -1)
		s->data[(s->top + s->length) % s->capacity] = stack_get(s, 0);
	s->top = (s->top - step + s->capacity) % s->capacity;
	if (step == +1)
		s->data[s->top] = stack_get(s, s->length);
}

int	make_move(t_stack *a, t_stack *b, int move)
{
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
	print_move(move);
	return (1);
}

int	find_index_of_min_value(t_stack *a)
{
	int	lo;
	int	hi;
	int	mid_index;
	int	mid_value;

	lo = 0;
	hi = a->length - 1;
	while (1)
	{
		mid_index = (lo + hi) / 2;
		mid_value = stack_get(a, mid_index);
		if (stack_get(a, lo) > mid_value)
			hi = mid_index;
		else if (stack_get(a, hi) < mid_value)
			lo = mid_index + 1;
		else
			return (lo);
	}
}

int	find_target_index(t_stack *stack, int value)
{
	const int	base = find_index_of_min_value(stack);
	int			mid_index;
	int			mid_value;
	int			lo;
	int			hi;

	lo = 0;
	hi = stack->length;
	while (lo < hi)
	{
		mid_index = (lo + hi) / 2;
		mid_value = stack_get(stack, (base + mid_index) % stack->length);
		if (mid_value > value)
			hi = mid_index;
		else
			lo = mid_index + 1;
	}
	return ((base + hi + stack->length) % stack->length);
}

int	move_number(t_stack *a, t_stack *b, int b_index, int score_only)
{
	const int	a_index = find_target_index(a, stack_get(b, b_index));
	int			score;
	int			a_dist;
	int			b_dist;

	score = 0;
	a_dist = a_index - a->length * (a_index > a->length / 2);
	b_dist = b_index - b->length * (b_index > b->length / 2);
	while (a_dist * b_dist > 0)
	{
		score += score_only || make_move(a, b, rr + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
	while (a_dist != 0)
	{
		score += score_only || make_move(a, b, ra + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
	}
	while (b_dist != 0)
	{
		score += score_only || make_move(a, b, rb + 3 * (b_dist < 0));
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
	return (score + (score_only || make_move(a, b, pa)));
}

int	find_best_candidate(t_stack *a, t_stack *b)
{
	int	best_index;
	int	best_score;
	int	index;
	int	score;

	index = 0;
	best_score = INT_MAX;
	while (index < b->length)
	{
		score = move_number(a, b, index, 1);
		if (score <= best_score)
		{
			best_score = score;
			best_index = index;
		}
		index++;
	}
	return (best_index);
}

void	sort(t_stack *a, t_stack *b)
{
	const int	midpoint = a->length / 2;
	int			index_of_zero;
	int			value;

	while (a->length > 1)
	{
		value = stack_get(a, 0);
		if (value < midpoint)
			make_move(a, b, pb);
		else
		{
			make_move(a, b, pb);
			make_move(a, b, rb);
		}
	}
	while (b->length > 0)
		move_number(a, b, find_best_candidate(a, b), 0);
	index_of_zero = find_index_of_min_value(a);
	while (stack_get(a, 0) != 0)
		make_move(a, b, ra + 3 * (index_of_zero > midpoint));
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
	for (int i = 0; i < stack->length - 1; i++)
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
	for (int i = 0; i < s->length; i++)
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
		stacks[i].length = length * (1 - i);
		stacks[i].capacity = length * 2;
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
