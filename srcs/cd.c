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
#define ARG shell->args[shell->st]

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
		if (has_paths(shell, 1) == 2 && cd_path(shell))
			return (1);
		else
		{
			if (lstat(ARG, &buf) >= 0 && S_ISLNK(buf.st_mode))
			{
				ft_putstr_fd("cd: ", 2);
				ft_putstr_fd(ARG, 2);
				ft_putstr_fd(": Too many levels of symbolic links\n", 2);
			}
			else
			{
				ft_putstr_fd("cd: ", 2);
				ft_putstr_fd(ARG, 2);
				ft_putstr_fd(": No such file or directory\n", 2);
			}
		}
	}
	return (1);
}

static void	cd_canon(void)
{
	//successfully removes single dots and shortens remaining string, now need to process .. and treat symbolic links
	char	*clean = NULL;
	char	*tmp = NULL;
	int		i = 0;
	char	*test = "././././libft/../././libft/././..";
	char	**tab;

	tab = ft_strsplit(test, '/');
	printf("table after ft_strsplit is :\n");
	ft_print_table(tab);
	while (tab && tab[i])
	{
		while (ft_strcmp(tab[i], ".") == 0)
			i++;
		while (tab[i] && ft_strcmp(tab[i], "."))
		{
			if (!clean)
				clean = ft_strjoin(tab[i], "/");
			else if (clean && tab[i])
			{
				tmp = ft_strjoin(clean, tab[i]);
				ft_strdel(&clean);
				clean = ft_strjoin(tmp, "/");
				ft_strdel(&tmp);
			}
			i++;
		}
	}
	free_table(tab);
	printf("WAS: %s\nNOW: %s\n", test, clean);
	ft_strdel(&clean);
}

int			ash_cd(t_shell *shell)
{
	shell->st = opt_check(shell);
	cd_canon();
	printf("shell->st = %d\nshell->l = %d\nshell->p = %d\n", shell->st, shell->l, shell->p);
	if (shell->st == -1)
		return (1);
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
