/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ash_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arohani <arohani@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/16 14:24:51 by arohani           #+#    #+#             */
/*   Updated: 2018/03/21 14:55:19 by arohani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>

static void	launch_exec(t_shell *shell, char *full_path)
{
	if (execve(shell->args[0], shell->args, shell->envv) == -1)
	{
		if (execve(full_path, shell->args, shell->envv) == -1)
		{
			(!full_path) ? ft_putstr_fd(shell->args[0], 2) :
			ft_putstr_fd(full_path, 2);
			ft_putstr_fd(": Command not found.\n", 2);
			exit(EXIT_FAILURE);
		}
	}
}

static int	*redirect_check(t_shell *shell)
{
/* Basically, to perform > or >>, first close stdout, then open file to the right of > or >>, then execute command to the
	left of > or >>, then close the fd used for file on the right of > or >> */	
	int		i;
	int		fd = -1;
	int		*result = (int *)malloc(sizeof(int) * 2);

	i = 0;
	while (shell->args && shell->args[i])
	{
		// if '>' found, need to check if what comes after exists, open (so it gets fd) else create then open, then execute
		if (shell->args[i] && ((!(ft_strcmp(shell->args[i], ">")) || (!(ft_strcmp(shell->args[i], ">>"))))))
		{
			printf("redirections found\n");
			close(1);
			printf("shell->args[i] = %s\nshell->args[i][1] = %c\n", shell->args[i], shell->args[i][1]);
			fd = (shell->args[i][1] && shell->args[i][1] == '>') ? open(shell->args[i + 1], O_CREAT | O_APPEND | O_WRONLY, 0644)
			: open(shell->args[i + 1], O_CREAT | O_WRONLY, 0644);
			if (fd < 0)
			{
				printf("manage error opening file here\n");
				exit(EXIT_FAILURE);
			}
			shell->args[i] = 0;
			result[1] = i;
			result[0] = fd;
			return (result);
		}
		else
			printf("redirections not found. shell->args[i] = %s before incrementing\nstrcmp with >> = %d\n", shell->args[i], ft_strcmp(shell->args[i], ">>"));
		i++;
	}
	result[0] = 0;
	result[1] = 0;
	return (result);
}

static int	ash_launch(t_shell *shell)
{
	pid_t	pid;
	pid_t	wpid;
	int		status;
	char	*full_path;
	int		*fd_changed;

	full_path = NULL;
	status = 0;
	pid = fork();
	if (pid == 0)
	{
		fd_changed = redirect_check(shell);
		full_path = (has_paths(shell) == 1) ? arg_full_path(shell, 0) : NULL;
		launch_exec(shell, full_path);
		if (fd_changed[1])
		{
			close(fd_changed[0]);
//			open("/dev/fd/1", O_WRONLY);
		}
	}
	else if (pid < 0)
		ft_putstr_fd("error pid less than 0 in lsh launch", 2);
	else
	{
		wpid = waitpid(pid, &status, WUNTRACED);
		while (!WIFEXITED(status) && !WIFSIGNALED(status))
			wpid = waitpid(pid, &status, WUNTRACED);
	}
	if (full_path && full_path[0])
		ft_strdel(&full_path);
	return (1);
}

int			ash_execute(t_shell *shell)
{
	int		ret;

	ret = 0;
	if (shell->args[0] == NULL)
		return (1);
	if ((ret = builtin_check(shell)) != -1)
		return (ret);
	return (ash_launch(shell));
}
