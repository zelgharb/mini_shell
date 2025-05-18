#include "../minishell.h"

static void	toggle_quote_status(t_separation **token, char c)
{
	if (c == '\'' && (*token)->status == DFLT)
		(*token)->status = SQUOTE;
	else if (c == '\'' && (*token)->status == SQUOTE)
		(*token)->status = DFLT;
	else if (c == '"' && (*token)->status == DFLT)
		(*token)->status = DQUOTE;
	else if (c == '"' && (*token)->status == DQUOTE)
		(*token)->status = DFLT;
}

static bool	is_var_separator(char c)
{
	return (c == '$' || c == ' ' || c == '=' || c == '\0');
}

static bool	is_var_quoted(char *str, int i)
{
	if (i > 0)
	{
		if (str[i - 1] == '\"' && str[i + 1] == '\"')
			return (true);
		else
			return (false);
	}
	return (false);
}

static int	count_preceding_dollars(char *str, int pos)
{
	int	count = 0;

	while (pos - 1 >= 0 && str[pos - 1] == '$')
	{
		count++;
		pos--;
	}
	return (count);
}

static void	process_variable_token(t_data *data, t_separation **curr)
{
	int	i;

	i = 0;
	while ((*curr)->str[i])
	{
		toggle_quote_status(curr, (*curr)->str[i]);
		if ((*curr)->str[i] == '$'
			&& !is_var_separator((*curr)->str[i + 1])
			&& !is_var_quoted((*curr)->str, i)
			&& ((*curr)->status == DFLT || (*curr)->status == DQUOTE))
		{
			if (i == 0 || (*curr)->str[i-1] != '\\')
			{
				int	dollar_count = count_preceding_dollars((*curr)->str, i);
				if (dollar_count % 2 == 0)
				{
					replace_variable(curr,
						get_value(*curr, (*curr)->str + i, data->env), i);
					continue;
				}
			}
		}
		i++;
	}
}

int expand_variables(t_data *data, t_separation **tokens)
{
    t_separation *curr = *tokens;
    while (curr)
    {
        if (curr->type == VAR)
            process_variable_token(data, &curr);
        curr = curr->next;
    }
    remove_empty_variables(tokens);
    return (0);
}

char	*expand_heredoc_variables(t_data *data, char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$'
			&& !is_var_separator(str[i + 1])
			&& !is_var_quoted(str, i))
			{
				if(i == 0 || str[i-1] != '\\')
				{
					int	dollar_count = count_preceding_dollars(str, i);
					if (dollar_count % 2 == 0) // impair si count est pair
					{
						str = replace_heredoc_var(str,
									get_value(NULL, str + i, data->env), i);
					}
				}
			}
			i++;
	}
	return (str);
}
