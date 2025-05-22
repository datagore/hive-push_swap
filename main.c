/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 15:35:59 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/22 17:27:12 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

static int	read_input(int *array, char **strings, int length)
{
	char	*str;
	long	sign;
	long	value;

	while (length-- > 0)
	{
		sign = 1;
		value = 0;
		str = strings[length];
		while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
			str++;
		if (*str == '-' || *str == '+')
			sign = (*str++ == '+') * 2 - 1;
		if (*str < '0' || *str > '9')
			return (0);
		while (*str >= '0' && *str <= '9')
			value = value * 10 + (*str++ - '0');
		while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
			str++;
		value *= sign;
		if (*str != '\0' || value < INT_MIN || value > INT_MAX)
			return (0);
		array[length] = value;
	}
	return (1);
}

static void	sort_array(int *array, int length)
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

static int	all_unique(int *numbers, int length)
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
			return (0);
		i++;
	}
	return (1);
}

static void	rank_numbers(int *numbers, const int *sorted, int length)
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
	t_stack		a;
	t_stack		b;
	const int	len = argc - 1;
	int			*array;

	if (len < 1)
		return (0);
	array = malloc(len * 4 * sizeof(int));
	if (array && read_input(array, argv + 1, len) && all_unique(array, len))
	{
		rank_numbers(array, array + len, len);
		stack_init(&a, array + len * 0, len * 1, len * 2);
		stack_init(&b, array + len * 2, len * 0, len * 2);
		push_swap_sort(&a, &b);
		rotate_stacks(&a, &b, find_index_of_min_value(&a), 0);
	}
	else
		write(2, "Error\n", 6);
	free(array);
}
