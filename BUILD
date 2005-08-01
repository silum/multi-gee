$Id$

Copyright (C) 2003, 2004, 2005 Deneys S. Maartens <dsm@tlabs.ac.za>

BUILD Procedure
===============

These are generic building instructions.  Please refer to INSTALL for
the installation instructions.  It is not neccessary to follow these
instructions if the distribution tree already contains a `configure'
script.

To build the package from sources found in the CVS or Subversion
repository, run the following commands from the command line:

    $ ./config/autogen.sh
    $ ./configure
    $ make

Now a distribution tarball can be created:

    $ make dist

Optionally, if the package provides this feature, build an RPM from the
distribution tarball using rpmbuild:

    $ rpmbuild -ta <package>-<version>.tar.gz

Alternatively, if the Makefile provides this target, use

    $ make distrpm

to create the RPMS.

