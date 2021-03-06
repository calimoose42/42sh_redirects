/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bin.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arohani <arohani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/26 12:01:43 by arohani           #+#    #+#             */
/*   Updated: 2018/02/21 14:56:11 by arohani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			builtin_check(t_shell *shell)
{
	if (shell && shell->args && shell->args[0])
	{
		if (ft_strcmp(shell->args[0], "echo") == 0)
			return (ash_echo(shell));
		else if (ft_strcmp(shell->args[0], "cd") == 0)
			return (ash_cd(shell));
		else if (ft_strcmp(shell->args[0], "setenv") == 0)
			return (ash_setenv(shell));
		else if (ft_strcmp(shell->args[0], "unsetenv") == 0)
			return (ash_unsetenv(shell));
		else if (ft_strcmp(shell->args[0], "env") == 0)
			return (ash_env(shell));
		else if (ft_strcmp(shell->args[0], "exit") == 0)
			return (ash_exit(shell));
		else
		{
			return (-1);
		}
	}
	return (-1);
}

int			if_opt_n(t_shell *shell)
{
	int		i;

	i = (shell->args && shell->args[1] && !ft_strcmp(shell->args[1], "-n"))
	? 2 : 1;
	if (shell->args[i] == NULL && i == 1)
		ft_putchar('\n');
	return (i);
}

int			ash_echo(t_shell *shell)
{
	int		i;

	i = if_opt_n(shell);
	if (shell->args && shell->args[i])
	{
		while (shell->args && shell->args[i])
		{
			if (i == 1 && shell->args[i][0] == '-')
				i++;
			if (!(shell->args[i + 1]))
			{
				(!(ft_strcmp(shell->args[1], "-n"))) ?
					ft_putstr(shell->args[i]) : ft_putendl(shell->args[i]);
				return (1);
			}
			ft_putstr(shell->args[i++]);
			(shell->args[i]) ? ft_putchar(' ') : ft_putchar('\n');
		}
		return (1);
	}
	return (1);
}

int			ash_exit(t_shell *shell)
{
	if (shell)
		return (0);
	return (0);
}
