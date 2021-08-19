/***************************************************************************
 *  Description:
 *      Remove duplicates from a fastq stream.  This implementation is
 *      similar to seqkit rmdup, using xxhash to generate hash keys and
 *      uthash to build a hash table and check for duplicates.
 *
 *      Currently this implementation has the same runtime as seqkit rmdup,
 *      uses significantly less memory, but does not filter all the same
 *      records.  seqkit appears to filter the correct number of records
 *      based on agreement with the sort - uniq method.
 *
 *      This tool was created primarily to demonstrate how easy it can be
 *      to write bioinformatics apps in C, given the right libraries.
 *      In this case, biolibc handles all the FASTQ I/O, xxhash the sequence
 *      hasing, and uthash the hash table management.
 *  
 *  History: 
 *  Date        Name        Modification
 *  2021-07-27  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <uthash.h>
#include <xxhash.h>
#include <biolibc/fastq.h>
#include <xtend/mem.h>

typedef struct
{
    XXH64_hash_t    hash;
    UT_hash_handle  hh;
}   entry_t;

int     main(int argc, char *argv[])

{
    bl_fastq_t      rec = BL_FASTQ_INIT;
    size_t          records_read,
		    records_written;
    int             seed = 0;
    entry_t         *table = NULL,
		    *entry,
		    *found = NULL;
    XXH64_hash_t    hash;
    
    records_read = records_written = 0;
    while ( bl_fastq_read(stdin, &rec) == BL_READ_OK )
    {
	++records_read;
	hash = XXH64(BL_FASTQ_SEQ(&rec), BL_FASTQ_SEQ_LEN(&rec), seed);
	HASH_FIND(hh, table, &hash, sizeof(hash), found);
	if ( found == NULL )
	{
	    // Record key for comparison to future records
	    entry = xt_malloc(1, sizeof(entry_t));
	    entry->hash = hash;
	    HASH_ADD(hh, table, hash, sizeof(entry->hash), entry);
	    
	    // Output record
	    bl_fastq_write(stdout, &rec, BL_FASTQ_LINE_UNLIMITED);
	    ++records_written;
	}
    }
    
    fprintf(stderr, "%zu records read, %zu written, %zu removed\n",
	   records_read, records_written, records_read - records_written);
    return EX_OK;
}
