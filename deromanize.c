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
#include <xtend/dsv.h>
#include <xtend/string.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    int     col, num, ch;
    char    *end, *field, arabic[10],
	    *roman[] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII",
			 "IX", "X", "XI", "XII", "XIII", "XIV", "XV", "XVI",
			 "XVII", "XVIII", "XIX", "XX", "XXI", "XXII", "XXIII",
			 "XXIV", "XXV", "XXVI", "XXVII", "XXVIII", "XXIX",
			 "XXX", "XXXI", "XXXII", "XXXIII", "XXXIV", "XXXV",
			 "XXXVI", "XXXVII", "XXXVIII", "XXXIX", "XL",
			 "XLI", "XLII", "XLIII", "XLIV", "XLV", "XLVI",
			 "XLVII", "XLVIII", "XLIX", "L", NULL };
    FILE    *infile = stdin, *outfile = stdout;
    dsv_line_t  line;
    
    switch(argc)
    {
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
	else
	{
	    ungetc(ch, infile);
	    dsv_line_read(&line, infile, "\t");
	    field = DSV_LINE_FIELDS_AE(&line, col);
	    // FIXME: Find a more efficient approach than linear search
	    for (num = 0; (roman[num] != NULL)
			  && (strcmp(roman[num], field) != 0); ++num)
		;
	    if ( roman[num] != NULL )
	    {
		snprintf(arabic, 10, "%d", num + 1);
		free(DSV_LINE_FIELDS_AE(&line, col));
		dsv_line_set_fields_ae(&line, col, strdup(arabic));
	    }
	    dsv_line_write(&line, outfile);
	    dsv_line_free(&line);
	}
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    exit(EX_USAGE);
}
