#include "../minishell.h"

void    free_str_null(char **str)
{
        if(!str || !(*str))
                return ;
        free(*str);
        *str = NULL;
}

int    ft_execve(t_data *data, t_cmd *cmd)
{
        int     pid;
        int     status;

        cmd->pipex->path = find_program_path(data->env, cmd->command);
        if(!cmd->pipex->path)
                return (g_last_exit_code);
        signal(SIGINT, handle_fork_sig);
        signal(SIGQUIT, handle_fork_sig);
        pid = fork();
        if(pid == 0)
        {
                if(execve(cmd->pipex->path, data->cmd->args, data->env_arr) == -1)
                        perror("execve");
                exit(127);
        }
        else
        {
                waitpid(pid, &status, 0);
                set_signals();
                if(WIFEXITED(status))
                                g_last_exit_code = WEXITSTATUS(status);
                return (g_last_exit_code);
        }
        return (0);
}
