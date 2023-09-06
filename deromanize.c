/***************************************************************************
 *  Description:
 *      Convert Roman numerals to Arabic in specified columns
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-11-05  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <xtend/dsv.h>
#include <xtend/string.h>
#include <xtend/stdlib.h>   // xt_romantoi()

#define MAX_ARABIC_DIGITS   64

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    int     col, num, ch;
    char    *end, *field, arabic[MAX_ARABIC_DIGITS];
    FILE    *infile = stdin, *outfile = stdout;
    xt_dsv_line_t  *dsv_line = xt_dsv_line_new();
    
    switch(argc)
    {
	case 3:
	    infile = fopen(argv[2], "r");
	    if ( infile == NULL )
	    {
		fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0],
			argv[2], strerror(errno));
		return EX_DATAERR;
	    }
	case 2:
	    col = strtol(argv[1], &end, 10) - 1;
	    if ( *end != '\0' )
		usage(argv);
	    break;
	
	default:
	    usage(argv);
    }
    
    xt_dsv_line_init(dsv_line);
    while ( !feof(infile) )
    {
	if ( (ch = getc(infile)) == '#' )
	{
	    putc(ch, outfile);
	    while ( ((ch = getc(infile)) != '\n') && (ch != EOF) )
		putc(ch, outfile);
	    if ( ch == '\n' )
		putc(ch, outfile);
	}
	else if ( ch != EOF )
	{
	    ungetc(ch, infile);
	    xt_dsv_line_read(dsv_line, infile, "\t");
	    field = xt_dsv_line_get_fields_ae(dsv_line, col);
	    if ( field != NULL )
	    {
		num = xt_romantoi(field, &end);
		// Convert Roman numerals, leave other data alone
		if ( *end == '\0' )
		{
		    snprintf(arabic, MAX_ARABIC_DIGITS, "%d", num);
		    free(xt_dsv_line_get_fields_ae(dsv_line, col));
		    xt_dsv_line_set_fields_ae(dsv_line, col, strdup(arabic));
		}
		xt_dsv_line_write(dsv_line, outfile);
	    }
	    xt_dsv_line_free(dsv_line);
	}
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s column [input-file]\n", argv[0]);
    exit(EX_USAGE);
}
