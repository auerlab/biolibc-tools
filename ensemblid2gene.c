/***************************************************************************
 *  Description:
 *      Report the gane names for a list of ensembl IDs.  IDs should
 *      be genes, transcripts, or other elements containing both
 *      ID=<ensembl-id> and Name=<gene> in the attributes.
 *  
 *  History: 
 *  Date        Name        Modification
 *  2022-02-21  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <stdlib.h>
#include <xtend/file.h>
#include <xtend/mem.h>
#include <xtend/string.h>
#include <biolibc/gff.h>

void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    char    *gff3_file, *id_file;
    FILE    *gff3_stream, *id_stream;
    char    **id_list, *id, *p, **found;
    size_t  id_count;
    bl_gff3_t    feature;
    
    switch(argc)
    {
	case 3:
	    gff3_file = argv[1];
	    id_file = argv[2];
	    break;
	
	default:
	    usage(argv);
    }

    if ( (id_stream = fopen(id_file, "r")) == NULL )
    {
	fprintf(stderr, "ensemblid2gene: Could not open %s for read.\n",
		id_file);
	exit(EX_NOINPUT);
    }
    id_count = xt_inhale_strings(id_stream, &id_list);
    fclose(id_stream);
    qsort(id_list, id_count, sizeof(*id_list), (int(*)(const void *,const void*))strptrcmp);
    if ( (gff3_stream = fopen(gff3_file, "r")) == NULL )
    {
	fprintf(stderr, "ensemblid2gene: Could not open %s for read.\n",
		gff3_file);
	exit(EX_NOINPUT);
    }
    bl_gff3_skip_header(gff3_stream);
    
    bl_gff3_init(&feature);
    while ( bl_gff3_read(&feature, gff3_stream, BL_GFF3_FIELD_ALL) == BL_READ_OK )
    {
	//printf("%s ", BL_GFF3_FEATURE_NAME(&feature));
	//fflush(stdout);
	id = BL_GFF3_FEATURE_ID(&feature);
	if ( id != NULL )
	{
	    // GFF ID format is  ID=feature-type:feature-id
	    // E.g. ID=gene:ENS00000000
	    if ( (p = strchr(id, ':')) != NULL )
		id = p + 1;
	
	    // Find ID in the list matching this feature's ID= attribute
	    // Name= should then contain the gene name, assuming this ID
	    // is for a transcript or gene as it should be
	    found = bsearch(&id, id_list, id_count, sizeof(*id_list),
		    (int (*)(const void *, const void *))strptrcasecmp);
	    
	    // Print feature name without trailing -###
	    if ( found != NULL )
	    {
		p = BL_GFF3_FEATURE_NAME(&feature);
		printf("%s\t%s\n", *found, strsep(&p, "-"));
		if ( p != NULL )
		    p[-1] = '-';
	    }
	}
    }
    fclose(gff3_stream);

    xt_free_strings(id_list);
    return EX_OK;
}


void    usage(char *argv[])

{
    fprintf(stderr, "Usage: %s file.gff ensembl-id-list.txt\n", argv[0]);
    exit(EX_USAGE);
}
