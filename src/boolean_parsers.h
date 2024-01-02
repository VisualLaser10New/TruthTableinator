#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#define AND_SYMBOL '*'
#define OR_SYMBOL '+'
#define NOT_SYMBOL '!'
#define ZERO_SYMBOL '0'
#define ONE_SYMBOL '1'

enum op
{
	OP_AND,
	OP_OR,
	OP_NOT
};

char bool_assert(char *expression);
char *extract_sub_expr(char *expression, int start);
char *extract_sub_expr_rev(char *expression, int end);
void strrepl(char *str, size_t var_len, char newest);
void replace_var_with_value(char *expression, char **var_names, size_t var_count, char *values);
char exec_atomic_expr(char val1, enum op ops, char val2, bool not_val1, bool not_val2);
int find_op(char* expression, int start_pos, bool rev);
void strins(char *str, int pos, char insert_char) ;
void insert_parenthesis(char *expression);
char bool_evaluate(char *expression, char** var_names, size_t var_count, char* var_values);