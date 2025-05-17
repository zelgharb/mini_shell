#include "../minishell.h"

bool	has_consecutive_ops(t_separation *token_node)
{
	t_separation	*prev;

	if (!token_node || !token_node->prev)
		return (false);

	prev = token_node->prev;
	if (token_node->type == PIPE && prev->type == PIPE)
		return (true);
	if ((token_node->type >= INPUT && token_node->type <= APPEND)
		&& (prev->type >= INPUT && prev->type <= APPEND))
	{
		if (!token_node->prev->next || token_node->prev->next->type != WORD)
			return (true);
		if (!token_node->next || token_node->next->type != WORD)
			return (true);
		return (false);
	}

	if ((token_node->type >= INPUT && token_node->type <= APPEND
			&& prev->type == PIPE)
		|| (token_node->type == PIPE
			&& prev->type >= INPUT && prev->type <= APPEND))
	{
		if (!token_node->next || token_node->next->type == PIPE
			|| token_node->next->type == END)
			return (true);
		if (!prev->prev || prev->prev->type == PIPE
			|| prev->prev->type >= INPUT)
			return (true);
		return (false);
	}

	return (false);
}

static int	check_errors_part_one(t_separation *temp)
{
	if (temp->type == PIPE && temp->prev
		&& temp->prev->type == INPUT)
	{
		if (temp->next == NULL || temp->next->type == END)
		{
			display_error_message(
				"bash: syntax error near unexpected token",
				"|", true);
			return (FAILURE);
		}
	}
	if ((temp->type >= INPUT && temp->type <= APPEND)
		|| temp->type == PIPE)
	{
		if (temp->next == NULL || temp->next->type == END)
		{
			display_error_message(
				"bash: syntax error near unexpected token",
				"newline", true);
			return (FAILURE);
		}
	}
	if (temp->type == PIPE && temp->next && temp->next->type == WORD)
	{
		const char *next_word = temp->next->str;
		if (next_word[0] == ')' || next_word[0] == '!' || next_word[0] == '&')
		{
			display_error_message("bash: syntax error near unexpected token",
				next_word, true);
			return (FAILURE);
		}
	}
	if (temp->type == WORD && temp->str)
	{
    	char c = temp->str[0];
    	if (c == '(' || c == ')' || c == '!' || c == ';' || c == '{' || c == '}')
    	{
        	display_error_message("bash: syntax error near unexpected token",
            	temp->str, true);
        	return (FAILURE);
    	}
	}
	if (temp->type == AMPER)
	{
		t_separation *curr = temp;
		int count = 0;
		while (curr && curr->type == AMPER)
		{
			count++;
			curr = curr->next;
		}
		if (count == 1)
		{
			if (temp->next == NULL || temp->next->type == END)
				display_error_message("bash: syntax error near unexpected token", "newline", true);
			else
				display_error_message("bash: syntax error near unexpected token", "&", true);
			return (FAILURE);
		}
		if (count >= 2)
		{
			display_error_message("bash: syntax error near unexpected token", "&", true);
			return (FAILURE);
		}
	}
	return (SUCCESS);
}

int	check_errors_rep(t_separation **token_lst)
{
	t_separation	*temp;

	temp = *token_lst;
	while (temp)
	{
		if (check_errors_part_one(temp) == FAILURE)
		{
			g_last_exit_code = 2;
			return (FAILURE);
		}
			
		if (has_consecutive_ops(temp))
		{
			display_error_message(
				"bash: syntax error near unexpected token",
				temp->str, true);
			g_last_exit_code = 2;
			return (FAILURE);
		}
		temp = temp->next;
	}
	return (SUCCESS);
}
