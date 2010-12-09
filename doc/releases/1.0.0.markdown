---
layout: release
class: release
---

### Rubinius 1.0 (Fabius) Released!

#### Thanks

The Rubinius 1.0 release has been a long time coming and the team is very
proud to announce that it is finally ready!

This marks the end of a long and wonderful road we have all traveled over the
last 3 1/2 years. There are so many people to thank for their help and
support.

[Fabius](http://en.wikipedia.org/wiki/Fabius_Maximus) is the release name of
version 1.0. This is a reference to the great defender of Rome. This name was
given to Rubinius by [Geoffrey Grosenbach](http://peepcode.com/), who secured
naming rights to version 1.0 many years ago by being the very first person to
provide some financial backing for the project. I can't thank him enough.

Tom Mornini and Engine Yard deserve the lion's share of the thanks. Back in
2007 when Tom asked me to come work on Rubinius at Engine Yard, it was a dream
come true. They've tirelessly supported the project over the years, even when
project momentum waned.

And lastly, thanks to everyone who has contributed to the project. Over the
years, there have been over 200 contributers who've given up their free time
to help make Rubinius great.

_- Evan Phoenix_

#### What's New Since 1.0rc5

* Updated README.
* Updated getting_started.txt.
* Handle :do in a method call properly
* Simplify qsort to using simplified and working algorithm

* Simple hack to allow using sqlite3 1.3.0
* Include RUBY_EXTCONF_H. Fixes #281.
* Use _exit() rather than exit() to avoid C++ finalization races
* Catch and handle a redo passing through a rescue properly (unicorn)
* Don't use Class#ancestors internally
* Clean up test directory

* Fixed proto for rb_define_alloc_func
* Restrict configure to only using LLVM 2.6 from the system
* Add rb_big2ull
* Update version number for 1.0.0! Break out the champagne!

#### Highlights

* Supports a huge amount of Ruby code and popular C extensions:
  * Rails 3 and 2.3.5
  * Sinatra
  * sqlite3, mysql, nokogiri, yajl-ruby
  * Much, much more!
* JIT compiler to accelerate running Ruby code.
* Generational Garbage Collector.
* Integrated profiler.

#### Known Issues

* String#unpack and Array#pack are currently pretty slow. We're working
  on this issue and will release a fix in the near future.
* A number of String methods are a bit slower than in 1.8. Most of these are
  known and being addressed.
* We've added much of the MRI C-API for running Ruby extensions, but
  there are some extensions that don't work yet. Typically, this is because
  the extension uses RBASIC(), RHASH(), or RREGEXP(). These depend on MRI's
  internal object memory layout. In most cases, there are regular functions
  available to use that do not depend on MRI internals. We will help C
  extension authors to make their code portable.  Additionally, extensions
  that fail due to a missing rb_* function should be reported in the issue
  tracker so that we can add the functions.
* Neither the builtin debugger nor ruby-debug currently work. The debugger
  is a high-priority item to work on post 1.0.
* The set_trace_func hook and the C event hooks are not supported. This
  means that rcov currently does not work. To profile your code, use our
  builtin profiler with the
      **-Xint -P**
    options on the command line.
