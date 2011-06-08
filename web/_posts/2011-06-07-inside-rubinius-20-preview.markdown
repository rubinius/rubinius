---
layout: post
title: Inside the Rubinius 2.0 Preview Release
author: Brian Ford
---

Houston, we have a problem. Complex projects are difficult to transition
forward and Rubinius is a complex project. Rubinius is at the point of a major
transition and we need your help to move forward.

Just over a year ago, Rubinius released version 1.0. Since then, dozens of
contributors have added over 2,200 commits to the master branch and we have
released seven new versions. At the same time, we have added nearly 450
commits on a development branch to remove the global interpreter lock (GIL)
and begin adding Ruby 1.9 and Windows support.

![Rubinius 2.0 Developer Preview](/images/rubinius-2-0-developer-preview-blog-post.png)

Today we are releasing *Rubinius 2.0 Developer Preview*. The primary focus of
this release is true Ruby multi-threaded concurrency. Additionally, it
contains the beginnings of Ruby 1.9 and Microsoft Windows support.


### Rubinius 2.0 Developer Preview

We are seeking developers interested in running their Ruby and Rails projects
on Rubinius to help us iron out issues as we work toward the final 2.0
release. Let's look at the details of the 2.0 developer preview.

One of the central features of Rubinius 2.0 is a fundamental change in the
threading architecture. In Rubinius 2.0, Ruby threads will run with true
concurrency. This means that if multi-core or multi-CPU hardware is available,
Ruby code on different threads will actually run in parallel.

Another central feature of Rubinius 2.0 is support for both Ruby 1.8.7 and
Ruby 1.9.2 syntax. The Rubinius 2.0 support for Ruby 1.8.7 should be 100%
backward compatible with the support in the Rubinius master branch. The full
Ruby concurrency is enabled regardless of what language syntax mode is active.


### Repository Migration

We are migrating the main Rubinius repository to the [Rubinius](
https://github.com/rubinius) organization.

The RVM project has already been updated to work with the new repository. So
`rvm install rbx` will use the new repository.  If you have an existing clone
of Rubinius, update and run `rake github` to point your existing repository to
the new one.

You can clone directly from the [new repository](https://github.com/rubinius/rubinius).


### Installation

For testing Rubinius with your application, installing with RVM is probably
the easiest. To install the 2.0 developer preview, ensure that RVM is updated:

    rvm get head
    rvm reload

Then, install the Rubinius 2.0.0pre branch:

    rvm install rbx-2.0.0pre

If you are interested in helping develop Rubinius, we suggest you clone the
repository directly and build. Rubinius runs fine from the source directory so
you don't need to install it. For more details about building from a clone,
see [Getting Started](http://rubini.us/doc/en/getting-started/).


### Configuration

The goal of Rubinius 2.0 is to fully support both Ruby 1.8.7 and 1.9.2 in a
single executable. When running Rubinius, you can select which version to use.
There are configuration options to change which versions are supported and
which version is the default.

The first configure option is `--enable-version=X[,Y]`. This option sets which
language versions will be supported. An example is:

    ./configure --enable-version=1.8,1.9

The language versions can be specified as `18` or `1.8`. Multiple versions are
separated by a comma and no space.

The second configure option is `--default-version=X`. This option sets which
language version is the default when running Rubinius. An example is:

    ./configure --default-version=1.9

These options are intended to be used together. You must enable a version to
select it as the default, so the options must come in the following order:

    ./configure --enable-version=X,Y --default-version=Y

Once you have enabled support for a language version, to select that mode when
running Rubinius, use the `-X18` or `-X19` command line switches. For example:

    rbx -X19 -v

This command will run Rubinius with the 1.9 language mode enabled.

You may also use the `RBXOPT` environment variable to select the language
mode. Assuming that 1.8 is the default mode, the following command would run
Rubinius with the 1.9 language mode enabled:

    RBXOPT=-X19 rbx -v

Right now, only the 1.8 language mode is enabled in the 2.0.0pre branch.
However, we will be enabling the 1.9 mode in the very near future. The default
mode will continue to be 1.8 unless you configure Rubinius to run in 1.9 mode.


### Building on Windows

On Windows, Rubinius uses the [MinGW-w64](http://mingw-w64.sourceforge.net/)
compiler to build a native application. There remains a lot of work to do for
Windows support but the Rubinius VM is currently compiling on Windows 7.

If you are a bleeding-edge Windows developer interested in diving into
Rubinius, here's how to get started:

1. Install *MRI 1.9* using [RubyInstaller](http://rubyinstaller.org/).
1. Install [MSysGit](http://code.google.com/p/msysgit/).
1. Install the 32bit or 64bit MinGW-w64 toolchain.
1. Clone the Rubinius repository.
1. In CMD, run `ruby configure`.
1. Run `rake`.


### Moving Forward

In the near future, we will release another version of Rubinius from the
current master branch. We hope to merge 2.0.0pre into master as soon as
possible and continue working toward the 2.0 final release. We'll make that
decision based on your reports about how the 2.0.0pre branch is working with
existing 1.8.7 applications.

We greatly appreciate your help polishing up the Rubinius 2.0 release! Follow
us on [Twitter](http://twitter.com/rubinius) for updates and stop by #rubinius
on freenode.net to chat about anything on your mind.


### Rubinius Support on EngineYard AppCloud

Rubinius 1.2.3 is currently in beta on Engine Yard AppCloud, and we encourage
users to [give it a try](http://docs.engineyard.com/rubinius.html). Engine
Yard will support Rubinius 2.0.0 after it’s officially released.

To learn more about Rubinius, join Engine Yard for a [Platform Options Webinar](http://pages.engineyard.com/RubyPlatformOptionsWebinarJuly212011.html) in July.
