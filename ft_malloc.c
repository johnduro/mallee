
#include <unistd.h>
#include <stdio.h>


typedef struct		s_malloc
{
	int			size;
	char		free;
}					t_malloc;

void	*ft_malloc(size_t size)
{
	static char		flag_1 = 0;
	static int		pg_size = 0;

	if (size <= 0)
		return (NULL);
	if (!flag_1)
	{
		if ((pg_size = getpagesize()) <= 0)
			return (NULL);
		flag_1 = 1;
	}
	if (size <= (pg_size / 100))
	{
		small_malloc(size, pg_size);
	}
	else if (size > (pg_size / 100) && size <= pg_size)
	{
		medium_malloc(size, pg_size);
	}
	else
	{
		big_malloc(size, pg_size);
	}
}


int		main()
{
	int		ret;

	ret = getpagesize();
	printf("Ret = %d\n", ret);
	return (0);
}
