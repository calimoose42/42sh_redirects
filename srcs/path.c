/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arohani <arohani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/25 11:50:10 by arohani           #+#    #+#             */
/*   Updated: 2018/02/21 13:14:35 by arohani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**fetch_paths(t_shell *shell, int m)
{
	t_env	*tmp;
	char	**path;
	int		search_env;

	search_env = 0;
	tmp = (m) ? shell->list->mod : shell->list;
	while (tmp)
	{
		if (ft_strcmp(tmp->var, "PATH") == 0)
		{
			path = ft_strsplit(tmp->val, ':');
			return (path);
		}
		tmp = tmp->next;
		if (shell->list && m && !tmp && search_env == 0)
		{
			tmp = shell->list;
			search_env++;
		}
	}
	return (NULL);
}

char		*arg_full_path(t_shell *shell, int m)
{
	char		*str;
	char		*full_path;
	int			ret;
	int			i;
	char		**paths;

	i = 0;
	paths = fetch_paths(shell, m);
	while (paths && paths[i])
	{
		str = (paths[i][ft_strlen(paths[i]) - 1] != '/')
		? ft_strjoin(paths[i++], "/") : ft_strdup(paths[i++]);
		full_path = ft_strjoin(str, shell->args[0]);
		if (str && str[0])
			ft_strdel(&str);
		if (!(ret = access(full_path, F_OK)))
		{
			free_table(paths);
			return (full_path);
		}
		if (full_path && full_path[0])
			ft_strdel(&full_path);
	}
	free_table(paths);
	return (NULL);
}

int			has_paths(t_shell *shell)
{
	t_env		*tmp;

	if (!(shell->list) || (shell->list && !(shell->list->var)))
		return (0);
	tmp = shell->list;
	while (tmp)
	{
		if (ft_strcmp(tmp->var, "PATH") == 0)
			return (1);
		tmp = tmp->next;
	}
	return (0);
}
