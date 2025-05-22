/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 17:16:51 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/22 17:19:58 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

#include "common.h"

static void	make_move(t_stack *a, t_stack *b, int move)
{
	static const char *const	move_names[] = {
		"sa\n", "sb\n", "ss\n",
		"pa\n", "pb\n",
		"ra\n", "rb\n", "rr\n",
		"rra\n", "rrb\n", "rrr\n"
	};

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
	write(1, move_names[move], 3 + (move >= rra));
}

void	rotate_stacks(t_stack *a, t_stack *b, int a_index, int b_index)
{
	int	a_dist;
	int	b_dist;

	a_dist = a_index - a->length * (a_index > a->length / 2);
	b_dist = b_index - b->length * (b_index > b->length / 2);
	while (a_dist * b_dist > 0)
	{
		make_move(a, b, rr + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
	while (a_dist != 0)
	{
		make_move(a, b, ra + 3 * (a_dist < 0));
		a_dist += (a_dist < 0) - (a_dist > 0);
	}
	while (b_dist != 0)
	{
		make_move(a, b, rb + 3 * (b_dist < 0));
		b_dist += (b_dist < 0) - (b_dist > 0);
	}
}

static void	sort_three(t_stack *a, t_stack *b)
{
	const int	x = stack_get(a, 0);
	const int	y = stack_get(a, 1);
	const int	z = stack_get(a, 2);

	if (x < y && y > z && x < z)
	{
		make_move(a, b, ra);
		make_move(a, b, sa);
		make_move(a, b, rra);
	}
	if (x > y && y < z && x < z)
		make_move(a, b, sa);
	if (x < y && y > z && x > z)
		make_move(a, b, rra);
	if (x > y && y < z && x > z)
		make_move(a, b, ra);
	if (x > y && y > z && x > z)
	{
		make_move(a, b, ra);
		make_move(a, b, sa);
	}
}

void	push_swap_sort(t_stack *a, t_stack *b)
{
	const int	midpoint = a->length / 2;
	int			to_push;
	int			a_index;
	int			b_index;

	to_push = find_longest_run(a, b);
	while (a->length > 3 && to_push-- > 0)
	{
		if (midpoint < 3 || stack_get(a, 0) < midpoint)
			make_move(a, b, pb);
		else
		{
			make_move(a, b, pb);
			make_move(a, b, rb);
		}
	}
	if (a->length == 3)
		sort_three(a, b);
	while (b->length > 0)
	{
		b_index = find_best_candidate(a, b);
		a_index = find_target_index(a, stack_get(b, b_index));
		rotate_stacks(a, b, a_index, b_index);
		make_move(a, b, pa);
	}
	rotate_stacks(a, b, find_index_of_min_value(a), 0);
}
