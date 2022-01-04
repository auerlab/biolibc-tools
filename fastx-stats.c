/***************************************************************************
 *  Description:
 *      Gather basic stats on a FAST[AQ] file
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

int     main(int argc, char *argv[])

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

    memset(counts, 0, 26 * sizeof(*counts));
    switch(argc)
    {
	case 1:
	    fastx_stream = stdin;
	    break;
	
	case 2:
	    if ( (fastx_stream = xt_fopen(argv[1], "r")) == NULL )
	    {
		fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0],
			argv[1], strerror(errno));
		return EX_NOINPUT;
	    }
	    break;
	
	default:
	    fprintf(stderr, "Usage: %s [file.fast(aq)[.gz|.bz2|.xz]]\n",
		    argv[0]);
	    return EX_USAGE;
    }
	
    bl_fastx_init(fastx_stream, &rec);
    while ( bl_fastx_read(fastx_stream, &rec) != BL_READ_EOF )
    {
	++records;
	len = bl_fastx_seq_len(&rec);
	bases += len;
	if ( len > max_len )
	    max_len = len;
	if ( len < min_len )
	    min_len = len;
	for (p = bl_fastx_seq(&rec); *p != '\0'; ++p)
	    ++counts[tolower(*p) - 'a'];
    }
    if ( argc == 1 )
	xt_fclose(fastx_stream);
    bl_fastx_free(&rec);
    printf("Sequences:  %lu\n", records);
    printf("Bases:      %lu\n", bases);
    printf("Avg length: %lu\n", bases / records);
    printf("Min length: %lu\n", min_len);
    printf("Max length: %lu\n", max_len);
    printf("A:          %lu (%lu%%)\n", counts['a' - 'a'],
	    counts['a' - 'a'] * 100UL / bases);
    printf("C:          %lu (%lu%%)\n", counts['c' - 'a'],
	    counts['c' - 'a'] * 100UL / bases);
    printf("G:          %lu (%lu%%)\n", counts['g' - 'a'],
	    counts['g' - 'a'] * 100UL / bases);
    printf("T:          %lu (%lu%%)\n", counts['t' - 'a'],
	    counts['t' - 'a'] * 100UL / bases);
    printf("N:          %lu (%lu%%)\n", counts['n' - 'a'],
	    counts['n' - 'a'] * 100UL / bases);
    printf("GC:         %lu (%lu%%)\n", counts['g' - 'a'] + counts['c' - 'a'],
	    (counts['g' - 'a'] + counts['c' - 'a']) * 100UL / bases);
    return EX_OK;
}
