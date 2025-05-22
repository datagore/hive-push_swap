/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:35:59 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/21 10:30:34 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

int	make_move(t_stack *a, t_stack *b, int move)
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

	while (a->length > 1)
	{
		if (stack_get(a, 0) < midpoint)
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

void	error(int *array)
{
	free(array);
	write(2, "Error\n", 6);
	exit(1);
}

void	parse_int(int *array, char **argv, int index)
{
	char	*str;
	long	sign;
	long	value;

	sign = 1;
	value = 0;
	str = argv[index + 1];
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '-' || *str == '+')
		sign = (*str++ == '+') * 2 - 1;
	if (*str < '0' || *str > '9')
		error(array);
	while (*str >= '0' && *str <= '9')
		value = value * 10 + (*str++ - '0');
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	value *= sign;
	if (*str != '\0' || value < INT_MIN || value > INT_MAX)
		error(array);
	array[index] = value;
}

void	sort_array(int *array, int length)
{
	int	pivot;
	int	split;
	int	temp;
	int	i;

	if (length < 2)
		return ;
	i = -1;
	split = 0;
	pivot = array[length - 1];
	while (++i < length)
	{
		if (array[i] <= pivot)
		{
			temp = array[i];
			array[i] = array[split];
			array[split++] = temp;
		}
	}
	sort_array(array, split - 1);
	sort_array(array + split, length - split);
}

void	check_for_duplicates(int *numbers, int length)
{
	int	i;
	int	*copy;

	i = 0;
	copy = numbers + length;
	while (i < length)
	{
		copy[i] = numbers[i];
		i++;
	}
	sort_array(copy, length);
	i = 0;
	while (i < length - 1)
	{
		if (copy[i] == copy[i + 1])
			error(numbers);
		i++;
	}
}

void	rank_numbers(int *numbers, const int *sorted, int length)
{
	int	i;
	int	lo;
	int	hi;
	int	mid;

	i = 0;
	while (i < length)
	{
		lo = 0;
		hi = length - 1;
		while (1)
		{
			mid = (lo + hi) / 2;
			if (numbers[i] < sorted[mid])
				hi = mid - 1;
			else if (numbers[i] > sorted[mid])
				lo = mid + 1;
			else
				break ;
		}
		numbers[i] = mid;
		i++;
	}
}

int	main(int argc, char **argv)
{
	t_stack		stacks[2];
	const int	length = argc - 1;
	int			*array;
	int			i;

	if (length < 1)
		return (0);
	array = malloc(length * 4 * sizeof(int));
	if (array == NULL)
		error(NULL);
	i = 0;
	while (i < length)
		parse_int(array, argv, i++);
	check_for_duplicates(array, length);
	rank_numbers(array, array + length, length);
	i = 0;
	while (i < 2)
	{
		stacks[i].data = array + length * i * 2;
		stacks[i].length = length * (1 - i);
		stacks[i].capacity = length * 2;
		stacks[i].top = 0;
		i++;
	}
	sort(&stacks[0], &stacks[1]);
	free(array);
}
