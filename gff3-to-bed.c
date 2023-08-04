/***************************************************************************
 *  Description:
 *      Convert a GFF3 stream to BED format.
 *
 *      Some tools accept only BED files as inputs.  This program provides
 *      a workaround.
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-14  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <biolibc/gff.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    bl_gff3_t    gff3_feature;
    bl_bed_t    bed_feature = BL_BED_INIT;

    if ( argc != 1 )
	usage(argv);
    
    bl_gff3_init(&gff3_feature);
    while ( bl_gff3_read(&gff3_feature, stdin, BL_GFF3_FIELD_ALL) == BL_READ_OK )
    {
	bl_gff3_to_bed(&gff3_feature, &bed_feature);
	bl_bed_write(&bed_feature, stdout, BL_BED_FIELD_ALL);
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    exit(EX_USAGE);
}
