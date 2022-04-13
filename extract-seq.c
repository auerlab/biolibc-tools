/***************************************************************************
 *  Description:
 *      Extract a sequence from a reference genome using info from a GFF.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-10-28  Jason Bacon Begin
 ***************************************************************************/

#ifdef __linux__
#define _GNU_SOURCE // strcasestr()
#endif

#include <stdio.h>
#include <sysexits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <biolibc/gff.h>
#include <biolibc/fasta.h>
#include <xtend/file.h>

#define KEY_MAX     1024

int     process_subfeatures(bl_fasta_t *fasta_rec, FILE *gff_stream,
			    char *feature_type, char *parent);
void    print_seq(char *feature_seq, int64_t start, int64_t end);
void    usage(char *argv[]);

int     main(int argc,char *argv[])

{
    FILE        *fasta_stream,
		*gff_stream;
    char        *gff_file,
		*fasta_file,
		*feature_type,
		*search_key,
		*gff_chrom,
		*fasta_chrom,
		*parent_gene,
		*parent_transcript;
    int         status;
    size_t      chrom_len;
    unsigned long   start, end;
    bl_gff_t    feature;
    bl_fasta_t  fasta_rec = BL_FASTA_INIT;
    bool        found_chrom;

    switch(argc)
    {
	case 5:
	    gff_file = argv[1];
	    fasta_file = argv[2];
	    feature_type = argv[3];
	    search_key = argv[4];
	    break;
	
	default:
	    usage(argv);
	    return EX_USAGE;    // Useless but to silence compiler warning
    }
    
    // FIXME: Limit field input
    if ( (gff_stream = xt_fopen(gff_file, "r")) == NULL )
    {
	fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], gff_file,
		strerror(errno));
	return EX_NOINPUT;
    }

    // FIXME: Call this automatically from bl_gff_read() if needed?
    bl_gff_skip_header(gff_stream);

    // FIXME: Factor this out to a bl_gff_search() function in biolibc
    // FIXME: Collect a list of GFF features, not just one, and check them
    // all against each FASTA record
    // printf("Searching %s for %s %s\n", argv[1], feature_type, feature_name);
    bl_gff_init(&feature);
    parent_gene = parent_transcript = "X";
    while ( (status = bl_gff_read(&feature, gff_stream, BL_GFF_FIELD_ALL))
		== BL_READ_OK )
    {
	if ( (strcasecmp(BL_GFF_TYPE(&feature), feature_type) == 0) &&
	     (strcasestr(BL_GFF_ATTRIBUTES(&feature), search_key) != NULL) )
	{
	    gff_chrom = BL_GFF_SEQID(&feature);
	    chrom_len = strlen(gff_chrom);
	    start = BL_GFF_START(&feature);
	    end = BL_GFF_END(&feature);
	    // printf("%s %lu %lu\n", gff_chrom, start, end);
	    
	    printf("> %s %" PRId64 " %" PRId64 " %s %s %s %s %s\n", gff_chrom, start, end,
		    feature_type, search_key, 
		    BL_GFF_ATTRIBUTES(&feature), gff_file, fasta_file);

	    // GFFs are sorted lexically and FASTAs numerically
	    // so for now we re-read the FASTA from the beginning for
	    // each hit.
	    if ( (fasta_stream = xt_fopen(fasta_file, "r")) == NULL )
	    {
		fprintf(stderr, "%s: Cannot open %s: %s\n", argv[0], fasta_file,
			strerror(errno));
		return EX_NOINPUT;
	    }
    
	    // FIXME: Factor this out to bl_fasta_search() in biolibc?
	    found_chrom = false;
	    while ( !found_chrom &&
		    (bl_fasta_read(&fasta_rec, fasta_stream) == BL_READ_OK) )
	    {
		fasta_chrom = BL_FASTA_DESC(&fasta_rec) + 1;    // Skip '>'
		found_chrom = memcmp(fasta_chrom, gff_chrom, chrom_len) == 0;
		if ( found_chrom )
		{
		    // puts(BL_FASTA_DESC(&fasta_rec));
		    print_seq(BL_FASTA_SEQ(&fasta_rec), start, end);
		    
		    // Process subfeatures
		    if ( strcmp(feature_type, "gene") == 0 )
		    {
			parent_gene = BL_GFF_FEATURE_ID(&feature);
			fprintf(stderr, "Parent = %s\n", parent_gene);
			process_subfeatures(&fasta_rec, gff_stream,
					    feature_type, parent_gene);
		    }
		    else if ( strcmp(feature_type, "mRNA") == 0 )
		    {
			parent_transcript = BL_GFF_FEATURE_ID(&feature);
			fprintf(stderr, "Parent = %s\n", parent_transcript);
			// while read GFF != "mRNA"
		    }
		}
	    }
	    fclose(fasta_stream);
	}
    }
    fclose(gff_stream);
    
    // Extract sequence from FASTA
    return EX_OK;
}


