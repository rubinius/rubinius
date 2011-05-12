---
layout: doc_pt_br
title: Troubleshooting
previous: Running Rubinius
previous_url: getting-started/running-rubinius
next: Contributing
next_url: contributing
---

The following are errors that you may encounter while building, installing, or
running Rubinius along with suggested solutions.

For any error, a first step is to ensure you have a current, clean checkout of
Rubinius. Before going further, consider running the following steps:

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### Rubinius is unable to find the `runtime` directory

  After building or installing, the following error occurs when attempting to
  run Rubinius:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Solution:

  If you configured Rubinius with a `--prefix`, run rake install.

  If you configured Rubinius with a `--prefix` and renamed the install
  directory after installing Rubinius, re-configure Rubinius and reinstall it.

  If you renamed the source directory after building Rubinius, re-configure
  and rebuild it.

  In general, do not rename the build or install directory after building or
  installing Rubinius.


### Rubinius segfaults when building on FreeBSD

  On FreeBSD, including up to version 8.1 stable, there is an issue with execinfo that
  causes Rubinius to segfault when it loads.

#### Solution:

  Disable execinfo when configuring Rubinius:

    ./configure --without-execinfo
