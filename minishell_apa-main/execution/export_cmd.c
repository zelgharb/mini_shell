#include "../minishell.h"

static int      export_exist(t_env *tmp, char *var)
{
    while(tmp)
    {
        if(ft_strcmp(tmp->name, var) == 0)
        {
            return (-1);
        }
        tmp = tmp->next;
    }
    return (0);
}

static void     add_to_value(t_env **env, t_env **export, char *arg)
{
        if(appand_value(*env, arg) == -1)
            ft_lstadd_back(env, new_node(arg));
        if(appand_value(*export, arg) == -1)
                    ft_lstadd_back(export, new_node(arg));
}

static void     export_fun(t_env **env, t_env **export, char *arg)
{
    t_env   *new;;

    if(ft_strchr(arg, '=') != NULL)
    {
        export_var(env, arg);
        export_var(export, arg);
    }
    else
    {
        if(export_exist(*export, arg) == 0)
        {
            new = malloc(sizeof(t_env));
            if(!new)
                malloc_error("t_env");
            else
            {
                new->name = ft_strdup(arg);
                new->value = NULL;
                new->next = NULL;
                new->previous = NULL;
                ft_lstadd_back(export, new);
            }
        }
    }
}

static int     valide_name(t_env **env, t_env **export, char *arg)
{
        int i;

        i = 0;
        while(arg[i] != '=' && arg[i] != '\0')
        {
            if(ft_isdigit(arg[0]) == 1)
                break ;
            while(ft_isalpha(arg[i]) == 1 || ft_isdigit(arg[i]) == 1 || arg[i] == '_')
                i++;
            if(arg[i] == '=' || arg[i] == '\0')
                return (0);
            else if(arg[i] == '+' && arg[i + 1] == '=')
            {
                g_last_exit_code = 0;
                return (add_to_value(env, export, arg), 1);
            }
            break ;
        }
        g_last_exit_code = 1;
        print_cmd_error("minishell: export", "not a valid identifier", arg);
        return (1);
}


int    export_cmd(t_data *data, t_env **envrmnt, t_env **export, char **args)
{
    int     j;

    j = 0;
    g_last_exit_code = 0;
    if(!args || !args[0])
        sort_env(export);
    else
    {
        if(args[0][0] == '-')
        {
            print_cmd_error("minishell: export", "invalid option", args[j]);
            g_last_exit_code = 2;
            return (g_last_exit_code);
        }
        while(args[j])
        {
            if(valide_name(envrmnt, export, args[j]) == 0)
            {
                export_fun(envrmnt, export, args[j]);
            }
            j++;           
        }
    }
    return (g_last_exit_code);
}