/***************************************************************************
 *  Use auto-c2man to generate a man page from this comment
 *
 *  Library:
 *      #include <>
 *      -l
 *
 *  Description:
 *  
 *  Arguments:
 *
 *  Returns:
 *
 *  Examples:
 *
 *  Files:
 *
 *  Environment
 *
 *  See also:
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-12  Jason Bacon Begin
 ***************************************************************************/

int     process_subfeatures(bl_fasta_t *fasta_rec, FILE *gff_stream,
			    char *feature_type, char *parent)

{
    bl_gff_t    feature;
    
    fprintf(stderr, "Processing subfeatures of %s %s\n", feature_type, parent);
    // Read everything to next feature of the same type, i.e. all subfeatures
    bl_gff_init(&feature);
    while ( (bl_gff_read(&feature, gff_stream, BL_GFF_FIELD_ALL) == BL_READ_OK)
	    && (strcmp(BL_GFF_TYPE(&feature), feature_type) != 0) )
    {
	if ( strcmp(BL_GFF_FEATURE_PARENT(&feature), parent) == 0 )
	{
	    printf("> %s %" PRId64 " %" PRId64 " %s %s\n",
		    BL_GFF_SEQID(&feature),
		    BL_GFF_START(&feature),
		    BL_GFF_END(&feature),
		    BL_GFF_TYPE(&feature),
		    BL_GFF_ATTRIBUTES(&feature));
	    print_seq(BL_FASTA_SEQ(fasta_rec),
		      BL_GFF_START(&feature), BL_GFF_END(&feature));
	}
    }
    return 0;   // FIXME: Return a status value
}



/***************************************************************************
 *  Use auto-c2man to generate a man page from this comment
 *
 *  Library:
 *      #include <>
 *      -l
 *
 *  Description:
 *  
 *  Arguments:
 *
 *  Returns:
 *
 *  Examples:
 *
 *  Files:
 *
 *  Environment
 *
 *  See also:
 *
 *  History: 
 *  Date        Name        Modification
 *  2022-04-12  Jason Bacon Begin
 ***************************************************************************/

void    print_seq(char *feature_seq, int64_t start, int64_t end)

{
    int64_t feature_len = end - start + 1;
    int     save_base;
    
    feature_seq += start - 1;
    feature_seq[feature_len] = '\0';
    save_base = feature_seq[feature_len];
    puts(feature_seq);
    feature_seq[feature_len] = save_base;
}


void    usage(char *argv[])

{
    fprintf(stderr, "\nUsage: %s file.gff3 file.fasta feature-type 'search-key'\n", argv[0]);
    fprintf(stderr, "Search-key is any exact substring of the attributes column in the GFF.\n");
    fprintf(stderr, "To match a gene name exactly, include ';' in the search key. e.g. 'Name=jun;'\n");
    fprintf(stderr, "View your GFF file with \"more file.gff3\" to get ideas for search-key.\n");
    fprintf(stderr, "\nExample:\n\nblt extract-seq Danio_rerio.GRCz11.104.gff3 Danio_rerio.GRCz11.dna.primary_assembly.fa gene 'Name=jun;'\n\n");
    fprintf(stderr, "Be sure to enclose search-key in hard quotes ('') if it contains any special\n");
    fprintf(stderr, "characters such as *, ?, ;, etc.\n\n");
    exit(EX_USAGE);
}
