/***************************************************************************
 *  Description:
 *      Display basic stats on a FAST[AQ] file such as size, base content
 *      percentages, etc.
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
#include <math.h>
#include <xtend/string.h>
#include <xtend/file.h>
#include <biolibc/fastx.h>
#include <biolibc/biolibc.h>

int     fastx_stats(char *filename);

int     main(int argc, char *argv[])

{
    int     arg,
	    status;

    switch(argc)
    {
	case 1:
	    return fastx_stats("-");
	    break;
	
	default:
	    for (arg = 1; arg < argc; ++arg)
		if ( (status = fastx_stats(argv[arg])) != EX_OK )
		    return status;
    }
	
    return EX_OK;
}


int     fastx_stats(char *filename)

{
    bl_fastx_t      rec = BL_FASTX_INIT;
    unsigned long   records = 0,
		    bases = 0,
		    len,
		    min_len = ULONG_MAX,
		    max_len = 0,
		    counts[26];
    FILE            *fastx_stream;
    char            *p;
    int             status;
    double          mean_len, sum_sq, variance;

    if ( strcmp(filename, "-") == 0 )
	fastx_stream = stdin;
    else if ( (fastx_stream = xt_fopen(filename, "r")) == NULL )
    {
	fprintf(stderr, "fastx-stats: Cannot open %s: %s\n",
		filename, strerror(errno));
	return EX_NOINPUT;
    }

    memset(counts, 0, 26 * sizeof(*counts));
    bl_fastx_init(&rec, fastx_stream);
    sum_sq = 0.0;
    while ( (status = bl_fastx_read(&rec, fastx_stream)) == BL_READ_OK )
    {
	++records;
	len = bl_fastx_seq_len(&rec);
	bases += len;
	sum_sq += len * len;
	if ( len > max_len )
	    max_len = len;
	if ( len < min_len )
	    min_len = len;
	for (p = bl_fastx_seq(&rec); *p != '\0'; ++p)
	    ++counts[tolower(*p) - 'a'];
    }

    if ( fastx_stream != stdin )
	xt_fclose(fastx_stream);
    bl_fastx_free(&rec);
    
    if ( status != BL_READ_EOF )
    {
	fprintf(stderr, "Error reading file: %s\n", strerror(errno));
	return EX_DATAERR;
    }
    
    mean_len = (double)bases / records;
    // Single-pass variance
    variance = sum_sq / records - mean_len * mean_len;
    
    printf("\nFilename:           %s\n", filename);
    printf("Sequences:          %lu\n", records);
    printf("Bases:              %lu\n", bases);
    printf("Mean-length:        %0.2f\n", mean_len);
    printf("Standard-deviation: %0.2f\n", sqrt(variance));
    printf("Min-length:         %lu\n", min_len);
    printf("Max-length:         %lu\n", max_len);
    printf("A:                  %lu (%0.2f%%)\n", counts['a' - 'a'],
	    counts['a' - 'a'] * 100.0 / bases);
    printf("C:                  %lu (%0.2f%%)\n", counts['c' - 'a'],
	    counts['c' - 'a'] * 100.0 / bases);
    printf("G:                  %lu (%0.2f%%)\n", counts['g' - 'a'],
	    counts['g' - 'a'] * 100.0 / bases);
    printf("T:                  %lu (%0.2f%%)\n", counts['t' - 'a'],
	    counts['t' - 'a'] * 100.0 / bases);
    printf("N:                  %lu (%0.2f%%)\n", counts['n' - 'a'],
	    counts['n' - 'a'] * 100.0 / bases);
    printf("GC:                 %lu (%0.2f%%)\n", counts['g' - 'a'] + counts['c' - 'a'],
	    (counts['g' - 'a'] + counts['c' - 'a']) * 100.0 / bases);
    return EX_OK;
}
