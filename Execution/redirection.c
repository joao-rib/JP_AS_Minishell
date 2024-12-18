/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alfreire <alfreire@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 01:56:02 by alfreire          #+#    #+#             */
/*   Updated: 2024/12/03 11:24:24 by alfreire         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

void	ctrld_eof(t_minish *ms, char *file)
{
	int	i;

	ft_putstr_fd("minishell: warning: heredoc delimited by eof\n", 1);
	close_all_pipes(ms);
	if (file)
		free(file);
	if (ms->pipes)
	{
		i = 0;
		while (i < cmdlst_size(ms->cmd_list, false) - 1)
		{
			if (ms->pipes[i])
				free(ms->pipes[i]);
			i++;
		}
		free(ms->pipes);
		ms->pipes = NULL;
	}
	return ;
}

static bool	report_error(char *filename)
{
	if (get_exit_status() != 130)
	{
		if (errno != 0)
			perror(filename);
		else
			error(filename, 2);
		set_exit_status(1);
	}
	return (true);
}

static int	exec_redir_in(t_minish *ms, char *filename)
{
	int	fd;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		report_error(filename);
		return (fd);
	}
	if (ms->fd_in != STDIN_FILENO)
		close(ms->fd_in);
	return (fd);
}

static int	exec_redir_out(t_minish *ms, const char *type, char *filename)
{
	int	fd;

	if (type[1] == '>')
		fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
	else
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd == -1)
		report_error(filename);
	if (ms->fd_out != STDOUT_FILENO)
		close(ms->fd_out);
	return (fd);
}

bool	execute_redir(const char *type, char *filename, t_minish *ms)
{
	int	fd;

	if (type[0] == '>')
		ms->fd_out = exec_redir_out(ms, type, filename);
	else if (ft_str_cmp(type, "<"))
	{
		fd = exec_redir_in(ms, filename);
		if (fd == -1)
			return (true);
		ms->fd_in = fd;
	}
	else if (ft_str_cmp(type, "<<"))
	{
		fd = do_heredoc(filename, ms);
		if (fd == -1)
		{
			ms->dont_execve = true;
			return (true);
		}
		if (ms->fd_in != STDIN_FILENO)
			close(ms->fd_in);
		ms->fd_in = fd;
	}
	return (false);
}
