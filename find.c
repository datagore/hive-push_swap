/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 17:12:29 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/22 17:54:36 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>

#include "common.h"

int	get_score(t_stack *a, t_stack *b, int a_index, int b_index)
{
	const int	ad = a_index - a->len * (a_index > a->len / 2);
	const int	bd = b_index - b->len * (b_index > b->len / 2);
	const int	as = ad * (ad > 0) - ad * (ad < 0);
	const int	bs = bd * (bd > 0) - bd * (bd < 0);
	const int	ab = as * (as < bs) + bs * (bs <= as);

	return (as + bs - (ad * bd > 0) * ab);
}

int	find_index_of_min_value(t_stack *a)
{
	int	lo;
	int	hi;
	int	mid_index;
	int	mid_value;

	lo = 0;
	hi = a->len - 1;
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
	hi = stack->len;
	while (lo < hi)
	{
		mid_index = (lo + hi) / 2;
		mid_value = stack_get(stack, (base + mid_index) % stack->len);
		if (mid_value > value)
			hi = mid_index;
		else
			lo = mid_index + 1;
	}
	return ((base + hi + stack->len) % stack->len);
}

int	find_best_candidate(t_stack *a, t_stack *b)
{
	int	best_index;
	int	best_score;
	int	a_index;
	int	b_index;
	int	score;

	b_index = 0;
	best_score = INT_MAX;
	while (b_index < b->len)
	{
		a_index = find_target_index(a, stack_get(b, b_index));
		score = get_score(a, b, a_index, b_index);
		if (score <= best_score)
		{
			best_score = score;
			best_index = b_index;
		}
		b_index++;
	}
	return (best_index);
}

int	find_longest_run(t_stack *a, t_stack *b)
{
	int	i;
	int	j;
	int	start;
	int	count;

	i = 0;
	j = 0;
	start = 0;
	count = 0;
	while (i < a->len * 2)
	{
		if (stack_get(a, i % a->len) < stack_get(a, (i + a->len - 1) % a->len))
		{
			if (i - j > count)
			{
				count = i - j;
				start = j;
			}
			j = i;
		}
		i++;
	}
	rotate_stacks(a, b, (start + count) % a->len, 0);
	return (a->len - count);
}
