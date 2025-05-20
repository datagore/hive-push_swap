/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abostrom <abostrom@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:03:39 by abostrom          #+#    #+#             */
/*   Updated: 2025/05/20 12:04:00 by abostrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "common.h"

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
