#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <biolibc/gff.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    bl_gff_t    gff_feature;
    bl_bed_t    bed_feature = BL_BED_INIT;

    if ( argc != 1 )
	usage(argv);
    
    bl_gff_init(&gff_feature);
    while ( bl_gff_read(&gff_feature, stdin, BL_GFF_FIELD_ALL) == BL_READ_OK )
    {
	bl_gff_to_bed(&gff_feature, &bed_feature);
	bl_bed_write(&bed_feature, stdout, BL_BED_FIELD_ALL);
    }
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s\n", argv[0]);
    exit(EX_USAGE);
}
