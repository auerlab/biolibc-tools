/***************************************************************************
 *  Description:
 *      Convert Roman numerals to Arabic in specified columns
 *
 *  Arguments:
 *
 *  Returns:
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
#include <xtend/stdlib.h>   // romantoi()

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    int     col, num, ch;
    char    *end, *field, arabic[10];
    FILE    *infile = stdin, *outfile = stdout;
    dsv_line_t  line;
    
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
    
    dsv_line_init(&line);
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
	    dsv_line_read(&line, infile, "\t");
	    field = DSV_LINE_FIELDS_AE(&line, col);
	    if ( field != NULL )
	    {
		num = romantoi(field, &end);
		if ( *end == '\0' )
		{
		    snprintf(arabic, 10, "%d", num);
		    free(DSV_LINE_FIELDS_AE(&line, col));
		    dsv_line_set_fields_ae(&line, col, strdup(arabic));
		}
		else
		{
		    fprintf(stderr,
			"deromanize: Error: Field is not a Roman numeral: %s\n",
			field);
		    return EX_DATAERR;
		}
		dsv_line_write(&line, outfile);
	    }
	    dsv_line_free(&line);
	}
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s column < input-file\n", argv[0]);
    exit(EX_USAGE);
}
