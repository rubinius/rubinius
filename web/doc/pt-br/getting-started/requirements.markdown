---
layout: doc_pt_br
title: Requirements
previous: Getting Started
previous_url: getting-started
next: Building
next_url: getting-started/building
---

Ensure you have the following programs and libraries installed. Also see the
subsections below for special requirements for your particular operating
system.

The following are suggestions for getting more information about the programs
and libraries needed to build Rubinius. Your operating system or package
manager may have other packages available.

  * [GCC and G++ 4.x](http://gcc.gnu.org/)
  * [GNU Bison](http://www.gnu.org/software/bison/)
  * [MRI Ruby 1.8.7+](http://www.ruby-lang.org/) If your system does not have
    Ruby 1.8.7 installed, consider using [RVM](http://rvm.beginrescueend.com/)
    to install it.
  * [Rubygems](http://www.rubygems.org/)
  * [Git](http://git.or.cz/)
  * [ZLib](http://www.zlib.net/)
  * pthread - The pthread library should be installed by your operating system
  * [gmake](http://savannah.gnu.org/projects/make/)
  * [rake](http://rake.rubyforge.org/) `[sudo] gem install rake`


### Apple OS X

The easiest way to get a build environment on Apple OS X is to install the
XCode Tools and Utilities. Once installed, you can enable developer mode crash
reporting at: /Developer/Applications/Utilities/CrashReporterPrefs.app


### Debian/Ubuntu

  * ruby-dev or ruby1.8-dev
  * libreadline5-dev
  * zlib1g-dev
  * libssl-dev
