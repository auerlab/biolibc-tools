#!/bin/sh -e

##########################################################################
#
#   Perform a cave-man install for development and testing purposes.
#   For production use, this software should be installed via a package
#   manager such as Debian packages, FreeBSD ports, MacPorts, pkgsrc, etc.
#       
#   History:
#   Date        Name        Modification
#   2021-07-12  Jason Bacon Begin
##########################################################################

# Default to ../local if PREFIX is not set
: ${PREFIX:=../local}
: ${LOCALBASE:=/usr/local}

# OS-dependent tricks
# Set rpath to avoid picking up libs installed by package managers in
# /usr/local/lib, etc.
case $(uname) in
*)
    if [ -z "$CFLAGS" ]; then
	export CFLAGS="-Wall -g -O"
    fi
    for pkgsrc in /usr/pkg /opt/pkg ~/Pkgsrc/pkg; do
	if [ -e $pkgsrc ]; then
	    echo "Using $pkgsrc..."
	    LOCALBASE=$pkgsrc
	    if ! pkg_info uthash > /dev/null; then
		cd ${pkgsrc%pkg}/pkgsrc/devel/uthash && sbmake install
	    fi
	    if ! pkg_info xxhash > /dev/null; then
		cd ${pkgsrc%pkg}/pkgsrc/devel/xxhash && sbmake install
	    fi
	fi
    done
    ;;

esac

mkdir -p $PREFIX/libexec/biolibc-tools $PREFIX/lib
LIBDIR=$(realpath $PREFIX/lib)
LDFLAGS="-L. -L$LIBDIR -Wl,-rpath,$LIBDIR:/usr/lib:/lib"
LIBEXECDIR=$(realpath $PREFIX/libexec/biolibc-tools)
export PREFIX LOCALBASE LIBEXECDIR LDFLAGS
make clean
make install
