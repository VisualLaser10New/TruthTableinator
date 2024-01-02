#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "boolean_parsers.h"

struct table
{
	char **table;
	size_t len;
};

struct table asktable(char *text)
{
	int len=0;
	printf("%s: ", text);
	scanf("%d%*c",&len);
	struct table table;
	table.len=len;
	table.table=(char**)malloc(sizeof(char*)*len);
	for(int i=0;i<len;i++)
	{
		table.table[i]=(char*)malloc(sizeof(char)*10);
		printf("Enter the %d° name: ",i+1);
		scanf("%s%*c", table.table[i]);
	}
	return table;
}

struct table askeq(char **output, size_t len)
{
	struct table res;
	res.table=(char**)malloc(sizeof(char*)*len);
	res.len=len;
	for(int i=0;i<len;i++)
	{
		res.table[i]=(char*)malloc(sizeof(char)*1000);
		printf("Enter the equation for %s: ",output[i]);
		scanf("%s%*c", res.table[i]);
	}
	return res;
}

char ** gen_res_table(size_t row, size_t col)
{
	char **res=(char**)malloc(sizeof(char*)*row);
	for(int i =0; i< row; i++)
	{
		res[i] = (char*)malloc(sizeof(char)*col);
	}
	return res;
}

void fill_input(struct table input, char*** res_table, size_t row)
{
	//foreach input column
	for(int i =0; i< input.len; i++)
	{
		int val = 1;
		//foreach row
		for(int j=0; j<row; j++)
		{
			//calc the value using modular math
			val = j%(lround(pow(2,input.len-i-1)))==0 ? ((val==1)?0:1):val;
			(*res_table)[j][i]= val;
		}
	}
}

void print_res_table(char** res_table,struct table input, struct table output, size_t row, size_t col)
{
	//HEADERS
	printf("|\t");
	for(int i = 0; i< input.len;i++)
	{
		printf("%s\t",input.table[i]);
		if(i == input.len-1)
			printf("║\t");
		else
			printf("|\t");
	}
	
	for(int i = 0; i< output.len;i++)
	{
		printf("%s\t|\t",output.table[i]);
	}
	printf("\n");
	//VALUES
	for(int i = 0; i< row; i++)
	{
		printf("|\t");
		for(int j = 0; j< col; j++)
		{
			printf("%d\t", res_table[i][j]);
			if(j == input.len-1)
				printf("║\t");
			else
				printf("|\t");
		}
		printf("\n");
	}
}

char *get_values(char **res_table, size_t row, size_t col, int row_index, size_t number_vars)
{
	char *res=(char*)malloc(sizeof(char)*number_vars);
	if(col < number_vars)
		return res;
	for(int i = 0; i< number_vars; i++)
	{
		res[i] = res_table[row_index][i];
	}
	return res;
}

void resolve_output(struct table output, struct table input, struct table equations, char*** res_table, size_t row, size_t col)
{
	for(int i =0; i< row; i++)
	{
		for(int j=input.len; j<col; j++)
		{
			int pos = j-input.len;
			(*res_table)[i][j]= bool_evaluate(equations.table[pos], input.table, input.len, get_values((*res_table), row, col, i, input.len));
		}
	}
}

int main(void) 
{
	struct table input=asktable("How many input do you want to enter?");
	struct table output=asktable("How many output do you want to enter?");

	struct table eq=askeq(output.table, output.len);
	size_t m = input.len + output.len;
	size_t n = pow(2, input.len);
	
	char **res = gen_res_table(n,m);
	fill_input(input, &res, n);
	resolve_output(output, input, eq, &res, n,m);
	print_res_table(res,input,output, n, m);
	
	return 0;
	/*
	foreach element in input
	{	
		M=len(input)+len(output)
		N=2**len(input)
		generate the table as matrix of 8bit int N*M
		compile the firsts len(input)s rows of table with binary values
		resolve the equations for each rows, foreach output cols and compile the output of table
		print the table with header too
	}
		
	*/
}