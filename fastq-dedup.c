/***************************************************************************
 *  Description:
 *      Remove duplicates from a fastq stream
 *  
 *  History: 
 *  Date        Name        Modification
 *  2021-07-27  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <string.h>
#include <stdlib.h>
#include <xtend/string.h>
#include <biolibc/fastq.h>
#include <biolibc/biolibc.h>
#include <xtend/mem.h>

int     bl_fastq_seq_cmp(bl_fastq_t **f1, bl_fastq_t **f2)

{
    return strcmp(BL_FASTQ_SEQ(*f1), BL_FASTQ_SEQ(*f2));
}


int     main(int argc, char *argv[])

{
    bl_fastq_t  **array;
    size_t      records_read,
		records_written,
		array_size,
		c;
    int         status;
    
    array_size = 1024;
    array = xt_malloc(array_size, sizeof(*array));
    records_read = 0;
    do
    {
	array[records_read] = xt_malloc(1, sizeof(*array[records_read]));
	bl_fastq_init(array[records_read]);
	if ( (status = bl_fastq_read(stdin, array[records_read])) == BL_READ_OK )
	{
	    if ( ++records_read == array_size )
	    {
		array_size *= 2;
		array = xt_realloc(array, array_size, sizeof(*array));
	    }
	}
	else
	    bl_fastq_free(array[records_read]);
    }   while ( status != BL_READ_EOF );
    fprintf(stderr, "%zu records read.\n", records_read);
    
    qsort(array, records_read, sizeof(*array),
	  (int (*)(const void *, const void *))bl_fastq_seq_cmp);
    
    records_written = 0;
    bl_fastq_write(stdout, array[0], BL_FASTQ_LINE_UNLIMITED);
    for (c = 1; c < records_read; ++c)
    {
	if ( strcmp(BL_FASTQ_SEQ(array[c]), BL_FASTQ_SEQ(array[c-1])) != 0 )
	{
	    bl_fastq_write(stdout, array[c], BL_FASTQ_LINE_UNLIMITED);
	    ++records_written;
	}
    }
    printf("%zu records written, %zu removed\n",
	   records_written, records_read - records_written);
    return EX_OK;
}
