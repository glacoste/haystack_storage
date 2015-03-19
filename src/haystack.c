#include <fcntl.h>
#include <unistd.h>
#include "haystack.h"
#include "help.h"

#define  DEFAULT_BUFFER_SIZE  4096
#define  DEFAULT_PERMISSIONS  0666
typedef  unsigned char fnlen;

void haystack_init(void)
{
	/* Opening blob file */
	blob_fd = open(BLOB_FILENAME, O_RDWR, 0);
	if (blob_fd == -1) {
		blob_fd = creat(BLOB_FILENAME, DEFAULT_PERMISSIONS);
		if (blob_fd == -1) {
			fprintf(stderr, "can not create blob file\n");
			exit(1);
		}
		blob_fd = open(BLOB_FILENAME, O_RDWR, 0);
	}
	
	/* Opening blin file */
	blin_fd = open(BLIN_FILENAME, O_RDWR, 0);
	if (blin_fd == -1) {
		blin_fd = creat(BLIN_FILENAME, DEFAULT_PERMISSIONS);
		if (blin_fd == -1) {
			fprintf(stderr, "can not create blin file\n");
			exit(1);
		}
		write(blin_fd, 0L, sizeof(finfo_list_size));
		lseek(blin_fd, sizeof(finfo_list_size), SEEK_SET);
	}
	else {
		read(blin_fd, &finfo_list_size, sizeof(finfo_list_size));
		if ( finfo_list_size > 0 ) {
			int i;
			haystack_list_resize(finfo_list_size*DEFAULT_LIST_INCR);
			
			for (i = 0; i < finfo_list_size; ++i) {
				fnlen fname_len = 0;
				read(blin_fd, &fname_len, sizeof(fname_len));

				finfo_list[i]._fname = (char*) malloc(fname_len+1);
				read( blin_fd,  finfo_list[i]._fname, fname_len		  	  	 	);
				read( blin_fd, &finfo_list[i]._start, sizeof(finfo_list[i]._start)  );
				read( blin_fd, &finfo_list[i]._size,  sizeof(finfo_list[i]._size)   );
				finfo_list[i]._fname[fname_len] = '\0';
			}
		}
	}
}

void haystack_close(void)
{
	lseek(blin_fd, 0L, SEEK_SET);
	write(blin_fd, &finfo_list_size, sizeof(size_t));
	close(blin_fd);
	close(blob_fd);
}

void haystack_list_resize(size_t new_size)
{
	finfo_list = realloc(finfo_list, sizeof(FileInfo)*new_size);
	finfo_list_cap = new_size;
}

void haystack_list_show(void)
{
	size_t i;
	for (i = 0; i < finfo_list_size; ++i)
		printf("%-3.lu %-25.25s \t%10.lu \t%10.lu bytes\n", i+1, 
				finfo_list[i]._fname, 
				finfo_list[i]._start, 
				finfo_list[i]._size);
}

int haystack_add_file(const char *new_fname) 
{
	ssize_t 	n_read;
	fnlen		fname_len;
	off_t		fsize, fstart, written;
	char    	buffer[DEFAULT_BUFFER_SIZE], *fname;
	FileInfo 	*finfo;
	
	int fdesc = open(new_fname, O_RDONLY, 0);
	if (fdesc == -1) {
		fprintf(stderr, "haystack_add_file(): no such file\n");
		return 0;
	}
	
	fsize = lseek(fdesc, 0L, SEEK_END);
	if (fsize <= 0) {
		fprintf(stderr, "haystack_add_file(): new file is empty\n");
		return 0;
	}
	lseek(fdesc, 0L, SEEK_SET);
	
	close(blob_fd);
	blob_fd = open(BLOB_FILENAME, O_WRONLY, 0);
	
	fstart = lseek(blob_fd, 0L, SEEK_END);
	while ((n_read = read(fdesc, buffer, DEFAULT_BUFFER_SIZE)) > 0) {
		write(blob_fd, buffer, n_read);
	}
	
	written = lseek(blob_fd, 0, SEEK_CUR) - fstart;
	if (written != fsize) {
		fprintf(stderr, "haystack_add_file(): size: %ld, written: %ld\n",
						fsize, written);
		return 0;
	}
	
	if (finfo_list_size == finfo_list_cap)
		haystack_list_resize(finfo_list_size*DEFAULT_LIST_INCR);
	
	/* --- adding to file info list --- */
	finfo 		= &finfo_list[finfo_list_size];
	fname 	    = cut_file_path(new_fname);
	fname_len   = strlen(fname);
	
	finfo->_fname = (char*) malloc(fname_len);
	strcpy(finfo->_fname, fname);
	finfo->_start = fstart;
	finfo->_size  = fsize;
	
	/* --- adding to blin file --- */
	write( blin_fd, &fname_len,    sizeof(fname_len)  ); 
	write( blin_fd, finfo->_fname, fname_len	   	  );
	write( blin_fd, &fstart, 	     sizeof(fstart)   );
	write( blin_fd, &fsize,  	     sizeof(fsize)    );
	
	++finfo_list_size;
	return 1;
}

int haystack_get_file(char *fname, const char *out_fname) 
{
	int		 out_fd;
	char	 buffer[DEFAULT_BUFFER_SIZE];
	ssize_t  n_read, n_write_sum = 0;
	
	FileInfo *finfo = haystack_get_finfo(fname);
	if (finfo == NULL) {
		fprintf(stderr, "haystack_get_file(): no such file in blob\n");
		return 0;
	}
	
	out_fd = creat(out_fname, DEFAULT_PERMISSIONS);
	out_fd = open(out_fname, O_WRONLY, 0);
	if (out_fd == -1) {
		fprintf(stderr, "haystack_get_file(): can not create or open file\n");
		return 0;
	}
	
	close(blob_fd);
	blob_fd = open(BLOB_FILENAME, O_RDONLY, 0);
	lseek(blob_fd, finfo->_start, SEEK_SET);
	while (finfo->_size - n_write_sum >= DEFAULT_BUFFER_SIZE) {
		n_read = read(blob_fd, buffer, DEFAULT_BUFFER_SIZE);
		n_write_sum += write(out_fd, buffer, n_read);
	}
	
	n_read = read(blob_fd, buffer, finfo->_size-n_write_sum);
	n_write_sum += write(out_fd, buffer, n_read);
	if (n_write_sum != finfo->_size) {
		fprintf(stderr, "haystack_get_file(): size: %ld, written: %ld\n",
						finfo->_size, n_write_sum);
		return 0; 
	}
	
	return 1;
}

FileInfo* haystack_get_finfo(char *fname)
{
	size_t i;
	for (i = 0; i < finfo_list_size; ++i)
		if (strcmp(finfo_list[i]._fname, fname) == 0)
			return &finfo_list[i];
	return NULL;
}
