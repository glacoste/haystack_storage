#ifndef __help_h__
#define __help_h__

#include <stdio.h>
#include <string.h>

#define  PATH_SEPARATORS  "/"

char *cut_file_path(const char *path)
{
	char *str, *last, *ptr;
	str = (char *) malloc(strlen(path));
	strcpy(str, path);
	
	ptr = strtok(str, PATH_SEPARATORS);
	while (ptr != NULL) {
		last = ptr;
		ptr = strtok(NULL, PATH_SEPARATORS);
	}
	return last;
}

#endif
