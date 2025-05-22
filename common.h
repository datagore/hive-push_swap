/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:01:01 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/22 17:20:01 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_H
# define COMMON_H

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

typedef struct s_stack
{
	int	*data;
	int	capacity;
	int	length;
	int	top;
}	t_stack;

void	rotate_stacks(t_stack *a, t_stack *b, int a_index, int b_index);
void	push_swap_sort(t_stack *a, t_stack *b);
void	stack_init(t_stack *s, int *data, int length, int capacity);
int		stack_get(const t_stack *s, int index);
void	stack_push(t_stack *dst, t_stack *src);
void	stack_swap(t_stack *s);
void	stack_rotate(t_stack *s, int step);
int		find_index_of_min_value(t_stack *a);
int		find_target_index(t_stack *stack, int value);
int		find_best_candidate(t_stack *a, t_stack *b);
int		find_longest_run(t_stack *a, t_stack *b);

#endif
