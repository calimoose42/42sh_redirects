/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arohani <arohani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/25 11:05:03 by arohani           #+#    #+#             */
/*   Updated: 2018/02/20 14:51:44 by arohani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#define ARG shell->args[1]

static int	cd_no_arg(t_shell *shell)
{
	t_env	*tmp;

	tmp = (shell->list->var) ? shell->list : NULL;
	while (tmp && !(ARG))
	{
		if (ft_strcmp(tmp->var, "HOME") == 0)
		{
			if (tmp->val)
			{
				update_old_pwd(shell, tmp->val);
				chdir(tmp->val);
				return (1);
			}
		}
		tmp = tmp->next;
	}
	ft_putstr_fd("cd: No home directory.\n", 2);
	return (0);
}

static int	cd_dash(t_shell *shell)
{
	t_env		*tmp;
	struct stat	buf;

	tmp = (shell->list) ? shell->list : NULL;
	while (tmp)
	{
		if (ft_strcmp(tmp->var, "OLDPWD") == 0)
		{
			if (lstat(tmp->val, &buf) >= 0)
			{
				if (chdir(tmp->val) != 0)
				{
					ft_putstr_fd("cd: not a directory: ", 2);
					ft_putendl_fd(tmp->val, 2);
				}
				else
					ft_putendl(tmp->val);
				update_old_pwd(shell, tmp->val);
				return (1);
			}
		}
		tmp = tmp->next;
	}
	ft_putstr_fd("cd: OLDPWD not set\n", 2);
	return (1);
}

static int	cd_relative(t_shell *shell)
{
	t_env	*tmp;
	char	cwd[256];

	tmp = (shell->list) ? shell->list : NULL;
	if (chdir(ARG) != 0)
		ft_putstr_fd("error changing directory from cd_relative\n", 2);
	else
	{
		if (getcwd(cwd, sizeof(cwd)) == NULL)
			ft_putstr_fd("error retrieving cwd\n", 2);
		else
			update_old_pwd(shell, cwd);
	}
	return (1);
}

int			regular_cd(t_shell *shell)
{
	struct stat	buf;

	if (lstat(ARG, &buf) >= 0)
	{
		if ((S_ISDIR(buf.st_mode)))
			update_old_pwd(shell, ARG);
		else if (!(S_ISLNK(buf.st_mode)))
		{
			ft_putstr_fd("cd: ", 2);
			ft_putstr_fd(ARG, 2);
			ft_putstr_fd(": Not a directory\n", 2);
			return (1);
		}
	}
	if (chdir(ARG) != 0)
	{
	//	printf("first tried changing directory without concat of cdpath\nhas_paths(shell) = %d\n", has_paths(shell, 1));
		//include clause to check for CDPATH and concat, similar to execution of binaries searched via PATH
		if (has_paths(shell, 1) == 2 && cd_path(shell) != NULL)
			return (1);
		else
		{
			ft_putstr_fd("cd: ", 2);
			ft_putstr_fd(ARG, 2);
			ft_putstr_fd(": No such file or directory\n", 2);
		}
	}
	return (1);
}

int			ash_cd(t_shell *shell)
{
	if (ARG == NULL)
		cd_no_arg(shell);
	else if (shell->args && (ft_strcmp(ARG, "-") == 0))
		cd_dash(shell);
	else if (shell->args && ARG[0] == '.')
		cd_relative(shell);
	else
		regular_cd(shell);
	return (1);
}
