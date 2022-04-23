# biolibc-tools

## Description

Biolibc-tools is a collection of fast, memory-efficient, simple programs
based on biolibc for processing biological data.

These are programs too simple to warrant a separate project, because most
of the functionality is provided by the biolibc library.

They provide permanent, portable solutions for basic data processing tasks
in a form that's easier to install and use, and often more CPU and memory
efficient than analogous tools.

The needs met by biolibc-tools will in turn drive improvements to biolibc.
Most of the new code will go into the library, making it easier over time
to develop more simple C tools for bioinformatics.

## Building and installing

biolibc-tools is intended to build cleanly in any POSIX environment on
any CPU architecture.  Please
don't hesitate to open an issue if you encounter problems on any
Unix-like system.

Primary development is done on FreeBSD with clang, but the code is frequently
tested on Linux, MacOS, and NetBSD as well.  MS Windows is not supported,
unless using a POSIX environment such as Cygwin or Windows Subsystem for Linux.

The Makefile is designed to be friendly to package managers, such as
[Debian packages](https://www.debian.org/distrib/packages),
[FreeBSD ports](https://www.freebsd.org/ports/),
[MacPorts](https://www.macports.org/), [pkgsrc](https://pkgsrc.org/), etc.
End users should install via one of these if at all possible.

I maintain a FreeBSD port and a pkgsrc package, which is sufficient to install
cleanly on virtually any POSIX platform.  If you would like to see a
biolibc-tools package in another package manager, please consider creating a
package yourself.  This will be one of the easiest packages in the collection
and hence a good vehicle to learn how to create packages.

For an overview of available package managers, see the
[Repology website](https://repology.org/).

### Installing biolibc-tools on FreeBSD:

FreeBSD is a highly underrated platform for scientific computing, with over
2,000 scientific libraries and applications in the FreeBSD ports collection
(of more than 30,000 total), modern clang compiler, fully-integrated ZFS
filesystem, and renowned security, performance, and reliability.
FreeBSD has a somewhat well-earned reputation for being difficult to set up
and manage compared to user-friendly systems like [Ubuntu](https://ubuntu.com/).
However, if you're a little bit Unix-savvy, you can very quickly set up a
workstation, laptop, or VM using
[desktop-installer](http://www.acadix.biz/desktop-installer.php).

To install the binary package on FreeBSD:

```
pkg install biolibc-tools
```
You can just as easily build and install from source.  This is useful for
FreeBSD ports with special build options, for building with non-portable
optimizations such as -march=native, and for 
[work-in-progress ports](https://github.com/outpaddling/freebsd-ports-wip),
for which binary packages are not yet maintained.

```
cd /usr/ports/biology/biolibc-tools && env CFLAGS='-march=native -O2' make install
cd /usr/ports/wip/biolibc-tools && make install
```

### Installing via pkgsrc

pkgsrc is a cross-platform package manager that works on any Unix-like
platform. It is native to [NetBSD](https://www.netbsd.org/) and well-supported
on [Illumos](https://illumos.org/), [MacOS](https://www.apple.com/macos/),
[RHEL](https://www.redhat.com)/[CentOS](https://www.centos.org/), and
many other Linux distributions.
Using pkgsrc does not require admin privileges.  You can install a pkgsrc
tree in any directory to which you have write access and easily install any
of the nearly 20,000 packages in the collection.  The
[auto-pkgsrc-setup](https://github.com/outpaddling/auto-admin/blob/master/auto-pkgsrc-setup.md)
script will guide you through a basic setup in minutes.

First bootstrap pkgsrc using auto-pkgsrc-setup or any other method.

Then run the following commands:

```
cd pkgsrc-dir/biology/biolibc-tools
bmake install clean
```

There may also be binary packages available for your platform.  If this is
the case, you can install by running:

```
pkgin install biolibc-tools
```

See the [Joyent Cloud Services Site](https://pkgsrc.joyent.com/) for
available package sets.

### Building biolibc-tools locally

Below are cave man install instructions for development purposes, not
recommended for regular use.

biolibc-tools depends on 
[biolibc](https://github.com/auerlab/biolibc) and
[libxtend](https://github.com/outpaddling/libxtend).
Install biolibc and libxtend before attempting to build biolibc-tools.

1. Clone biolibc-tools and dependencies into sibling directories
2. In each dependency directory and then in biolibc-tools:
    
    1. Run "make depend" to update Makefile.depend
    2. Run "./cave-man-install.sh" if present, otherwise "make install"

The default install prefix is ../local.  Clone biolibc-tools, biolibc, and
libxtend into sibling directories so that ../local represents a common path
to all of them.

To facilitate incorporation into package managers, the Makefile respects
standard make/environment variables such as CC, CFLAGS, PREFIX, etc.  

The library, headers, and man pages are installed under
${DESTDIR}${PREFIX}.  DESTDIR is empty by default and is primarily used by
package managers to stage installations.  PREFIX defaults to ../local.

Add-on libraries required for the build, such as biolibc, should be found
under either ${PREFIX} or ${LOCALBASE}, which defaults to ${PREFIX}.
LOCALBASE can be set independently if you want to use libraries installed
by FreeBSD ports (/usr/local), MacPorts (/opt/local), pkgsrc (/usr/pkg), etc.

To install directly to /myprefix, assuming biolibc is installed there as well,
using a make variable:

```
make PREFIX=/myprefix clean depend install
```

Using an environment variable:

```
# C-shell and derivatives
setenv PREFIX /myprefix
make clean depend install

# Bourne shell and derivatives
PREFIX=/myprefix
export PREFIX
make clean depend install
```

View the Makefile for full details.
