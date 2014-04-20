/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/20 15:48:08 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/20 18:53:08 by rabid-on         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define TINY_M 512
#define SMALL_M 4096

typedef struct		s_big
{
//	int				addr;
	struct s_big	*next;
//	void			*pool;
}					t_big;

typedef struct		s_sm
{
//	int				addr;
	int				tab[100];
	struct s_sm		*next;
//	void			*pool;
}					t_sm;

typedef struct		s_malloc
{
	t_sm		*small_m;
	t_sm		*medium_m;
	t_big		*big_m;
}					t_malloc;


	t_malloc			pool;


void	init_small(t_sm *mem)
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

void	*find_alloc(t_sm *mem, size_t size)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		if (mem->tab[i] == 0)
		{
			mem->tab[i] = size;
			return ((void *)mem + ((i + 1) * TINY_M));
		}
		i++;
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
		pool.small_m = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		init_small(pool.small_m);
	}
	bws_small = pool.small_m;
	while (bws_small)
	{
		if ((ret = find_alloc(bws_small, size)))
			return (ret);
		keep = bws_small;
		bws_small = bws_small->next;
	}
	keep->next = (t_sm *)mmap(0, TINY_M * 101, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	return (find_alloc(keep->next, size));
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
/*	else if (size <= SMALL_M)
	{
		return (medium_malloc(size));
	}
	else
	{
		return (big_malloc(size));
	}*/
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
