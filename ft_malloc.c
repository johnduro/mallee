/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/20 15:48:08 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/20 21:14:03 by rabid-on         ###   ########.fr       */
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
	size_t			size;
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
		pool.large_m = (t_large *)mmap(0, size + sizeof(t_large), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		pool.large_m->next = NULL;
		pool.large_m->prev = NULL;
		pool.large_m->size = size;
		return ((void *)pool.large_m + sizeof(bws_large));
	}
	else
	{
		bws_large = pool.large_m;
		while (bws_large->next)
			bws_large = bws_large->next;
		bws_large->next = (t_large *)mmap(0, size + sizeof(t_large), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		bws_large->next->prev = bws_large;
		bws_large = bws_large->next;
		bws_large->next = NULL;
		bws_large->size = size;
		return ((void *)bws_large + sizeof(bws_large));
	}
}

void	*ft_malloc(size_t size)
{
	static char			flag = 0;

	if (size <= 0)
		return (NULL);
	if (!flag)
	{
		pool.tiny_m = NULL;
		pool.small_m = NULL;
		pool.large_m = NULL;
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

int		search_alloc(void *ptr, t_sm *mem, size_t m_range)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		if (mem->tab[i] != 0)
		{
			if (((void *)mem + ((i + 1) * m_range)) == ptr)
			{
				mem->tab[i] = 0;
				return (1);
			}
		}
		i++;
	}
	return (0);
}

int		is_tiny(void *ptr)
{
	t_sm	*bws_tiny;

	bws_tiny = pool.tiny_m;
	while (bws_tiny)
	{
		if (search_alloc(ptr, bws_tiny, TINY_M))
			return (1);
		bws_tiny = bws_tiny->next;
	}
	return (0);
}

int		is_small(void *ptr)
{
	t_sm	*bws_small;

	bws_small = pool.small_m;
	while (bws_small)
	{
		if (search_alloc(ptr, bws_small, SMALL_M))
			return (1);
		bws_small = bws_small->next;
	}
	return (0);
}

void	free_large(t_large *ptr)
{
	if (ptr->prev == NULL && ptr->next == NULL)
		pool.large_m = NULL;
	else if (ptr->prev == NULL && ptr->next)
		ptr->next->prev = NULL;
	else if (ptr->next == NULL && ptr->prev)
		ptr->prev->next = NULL;
	else if (ptr->prev && ptr->next)
	{
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
	}
	munmap(ptr, ptr->size);
}

int		is_large(void *ptr)
{
	t_large		*bws_large;

	bws_large = pool.large_m;
	while (bws_large)
	{
		if (((void *)bws_large + sizeof(bws_large)) == ptr)
		{
			free_large(bws_large);
			return (1);
		}
		bws_large = bws_large->next;
	}
	return (0);
}

void	ft_free(void *ptr)
{
	if (is_tiny(ptr))
		return ;
	else if (is_small(ptr))
		return ;
	else if (is_large(ptr))
		return ;
	return ;
}


int		main()
{
	int		i;
	char	*test;
	char	*test2;

	i = 0;
	test = (char *)ft_malloc(5000);
	test2 = (char *)ft_malloc(5000);
	while (i < 5000)
	{
		test[i] = 'a';
		test2[i] = 'b';
		i++;
	}
	printf("%s\n", test);
	printf("%s\n", test2);
	printf("free 1\n");
	ft_free(test);
	printf("free 2\n");
	ft_free(test2);
	test = (char *)ft_malloc(5000);
	test2 = (char *)ft_malloc(5000);
	i = 0;
	while (i < 5000)
	{
		test[i] = '1';
		test2[i] = '2';
		i++;
	}
	printf("%s\n", test);
	printf("%s\n", test2);
	return (0);
}
