#ifndef __help_h__
#define __help_h__

#include <stdio.h>
#include <string.h>

#define  PATH_SEPARATORS  "/"

char *cut_file_path(const char *path)
{
	char *last, *ptr;
	char *str = (char *) malloc(strlen(path));

	if (str == NULL) {
		fprintf(stderr, "cut_file_path(): str is null after malloc\n");
		return (char*)path;
	}
	strcpy(str, path);
	
	ptr = strtok(str, PATH_SEPARATORS);
	while (ptr != NULL) {
		last = ptr;
		ptr = strtok(NULL, PATH_SEPARATORS);
	}
	return last;
}

#endif
