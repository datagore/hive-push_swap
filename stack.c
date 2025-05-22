/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:03:39 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/22 17:24:43 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

void	stack_init(t_stack *s, int *data, int length, int capacity)
{
	s->data = data;
	s->len = length;
	s->cap = capacity;
	s->top = 0;
}

int	stack_get(const t_stack *s, int index)
{
	return (s->data[(s->top + index) % s->cap]);
}

void	stack_push(t_stack *dst, t_stack *src)
{
	const int	value = src->data[src->top];

	if (src->len == 0)
		return ;
	src->top = (src->top + 1 + dst->cap) % src->cap;
	dst->top = (dst->top - 1 + dst->cap) % dst->cap;
	dst->data[dst->top] = value;
	dst->len++;
	src->len--;
}

void	stack_swap(t_stack *s)
{
	const int	next = (s->top + 1) % s->cap;
	const int	temp = s->data[s->top];

	if (s->len == 0)
		return ;
	s->data[s->top] = s->data[next];
	s->data[next] = temp;
}

void	stack_rotate(t_stack *s, int step)
{
	if (step == -1)
		s->data[(s->top + s->len) % s->cap] = stack_get(s, 0);
	s->top = (s->top - step + s->cap) % s->cap;
	if (step == +1)
		s->data[s->top] = stack_get(s, s->len);
}
