#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1
#endif

char	*get_next_line(int fd);
int		has_newline(int start, char *str);
int		find_newline(char *str);
int		ft_strlen(char *str);
char	*strjoin(char *s1, char *s2, int s1_len, int s2_len);

int	is_word(char *str, char *word)
{
	while (*str && *word)
		if (*str++ != *word++)
			return (0);
	return (1);
}

int	get_number(char *str)
{
	int	number;

	number = 0;
	while (*str && *str >= '0' && *str <= '9')
		number = (number * 10) + (*str++ - 0x30);
	return (number);
}

int	get_game_sum(char *line, int rr, int rg, int rb)
{
	int	i;
	int	temp;

	int r, g, b;
	r = 0;
	g = 0;
	b = 0;
	while (*line)
		if (*line++ == ':')
			break ;
	i = 1;
	while (line[i])
	{
		if (line[i] >= '0' && line[i] <= '9')
			temp = get_number(&line[i]);
		else break;
		while (line[i++])
			if (line[i] == ' ')
			{
				i++;
				break ;
			}
		if (is_word(&line[i], "red"))
			if (r < temp)
				r = temp;
		if (is_word(&line[i], "green"))
			if (g < temp)
				g = temp;
		if (is_word(&line[i], "blue"))
			if (b < temp)
				b = temp;
		while (line[i])
			if (line[i++] == ' ')
				break ;
	}
	if (rr < r || rg < g || rb < b)
		return (0);
	return (1);
}

void	process_input(int r, int g, int b, int fd)
{
	char	*line;
	int		ln;
	int		ids_sum;

	ln = 1;
	ids_sum = 0;
	line = get_next_line(fd);
	while (line != NULL)
	{
		if (get_game_sum(line, r, g, b))
			ids_sum += ln;
		ln++;
		line = get_next_line(fd);
	}
	printf("Sum of ids of games: %d\n", ids_sum);
}

int	main(int ac, char **av)
{
	int	red;
	int	green;
	int	blue;
	int	fd;

	if (ac != 5)
	{
		printf("Usage: %s <red cubes> <green cubes> <blue cubes> <inputfile>\n",
			av[0]);
		return (-1);
	}
	red = atoi(av[1]);
	green = atoi(av[2]);
	blue = atoi(av[3]);
	fd = open(av[4], O_RDONLY);
	if (fd == -1)
	{
		printf("could not open file");
		return (-1);
	}
	process_input(red, green, blue, fd);
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
