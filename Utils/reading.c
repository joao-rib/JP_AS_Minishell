/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reading.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joao-rib <joao-rib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 19:30:33 by joao-rib          #+#    #+#             */
/*   Updated: 2024/11/30 16:19:10 by joao-rib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static bool	invalid_assignment(t_token *buff)
{
	while (buff && buff->token && buff->token[0])
	{
		if (!ft_strchr(buff->token, '='))
			break ;
		if (ft_isdigit(buff->token[0]) || buff->token[0] == '=')
		{
			printf("minishell: %s ", buff->token);
			error("is not a valid variable identifier\n", 127);
			return (true);
		}
		buff = buff->next;
	}
	return (false);
}

static bool	assign_var(t_minish *ms)
{
	t_token	*buff;
	bool	all_assign;

	buff = ms->tk_list;
	if (invalid_assignment(buff))
		return (true);
	all_assign = token_assign(buff);
	buff = ms->tk_list;
	while (buff && buff->token && buff->token[0])
	{
		if (all_assign)
		{
			add_or_update_env(&ms->env_tmp, buff->token);
			buff = buff->next;
		}
		else if (!all_assign && !ft_strchr(buff->token, '='))
			break ;
		else if (!all_assign && ft_strchr(buff->token, '=')
			&& (buff->type < SINGLE_QUOTES || buff->type > DOUBLE_QUOTES))
		{
			ms->tk_list = clear_top_token(buff);
			buff = ms->tk_list;
		}
	}
	return (all_assign);
}

static void	compute(t_minish *ms, char *input)
{
	if (!validate_quotes(input))
		return ;
	get_tokens(ms, input);
	if (!validate_tokens(ms, false))
		return ;
	expand(ms);
	ms->good_assign = assign_var(ms);
	if (!validate_tokens(ms, true))
		return ;
	parse(ms);
	if (!ms->good_assign)
		execute(ms);
	sanitize_path(ms);
	unlink_hd_file(ms);
}

static char	*maintain_prompt(char *cwd)
{
	char	*prompt;
	char	*suffix;

	prompt = ft_strdup("\033[1m""\033[34m""Curr.Directory:""\033[0m""\033[34m");
	if (!prompt)
		return (NULL);
	suffix = ft_strdup("\033[0m""\n""\033[4m""\033[97m" \
			"Input minishell:""\033[0m");
	if (!suffix)
	{
		free(prompt);
		return (NULL);
	}
	prompt = ft_strbuild(prompt, cwd);
	if (!prompt)
	{
		free(suffix);
		return (NULL);
	}
	prompt = ft_strbuild(prompt, suffix);
	free(suffix);
	return (prompt);
}

void	read_inputs(t_minish *ms)
{
	char	*prompt;
	char	*input;

	while (1)
	{
		prompt = maintain_prompt(ms->cwd);
		input = readline(prompt);
		free(prompt);
		if (!input)
		{
			error("exit\n", get_exit_status());
			sanitize_ms(ms, true);
			break ;
		}
		if (!input[0] || ft_isemptystr(input))
		{
			free_str_safe(input);
			continue ;
		}
		add_history(input);
		compute(ms, input);
		free(input);
		sanitize_ms(ms, false);
	}
	rl_clear_history();
}
