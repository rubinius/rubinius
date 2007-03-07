C/Invoke: A library for invoking C functions at runtime.

Copyright (c) 2006 by Will Weisser

See LICENSE.txt for license info.

The C/Invoke homepage is:
http://www.nongnu.org/cinvoke/

Please subscribe to the cinvoke-dev mailing list, more
information at the above URL.

To build and install the library, run configure.sh to see a list of
available platforms, then run it again to create the Makefiles.  Then
enter 'make', then edit the top-level Makefile to change the install path
if neccessary, and enter 'make install'.

The language bindings in the bindings/ directory require the library
to be installed in order to build; their Makefiles are not set up to build
from C/Invoke compiled in the source directory.
