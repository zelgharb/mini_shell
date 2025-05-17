#include "../minishell.h"

void    free_str_null(char **str)
{
        if(!str || !(*str))
                return ;
        free(*str);
        *str = NULL;
}

char    *valid_path(char *str, char *cmd)
{
        char    *tmp;
        char    *path;

        tmp = NULL;
        path = NULL;
        tmp = ft_strjoin(str, "/");
        path = ft_strjoin(tmp, cmd);
        if(tmp)
                free_str_null(&tmp);
        if(access(path, X_OK) == 0)
                return (path);
        if(path)
                free_str_null(&path);
        return (NULL);
}

char     *relative_path(t_env *env, char *cmd)
{
        char    **arr;
        char    *path;
        int     i;
        t_env   *tmp_env;

        arr = NULL;
        tmp_env = env;
        path = NULL;
        while(tmp_env)
        {
                if(ft_strcmp(tmp_env->name, "PATH") == 0)
                {
                        arr = ft_split(tmp_env->value, ':');
                }
                tmp_env = tmp_env->next;
        }
        i = 0;

        while(arr && arr[i])
        {
                path = valid_path(arr[i], cmd);
                if(path != NULL)
                        break ;
                i++;
        }
        if(arr)
                free_string_array(arr);
        if(!path)
        {
                print_cmd_error(cmd, "command not found", NULL);
                g_last_exit_code = 127;
        }
        return (path);
}

static bool    is_directory(char *cmd)
{
        struct stat statbuf;

        if (stat(cmd, &statbuf) == -1)
            return false;
        return S_ISDIR(statbuf.st_mode);
}

char     *find_program_path(t_env *env, char *cmd)
{
        int     i;

        i = 0;
        if(cmd == NULL || cmd[0] == '\0')
        {
                g_last_exit_code = 127;
                return (print_cmd_error("minishell", "command not found", ""), NULL);
        }
        while(cmd[i])
        {
                if(cmd[i] != '\0' && cmd[i] == '/')
                {
                        g_last_exit_code = 127;
                        if (access(cmd, F_OK) != 0)
                                return (print_cmd_error("minishell", "No such file or directory", cmd), NULL);
                        if (is_directory(cmd))
                        {
                                g_last_exit_code = 126;
                                return (print_cmd_error("minishell", "Is a directory", cmd), NULL);
                        }
                        if (access(cmd, X_OK) != 0)
                        {
                                g_last_exit_code = 126;
                                return (print_cmd_error("minishell", "Permission denied", cmd), NULL);
                        }
                        return (ft_strdup(cmd));
                }
                i++;
        }
        return (relative_path(env, cmd));
}

int    ft_execve(t_data *data, t_cmd *cmd)
{
        int     pid;
        int     status;

        cmd->pipex->path = find_program_path(data->env, cmd->command);
        if(!cmd->pipex->path) return (g_last_exit_code);
        data->env_arr = env_to_array(data->env);
        if(!data->env_arr)
                return (malloc_error("t_data env_array"));
        signal(SIGINT, handle_fork_sig);
        signal(SIGQUIT, handle_fork_sig);
        pid = fork();
        if(pid == 0)
        {
                // signal(SIGINT, SIG_DFL);
                // signal(SIGQUIT, SIG_DFL);
                if(execve(cmd->pipex->path, data->cmd->args, data->env_arr) == -1) perror("execve");
                exit(127);
        }
        else
        {
                waitpid(pid, &status, 0);
                set_signals();
                // if (WIFSIGNALED(status))
                // {
                //         if (WTERMSIG(status) == SIGINT)
                //                 write(1, "\n", 1);
                //         else if (WTERMSIG(status) == SIGQUIT)
                //                 write(1, "Quit (core dumped)\n", 19);
                //         return 128 + WTERMSIG(status);
                // }
                if(WIFEXITED(status))
                                g_last_exit_code = WEXITSTATUS(status);
                return (g_last_exit_code);
        }
        return (0);
}
