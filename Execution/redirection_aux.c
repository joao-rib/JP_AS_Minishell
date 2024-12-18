/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_aux.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alfreire <alfreire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 01:56:02 by alfreire          #+#    #+#             */
/*   Updated: 2024/12/03 11:14:02 by alfreire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*find_key(char *str)
{
	char	*tmp;
	int		len;

	len = 0;
	tmp = ft_strnstr(str, "$", ft_strlen(str));
	if (tmp[1] == '?')
		return (ft_strdup("$?"));
	while (ft_isalnum(tmp[len + 1]) || tmp[len + 1] == '_')
		len++;
	return (ft_substr(tmp, 0, len + 1));
}

char	*replace_substring(char *str, char *old, char *new)
{
	char	*ret;
	char	*tmp;
	char	*ptr;
	int		len;

	if (!str || !old || !new)
		return (NULL);
	len = ft_strlen(str) + ft_strlen(new) - ft_strlen(old);
	ret = ft_calloc(len + 1, sizeof(char));
	if (!ret)
		return (NULL);
	ptr = ft_strnstr(str, old, ft_strlen(str));
	if (!ptr)
		return (NULL);
	tmp = ft_substr(str, 0, ptr - str);
	ft_strlcat(ret, tmp, len + 1);
	ft_strlcat(ret, new, len + 1);
	ft_strlcat(ret, ptr + ft_strlen(old), len + 1);
	free(tmp);
	return (ret);
}

char	*expand_heredoc(char *line, char **env_list, bool fl)
{
	char	*result;
	char	*value;
	char	*key;
	char	*tmp;

	if (fl)
		return (line);
	result = ft_strdup(line);
	while (ft_strnstr(result, "$", ft_strlen(result)))
	{
		key = find_key(result);
		if (ft_str_cmp(key, "$?"))
			value = ft_itoa(get_exit_status());
		else
			value = get_env(key, env_list);
		tmp = result;
		result = replace_substring(result, key, value);
		free(tmp);
		free(value);
		free(key);
	}
	free(line);
	return (result);
}

void	write_file(char *here_doc_file, char *buffer)
{
	int	fd;

	fd = open(here_doc_file, O_WRONLY | O_APPEND);
	if (fd == -1)
		return ;
	if (buffer)
		write(fd, buffer, ft_strlen(buffer));
	write(fd, "\n", 1);
	close(fd);
}

void	read_until_deli(char *deli, t_minish *ms, char *file, bool fl)
{
	char	*line;

	while (1)
	{
		line = readline("heredoc > ");
		if (!line)
		{
			ctrld_eof(ms, file);
			sanitize_ms(ms, true);
			break ;
		}
		if (ft_str_cmp(line, deli))
		{
			free(line);
			free(file);
			set_exit_status(0);
			sanitize_ms(ms, true);
			break ;
		}
		line = expand_heredoc(line, ms->env_list, fl);
		write_file(file, line);
		free(line);
	}
}
