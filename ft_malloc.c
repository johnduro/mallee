/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/20 15:48:08 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/20 18:14:00 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#define TINY_M 512
#define SMALL_M 4096

typedef struct		s_big
{
//	int			addr;
	t_big		*next;
//	void		*pool;
}					t_big;

typedef struct		s_sm
{
//	int			addr;
	int			tab[100];
	t_sm		*next;
//	void		*pool;
}					t_sm;

typedef struct		s_malloc
{
	t_sm		*small_m;
	t_sm		*medium_m;
	t_big		*big_m;
}					t_malloc;


	t_malloc			pool;


void	init_small(t_sm **mem)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		*(mem)->tab[i] = 0;
		i++;
	}
	*(mem)->next = NULL;
}

void	*find_alloc(t_sm *mem)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		if (mem->tab[i] == 0)
		{
			return (mem + ((i + 1) * TINY_M));
		}
	}
	return (NULL);
}

void	*small_malloc(size_t size)
{
	void	*ret;
	t_sm	*bws_small;
	t_sm	*keep;

	if (!pool.small_m)
	{
		pool.small_m = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE);
		init_small(&(pool.small_m));
	}
	bws_small = pool.small_m;
	while (bws_small)
	{
		if ((ret = find_alloc(bws_small)))
			return (ret);
		keep = bws_small;
		bws_small = bws_small->next;
	}
	keep->next = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE);
	return (find_alloc(keep->next));
}

void	*ft_malloc(size_t size)
{
	static char			flag = 0;
//	static int			pg_size = 0;

	if (size <= 0)
		return (NULL);
	if (!flag)
	{
		pool.small_m = NULL;
		pool.medium_m = NULL;
		pool.big_m = NULL;
		flag = 1;
	}
	if (size <= TINY_M)
	{
		return (small_malloc(size));
	}
	else if (size <= SMALL_M)
	{
		return (medium_malloc(size));
	}
	else
	{
		return (big_malloc(size));
	}
}


int		main()
{
	int		ret;

	ret = getpagesize();
	printf("Ret = %d\n", ret);
	return (0);
}
