/***************************************************************************
 *  Description:
 *      Remove duplicates from a fastq stream
 *      This is a naive brute-force method requiring a lot of memory
 *      TODO: Explore a hash-table approach
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
    
    records_read = records_written = 0;
    while ( bl_fastq_read(stdin, &rec) == BL_READ_OK )
    {
	++records_read;
	entry = xt_malloc(1, sizeof(entry_t));
	entry->hash = XXH64(BL_FASTQ_SEQ(&rec), BL_FASTQ_SEQ_LEN(&rec), seed);
	HASH_FIND_INT(table, &entry->hash, found);
	if ( found == NULL )
	{
	    HASH_ADD_INT(table, hash, entry);
	    ++records_written;
	}
	else
	    fprintf(stderr, "Duplicate: %s\n", BL_FASTQ_SEQ(&rec));
    }
    fprintf(stderr, "%zu records written, %zu removed\n",
	   records_written, records_read - records_written);
    return EX_OK;
}
