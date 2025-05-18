#include "../minishell.h"

static int  is_len(char *str1, char *str2, char hint)
{
    int     len1;
    int     len2;

    len1 = ft_strlen(str1);
    len2 = 0;
    while(str2[len2] != hint)
        len2++;
    if(len1 > len2)
        return (len1);
    else
        return (len2);
}

int     appand_value(t_env *tmp, char *arg)
{
    char    *str;

    while(tmp)
    {
        if(ft_strncmp(tmp->name, arg, is_len(tmp->name, arg, '+')) == 0)
        {
            if(tmp->value)
            {
                str = tmp->value;
                tmp->value = ft_strjoin(str, (ft_strchr(arg, '=') + 1));
                free_str_null(&str);
            }
            else
                tmp->value = ft_strdup(ft_strchr(arg, '=') + 1);
            return (0);
        }
        tmp = tmp->next;         
    }
    return (-1);
}


void  export_var(t_env **lst, char *var)
{
    int     len;
    int     found;
    t_env   *tmp;

    found = 0;
    tmp = *lst;
    while(tmp)
    {
        if(ft_strncmp(tmp->name, var, is_len(tmp->name, var, '=')) == 0)
        {
            // printf("in found name %s var %s\n", tmp->name, var);
            len = ft_strlen(ft_strchr(var, '=') + 1);
            if(tmp->value)
                free_str_null(&tmp->value);
            tmp->value = malloc(sizeof(char) * len + 1);
            if(tmp->value)
                ft_strlcpy(tmp->value, ft_strchr(var, '=') + 1, len + 1);
            else
                malloc_error("t_env");
            found++;
            break;
        }
        tmp = tmp->next;
    }
    if(found == 0)
        ft_lstadd_back(lst, ft_lstnew(var));
}

void    swap_nodes(t_env *lst, t_env **env)
{
    t_env   *tmp;

    tmp = lst->next;
    lst->next = tmp->next;
    if (tmp->next)
        tmp->next->previous = lst;
    tmp->previous = lst->previous;
    if (lst->previous)
        lst->previous->next = tmp;
    else
        *env = tmp;
    tmp->next = lst;
    lst->previous = tmp;
}

static void    print_export(t_env *lst)
{
        while(lst != NULL)
        {
            if(lst->name && (ft_strcmp(lst->name, "_") != 0))
            {
                printf("declare -x ");
                printf("%s", lst->name);
                if(lst->value)
                {
                    printf("=\"");
                    printf("%s", lst->value);
                    printf("\"");
                }
                printf("\n");
            }
            lst = lst->next;
        }
}

void    sort_env(t_env **env)
{
    t_env   *lst;
    int len;

    len = ft_lstsize(*env);
    while(len)
    {
        lst = *env;
        while(lst->next != NULL)
        {
            if(lst->next && (ft_strcmp(lst->name, lst->next->name) > 0))
            {
                swap_nodes(lst, env);
            }
            else
                lst = lst->next;
        }
        len--;
    }
    print_export(*env);
    g_last_exit_code = 0;
}
