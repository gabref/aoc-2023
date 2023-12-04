#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

char	*get_next_line(int fd);
int		has_newline(int start, char *str);
int		find_newline(char *str);
int		ft_strlen(char *str);
char	*strjoin(char *s1, char *s2, int s1_len, int s2_len);

int	get_number(char *str, int i)
{
	int	res;

	while (i >= 0 && isdigit(str[i]))
		i--;
	i++;
	res = 0;
	while (str[i] && str[i] >= '0' && str[i] <= '9')
		res = (res * 10) + (str[i++] - 0x30);
	return (res);
}

void	process_sum(int fd)
{
	char	*lines[4096];
	char	*line;
	int		line_id;
	int		i;
	int		j;
	int		sum;
	int		m;
	int		n;
	int		c;
	int		na;
	int		tm;
	int		tn;
	int		first;
	int		second;

	// read file
	memset(lines, '\0', 4000);
	line_id = 0;
	line = get_next_line(fd);
	while (line != NULL)
	{
		lines[line_id++] = line;
		line = get_next_line(fd);
	}
	// analyze lines
	i = 0;
	sum = 0;
	while (lines[i])
	{
		line = lines[i];
		j = 0;
		while (line[j])
		{
			while (line[j] && line[j] != '*')
				j++;
			if (!line[j])
				break ;
			// check if valid gear
			// keeps track of numbers around
			na = 0;
			first = 0;
			second = 0;
			m = i > 0 ? -1 : 0;
			while (m < 2)
			{
				n = j > 0 ? -1 : 0;
				while (n < 2)
				{
					if (m == 0 && n == 0)
					{
						n++;
						continue ;
					}
					c = lines[i + m][j + n];
					if (isdigit(c))
					{
						if (na > 0 && m == tm && n == tn + 1)
						{
							tn = n;
							break ;
						}
						if (na == 0)
							first = get_number(lines[i + m], j + n);
						else
							second = get_number(lines[i + m], j + n);
						tm = m;
						tn = n;
						na++;
					}
					n++;
				}
				m++;
			}
			// is valid gear
			if (na == 2)
			{
				sum += first * second;
				printf("valid gears:\t%d\t%d\n", first, second);
				printf("current sum:\t%d\n", sum);
				printf("----------------\n");
			}
			j++;
		}
		i++;
	}
	printf("sum: %d\n", sum);
}

int	main(int ac, char **av)
{
	int	fd;

	if (ac != 2)
	{
		printf("usage: %s <input file>\n", av[0]);
		return (1);
	}
	fd = open(av[1], O_RDONLY);
	if (fd == -1)
	{
		printf("could not open file\n");
		return (1);
	}
	process_sum(fd);
	close(fd);
	return (0);
}

// get next line

int	has_newline(int start, char *str)
{
	if (!str)
		return (0);
	while (str[start])
		if (str[start++] == '\n')
			return (1);
	return (0);
}

int	find_newline(char *str)
{
	int	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		if (str[i++] == '\n')
			return (--i);
	return (0);
}

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	return (i);
}

char	*strjoin(char *s1, char *s2, int s1_len, int s2_len)
{
	char	*newstr;
	int		i;
	int		j;

	if (!s1)
	{
		s1 = malloc(sizeof(char));
		s1[0] = '\0';
	}
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	newstr = malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (newstr == NULL)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[j])
		newstr[i++] = s1[j++];
	while (*s2)
		newstr[i++] = *s2++;
	newstr[i] = '\0';
	free(s1);
	return (newstr);
}

int	read_fd_add_cache(int fd, char **cache)
{
	int		rbytes;
	char	*buf;
	int		last_cache_len;

	buf = (char *)malloc(sizeof(char) + (BUFFER_SIZE + 1));
	if (buf == NULL)
		return (0);
	rbytes = 1;
	last_cache_len = 0;
	while (rbytes > 0 && !has_newline(last_cache_len, *cache))
	{
		rbytes = read(fd, buf, BUFFER_SIZE);
		if (rbytes == -1)
			return (0);
		buf[rbytes] = '\0';
		last_cache_len = ft_strlen(*cache);
		*cache = strjoin(*cache, buf, last_cache_len, rbytes);
		if (*cache == NULL)
			return (0);
	}
	free(buf);
	return (1);
}

int	extract_line(char *cache, int cache_len, char **next_line)
{
	int	len;

	if (!cache)
		return (0);
	if (has_newline(0, cache))
		len = find_newline(cache) + 1;
	else
		len = cache_len;
	*next_line = malloc(sizeof(char) + (len + 1));
	if (*next_line == NULL)
		return (0);
	(*next_line)[len] = '\0';
	while (--len >= 0)
		(*next_line)[len] = cache[len];
	return (1);
}

int	clean_cache(char **cache, int cache_len)
{
	char	*temp;
	int		start;
	int		i;

	if (has_newline(0, *cache))
		start = find_newline(*cache) + 1;
	else
		start = cache_len;
	if ((cache_len - start + 1) == 0)
		return (1);
	temp = *cache;
	*cache = malloc(sizeof(char) * (cache_len - start + 1));
	if (*cache == NULL)
	{
		free(temp);
		return (0);
	}
	i = 0;
	while (temp[start])
		(*cache)[i++] = temp[start++];
	(*cache)[i] = '\0';
	free(temp);
	return (1);
}

char	*get_next_line(int fd)
{
	char		*next_line;
	static char	*cache;
	int			ok;
	int			cache_len;

	if (fd < 0 || BUFFER_SIZE < 1 || read(fd, &next_line, 0) < 0)
		return (NULL);
	ok = read_fd_add_cache(fd, &cache);
	if (!ok)
		return (NULL);
	cache_len = ft_strlen(cache);
	ok = extract_line(cache, cache_len, &next_line);
	if (!ok)
		return (NULL);
	ok = clean_cache(&cache, cache_len);
	if (!ok)
		return (NULL);
	if (*next_line == '\0')
	{
		free(next_line);
		return (NULL);
	}
	return (next_line);
}
