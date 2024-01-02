#include "boolean_parsers.h"

char bool_assert(char *expression)
{
	char current_res =0;
	char current_op= OP_OR;
	bool not=false;
	for(int i = 0; i < strlen(expression); i++)
	{
		if(expression[i] == '(')
		{
			char *sub = extract_sub_expr(expression, i);
			char tmp = bool_assert(sub);
			current_res = exec_atomic_expr(current_res, current_op, tmp, false, not);
			not = false;
			i += strlen(sub)+1;
		}
		else if(expression[i] == NOT_SYMBOL)
		{
			not = true;
		}
		else if(expression[i] == AND_SYMBOL)
		{
			current_op = OP_AND;
		}
		else if(expression[i] == OR_SYMBOL)
		{
			current_op = OP_OR;
		}
		else if(expression[i] == ')')
		{
			return current_res;
		}
		else if(expression[i] == ZERO_SYMBOL || expression[i] == ONE_SYMBOL)
		{
			current_res = exec_atomic_expr(current_res, current_op, expression[i]-48, false, not);
			not=false;
		}
	}
	return current_res;
}

char *extract_sub_expr(char *expression, int start)
{
	/*
	to exctract the sub-expression: count how many open-parenthesis ahead of the current one, and get the string up to the closing parenthesis coresponding to the current one, by counting the number of closed-parenthesis
	*/
	int n_open_parenthesis = 0;
	int n_closed_parenthes = 0;
	char *sub_expression = malloc(sizeof(char) * (strlen(expression)));
	for(int i = start; i < strlen(expression); i++)
	{
		if(expression[i] == '(')
			n_open_parenthesis++;
		else if(expression[i] == ')')
			n_closed_parenthes++;
		if(n_open_parenthesis == n_closed_parenthes && n_open_parenthesis!=0)
		{
			strncpy(sub_expression, expression+start+1, i-start-1);
			return sub_expression;
		}
		//+1 and -1 to avoid the parenthesis
	}
	return sub_expression;
}

char *extract_sub_expr_rev(char *expression, int end)
{
	/*
	to exctract the sub-expression: count how many open-parenthesis ahead of the current one, and get the string up to the closing parenthesis coresponding to the current one, by counting the number of closed-parenthesis
	*/
	int n_open_parenthesis = 0;
	int n_closed_parenthes = 0;
	char *sub_expression = malloc(sizeof(char) * (strlen(expression)));
	for(int i = end; i >= 0; i--)
	{
		if(expression[i] == '(')
			n_open_parenthesis++;
		else if(expression[i] == ')')
			n_closed_parenthes++;
		if(n_open_parenthesis == n_closed_parenthes && n_closed_parenthes != 0)
		{
			strncpy(sub_expression, expression+i+1, end-1);
			return sub_expression;
		}
		//+1 and -1 to avoid the parenthesis
	}
	return sub_expression;
}

void strrepl(char *str, size_t var_len, char newest)
{
	/*remove the current string from *str to *str+len and replace it with the newest*/
	//newset is a number stored in a char, so must sum 48 to get the ascii value
	newest+= 48;
	str[0] = newest;
	int i = 1;
	memmove(str+i, str+i+var_len-1,strlen(str)-i);
	while(str[i]!= '\0')
	{
		//*(str+i) = *(str+i+var_len);
		i++;
	}
	*(str+i) = '\0';
}

void replace_var_with_value(char *expression, char **var_names, size_t var_count, char *values)
{
	for(int i = 0; i < var_count; i++)
	{
		char * pch;
		pch = strstr(expression, var_names[i]);
		while(pch!=NULL)
		{
			strrepl(pch, strlen(var_names[i]), values[i]);
			pch = strstr(pch+1, var_names[i]);
		}
	}
}

char exec_atomic_expr(char val1, enum op ops, char val2, bool not_val1, bool not_val2)
{
	//TODO: check if it not the first val
	val1= not_val1 ? (val1 ? 0:1) : val1;
	val2= not_val2 ? (val2 ? 0:1) : val2;
	
	switch(ops)
	{
		case OP_AND:
			return val1 & val2;
		case OP_OR:
			return val1 | val2;
	}
}


int find_op(char* expression, int start_pos, bool rev)
{
	/*
	if rev: find pos of the begin of previous operand
	if !rev: find pos of the end of next operand

	searching by starting from start_pos
	*/
	
	int coef = (rev) ? -1: 1;
	char par = (rev)? ')': '(';
	int res_pos = 0;
	bool not=false;
	
	if(expression[start_pos+coef]== NOT_SYMBOL)
	{
		not=true;
	}
	
	if(expression[start_pos+coef+not] == par)
	{
		size_t len = (rev)? strlen(extract_sub_expr_rev(expression, start_pos+coef)) : strlen(extract_sub_expr(expression, start_pos+coef));
		res_pos = start_pos + coef*len + coef+not;
	}
	else if(expression[start_pos+coef+not] == ZERO_SYMBOL || expression[start_pos+coef+not] == ONE_SYMBOL)
	{
		res_pos= start_pos+coef+not;
	}
	
	if(expression[res_pos+coef-1] == NOT_SYMBOL)
	{
		return res_pos + coef;
	}
	else
	{
		return res_pos;
	}
}

void strins(char *str, int pos, char insert_char) {
    memmove(str + pos + 1, str + pos, strlen(str) - pos + 1); 
    str[pos] = insert_char;
}

void insert_parenthesis(char *expression)
{
	//insert parenthesis to all AND operations to respect the order of operations
	
	char * pch;
	pch = strchr(expression, AND_SYMBOL);
	while(pch!=NULL)
	{
		int pos = pch - expression;
		int pre_op_pos = find_op(expression, pos, true);
		int post_op_pos = find_op(expression, pos, false);
		strins(expression, pre_op_pos, '(');
		strins(expression, post_op_pos+2, ')');
		
		pch = strchr(pch+2, AND_SYMBOL); //plus 2 because it inserts the parenthesis
	}
}

char bool_evaluate(char *expression_p, char** var_names, size_t var_count, char* var_values)
{
	char *expression = (char*)malloc(sizeof(char) *(strlen(expression_p)+1));
	strcpy(expression, expression_p);
	
	//replace all variable names in expression whit theire correspettive values
	replace_var_with_value(expression, var_names, var_count, var_values);
	//insert parenthesis to all AND operations to respect the order of operations
	insert_parenthesis(expression);
	//printf("par: %s\n", expression);
	//evaluate the expression
	return bool_assert(expression);
}