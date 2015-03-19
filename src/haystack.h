#ifndef __haystack_h__
#define __haystack_h__

#include <stdlib.h>
#include <sys/types.h>

#define  DEFAULT_LIST_SIZE 	10
#define  DEFAULT_LIST_INCR 	1.5
#define	 BLOB_FILENAME		"data.blob"
#define	 BLIN_FILENAME  	"data.blin"

typedef struct FileInfo FileInfo;
struct FileInfo {
	char	*_fname;
	off_t	_start;
	size_t	_size;
};

int		 blob_fd;
int		 blin_fd;

size_t	 finfo_list_cap;
size_t	 finfo_list_size;
FileInfo *finfo_list;

void haystack_init(void);
void haystack_close(void);
void haystack_list_resize(size_t new_size);
void haystack_list_show(void);
int  haystack_add_file(const char *new_fname);
int  haystack_get_file(char *fname, const char *out_fname);
FileInfo* haystack_get_finfo(char *fname);

#endif
