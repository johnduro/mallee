/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-roy <mle-roy@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/04/20 15:48:08 by mle-roy           #+#    #+#             */
/*   Updated: 2014/04/20 22:00:19 by rabid-on         ###   ########.fr       */
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

void	*ft_memcpy(void *s1, const void *s2, size_t n)
{
	size_t			i;
	unsigned char	*s_1;
	unsigned char	*s_2;

	i = 0;
	s_1 = (unsigned char *)s1;
	s_2 = (unsigned char *)s2;
	while (i < n)
	{
		s_1[i] = s_2[i];
		i++;
	}
	return (s1);
}

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
		return ((void *)pool.large_m + sizeof(t_large));
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
		return ((void *)bws_large + sizeof(t_large));
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
	{
		ptr->next->prev = NULL;
		pool.large_m = ptr->next;
	}
	else if (ptr->next == NULL && ptr->prev)
		ptr->prev->next = NULL;
	else if (ptr->prev && ptr->next)
	{
		ptr->prev->next = ptr->next;
		ptr->next->prev = ptr->prev;
	}
	munmap(ptr, ptr->size + sizeof(t_large));
}

int		is_large(void *ptr)
{
	t_large		*bws_large;

	bws_large = pool.large_m;
	while (bws_large)
	{
		if (((void *)bws_large + sizeof(t_large)) == ptr)
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

void	*realloc_ts(t_sm *mem, void *ptr, size_t size, size_t m_range)
{
	int		i;

	i = 0;
	while (i < 100)
	{
		if (mem->tab[i] != 0)
		{
			if (((void *)mem + ((i + 1) * m_range)) == ptr)
			{
				if (size <= TINY_M)
				{
					mem->tab[i] = size;
					return (ptr);
				}
				else
				{
					mem->tab[i] = 0;
					return (ft_memcpy(ft_malloc(size), ptr, size));
				}
			}
		}
		i++;
	}
	return (0);
}

void	*realloc_large(void *ptr, size_t size)
{
	void	*ret;
	t_large	*tmp;

	tmp = pool.large_m;
	while (tmp)
	{
		if ((void *)tmp + sizeof(t_large) == ptr)
		{
			ret = ft_malloc(size);
			ret = ft_memcpy(ret, ptr, size);
			ft_free(ptr);
			return (ret);
		}
		tmp = tmp->next;
	}
	return (NULL);
}

void	*search_realloc(void *ptr, size_t size)
{
	void	*ret;
	t_sm	*tmp;

	tmp = pool.tiny_m;
	while (tmp)
	{
		if ((ret = realloc_ts(pool.tiny_m, ptr, size, TINY_M)))
			return (ret);
		tmp = tmp->next;
	}
	tmp = pool.small_m;
	while (tmp)
	{
		if ((ret = realloc_ts(pool.small_m, ptr, size, SMALL_M)))
			return (ret);
		tmp = tmp->next;
	}
	if ((ret = realloc_large(ptr, size)))
		return (ret);
	return (NULL);
}

void	*ft_realloc(void *ptr, size_t size)
{
	if (size <= 0)
	{
		ft_free(ptr);
		return (NULL);
	}
	else if (!ptr)
		return (ft_malloc(size));
	else
		return (search_realloc(ptr, size));
	return (NULL);
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
	test = (char *)ft_realloc(test, 6500);
	test2 = (char *)ft_realloc(test2, 6500);
	i = 5000;
	while (i < 6500)
	{
		test[i] = '1';
		test2[i] = '2';
		i++;
	}
	printf("%s\n", test);
	printf("%s\n", test2);
	return (0);
}
