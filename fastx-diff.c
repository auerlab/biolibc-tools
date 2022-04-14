/***************************************************************************
 *  Description:
 *      Show differences between two FAST[AQ] files, like the standard
 *      Unix diff commmand for plain text files.
 *
 *      Currently very simple and only useful to compare files with the
 *      same set of reads that have been modified in some way, such as
 *      a trimmed and untrimmed version of a FASTQ.
 *  
 *  History: 
 *  Date        Name        Modification
 *  2022-01-04  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <xtend/string.h>
#include <xtend/file.h>
#include <biolibc/fastx.h>
#include <biolibc/biolibc.h>

int     fastx_diff(char *filename1, char *filename2);

int     main(int argc, char *argv[])

{
    switch(argc)
    {
	case 3:
	    return fastx_diff(argv[1], argv[2]);
	    break;
	
	default:
	    fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
	    return EX_USAGE;
    }
	
    return EX_OK;
}


int     fastx_diff(char *filename1, char *filename2)

{
    FILE        *stream1,
		*stream2;
    bl_fastx_t  rec1 = BL_FASTX_INIT,
		rec2 = BL_FASTX_INIT;
    int         s1, s2, e1, e2;
    char        *seq1, *seq2;
    
    if ( (stream1 = xt_fopen(filename1, "r")) == NULL )
    {
	fprintf(stderr, "fastx-diff: Cannot open %s: %s\n",
		filename1, strerror(errno));
	return EX_NOINPUT;
    }
    if ( (stream2 = xt_fopen(filename2, "r")) == NULL )
    {
	fprintf(stderr, "fastx-diff: Cannot open %s: %s\n",
		filename2, strerror(errno));
	return EX_NOINPUT;
    }
    
    bl_fastx_init(stream1, &rec1);
    bl_fastx_init(stream2, &rec2);

    while ( true )
    {
	s1 = bl_fastx_read(stream1, &rec1);
	e1 = errno;
	s2 = bl_fastx_read(stream2, &rec2);
	e2 = errno;
	
	if ( (s1 != BL_READ_OK) || (s1 != BL_READ_OK) )
	    break;
	
	seq1 = bl_fastx_seq(&rec1);
	seq2 = bl_fastx_seq(&rec2);
	if ( strcmp(seq1, seq2) != 0 )
	{
	    printf("- %s\n", seq1);
	    printf("+ %s\n", seq2);
	}
    }
    
    if ( s1 != BL_READ_EOF )
	fprintf(stderr, "Error reading %s: %s\n", filename1, strerror(e1));
    if ( s1 != BL_READ_EOF )
	fprintf(stderr, "Error reading %s: %s\n", filename1, strerror(e2));
    
    xt_fclose(stream1);
    xt_fclose(stream1);
    bl_fastx_free(&rec1);
    bl_fastx_free(&rec2);
    
    return EX_OK;
}
