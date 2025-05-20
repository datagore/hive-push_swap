/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:01:01 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/20 12:04:49 by abostrom         ###   ########.fr       */
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

int		stack_get(const t_stack *s, int index);
void	stack_push(t_stack *dst, t_stack *src);
void	stack_swap(t_stack *s);
void	stack_rotate(t_stack *s, int step);

#endif
