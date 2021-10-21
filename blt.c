/***************************************************************************
 *  Description:
 *      Wrapper to turn biolibc commands into subcommands.  This will help
 *      avoid future conflicts with other programs without sacrificing
 *      desriptive commands names.
 *      This wrapper can be installed under the standard
 *      PATH and used to to execute blt commands installed under a
 *      private prefix, without altering PATH, activating a special
 *      environment, opening a container, etc.  This sub-command paradigm
 *      is already familiar to bioinformaticians thanks to other suites
 *      like samtools, bedtools, etc.
 *
 *      Example:
 *
 *          blt fastx-derep args
 *
 *      instead of one of the following:
 *
 *          prefix/bin/fastx-derep args
 *
 *          env PATH=prefix/bin:$PATH fastx-derep args
 *
 *  Arguments:
 *      The subcommand and its specific arguments, as if it were run
 *      directly.
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-09-13  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>
#include <limits.h>
#include <unistd.h>

#ifndef LIBEXECDIR
#define LIBEXECDIR   "/usr/local/libexec/biolibc-tools"
#endif

int     main(int argc,char *argv[])

{
    char    cmd[PATH_MAX + 1];
    
    if ( argc < 2 )
    {
	fprintf(stderr, "Usage: %s blt-command [args]\n", argv[0]);
	return EX_USAGE;
    }

    snprintf(cmd, PATH_MAX, "%s/%s", LIBEXECDIR, argv[1]);
    execv(cmd, argv + 1);
}
