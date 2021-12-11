#include <stdio.h>
#include <sysexits.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <biolibc/translate.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    long        pos,
		offset,
		c;
    char        codon[4],
		*end;
    struct stat st;
    FILE        *stream = stdin;
    
    if ( argc != 2 )
	usage(argv);

    pos = strtol(argv[1], &end, 10);
    if ( *end != '\0' )
    {
	fprintf(stderr, "Invalid offset: %s\n", argv[1]);
	usage(argv);
    }
    ++pos;  // Make 1-based
    
    fstat(fileno(stream), &st);
    if ( ! (st.st_mode & S_IFIFO) )
    {
	if ( fseek(stream, pos - 1, SEEK_CUR) == -1 )
	{
	    fputs("Seek failed.", stderr);
	    return EX_SOFTWARE;
	}
    }
    else
    {
	for (c = 0; c < pos - 1; ++c)
	    getc(stream);
    }
    while ( ! feof(stream) )
    {
	if ( (offset = bl_next_start_codon(stream, codon)) != EOF )
	{
	    printf("Start codon %s at %ld (+%ld)\n", codon, pos + offset, offset);
	    pos += offset + 3;
	    if ( (offset = bl_next_stop_codon(stream, codon)) != EOF )
	    {
		printf("Stop codon %s at %ld (+%ld)\n", codon, pos + offset, offset);
		pos += offset + 3;
	    }
	    else
		puts("No stop codon found.");
	}
	else
	    puts("No start codon found.");
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s offset\n(offset is 0-based)\n",
	    argv[0]);
    exit(EX_USAGE);
}
