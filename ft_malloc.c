/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/20 15:48:08 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/20 19:27:40 by mle-roy          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define TINY_M 512
#define SMALL_M 4096


typedef struct		s_large
{
	struct s_large	*next;
	struct s_large	*prev;
}					t_large;

typedef struct		s_sm
{
	int				tab[100];
	struct s_sm		*next;
}					t_sm;

typedef struct		s_malloc
{
	t_sm		*tiny_m;
	t_sm		*small_m;
	t_large		*large_m;
}					t_malloc;


	t_malloc			pool;


void	init_mem(t_sm *mem)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		mem->tab[i] = 0;
		i++;
	}
	mem->next = NULL;
}

void	*find_alloc(t_sm *mem, size_t size, int m_range)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		if (mem->tab[i] == 0)
		{
			mem->tab[i] = size;
			return ((void *)mem + ((i + 1) * m_range));
		}
		i++;
	}
	return (NULL);
}

void	*tiny_malloc(size_t size)
{
	void	*ret;
	t_sm	*bws_tiny;
	t_sm	*keep;

	if (!pool.tiny_m)
	{
		pool.tiny_m = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		init_mem(pool.tiny_m);
	}
	bws_tiny = pool.tiny_m;
	while (bws_tiny)
	{
		if ((ret = find_alloc(bws_tiny, size, TINY_M)))
			return (ret);
		keep = bws_tiny;
		bws_tiny = bws_tiny->next;
	}
	keep->next = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	init_mem(keep->next);
	return (find_alloc(keep->next, size, TINY_M));
}

void	*small_malloc(size_t size)
{
	void	*ret;
	t_sm	*bws_small;
	t_sm	*keep;

	if (!pool.small_m)
	{
		pool.small_m = (t_sm *)mmap(0, SMALL_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		init_mem(pool.small_m);
	}
	bws_small = pool.small_m;
	while (bws_small)
	{
		if ((ret = find_alloc(bws_small, size, SMALL_M)))
			return (ret);
		keep = bws_small;
		bws_small = bws_small->next;
	}
	keep->next = (t_sm *)mmap(0, SMALL_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	init_mem(keep->next);
	return (find_alloc(keep->next, size, SMALL_M));
}

void	*large_malloc(size_t size)
{
	t_large		*bws_large;

	if (!pool.large_m)
	{
		pool.large_m = (t_sm *)mmap(0, size + 16, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		pool.large_m->next = NULL;
		pool.large_m->prev = NULL;
		return ((void *)pool.large_m + 16);
	}
	else
	{
		bws_large = pool.large_m;
		while (bws_large->next)
			bws_large = bws_large->next;
		bws_large->next = (t_sm *)mmap(0, size + 16, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		bws_large->next->next = NULL;
		bws_large->next->prev = bws_large;
		return ((void *)bws_large->next + 16);
	}
}

void	*ft_malloc(size_t size)
{
	static char			flag = 0;

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
		return (tiny_malloc(size));
	else if (size <= SMALL_M)
		return (small_malloc(size));
	else
		return (large_malloc(size));
	return (NULL);
}


int		main()
{
	int		i;
	int		j;
	char	*test;

	i = 0;
	j = 0;
	while (j < 150)
	{
		test = (char *)ft_malloc(256);
		while (i < 256)
		{
			test[i] = 'a';
			i++;
		}
		printf("%s\n", test);
		j++;
		i = 0;
	}
	return (0);
}
