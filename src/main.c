#include <stdio.h>
#include <string.h>
#include "haystack.h"

#define 	DEFAULT_PARSE_ARRAY_SIZE 	3
#define 	DEFAULT_INPUT_STR_SIZE		255

const char *STR_COMMAND_EXIT  = "exit";
const char *STR_COMMAND_LIST  = "list";
const char *STR_COMMAND_ADD	  = "add";
const char *STR_COMMAND_GET	  = "get";
const char *STR_DIVISORS	  = " \n";
typedef enum { PAI_COMMAND, PAI_INFILENAME, PAI_OUTFILENAME } PAI;

void parse_args(char *str)
{
	char 	*parse_array[DEFAULT_PARSE_ARRAY_SIZE];
	char 	*ptr = strtok(str, STR_DIVISORS);
	size_t	parse_array_index = 0;

	while ( ptr != NULL ) {
		parse_array[parse_array_index++] = ptr;
		ptr = strtok(NULL, STR_DIVISORS);
	}

	if 		( strcmp(parse_array[PAI_COMMAND], STR_COMMAND_ADD ) == 0 ) {
		haystack_add_file(parse_array[PAI_INFILENAME]);
	}
	else if ( strcmp(parse_array[PAI_COMMAND], STR_COMMAND_GET ) == 0 ) {
		haystack_get_file(parse_array[PAI_INFILENAME], parse_array[PAI_OUTFILENAME]);
	}
	else if ( strcmp(parse_array[PAI_COMMAND], STR_COMMAND_LIST) == 0 ) {
		haystack_list_show();
	}
}

int main(int argc, char* argv[])
{
	char str[DEFAULT_INPUT_STR_SIZE];
	
	haystack_init();
	while ( 1 ) {
		fgets(str, DEFAULT_INPUT_STR_SIZE, stdin);
		if (strstr(str, STR_COMMAND_EXIT))
			break;
		parse_args(str);
	}
	haystack_close();

	return 0;
}
