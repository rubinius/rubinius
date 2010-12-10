---
layout: doc_en
next: Getting Started
next_url: getting-started
---

## What is Rubinius

Rubinius is an implementation of the [Ruby programming
language](http://ruby-lang.org).

Rubinius includes a bytecode virtual machine, Ruby syntax parser, bytecode
compiler, generational garbage collector, just-in-time (JIT) native machine
code compiler, and Ruby Core and Standard libraries.

Rubinius currently implements Ruby version 1.8.7.


## License

Rubinius uses the BSD license. See the LICENSE file in the source.


## Installing

Rubinius runs on Mac OS X and many Unix/Linux operating systems. Support for
Microsoft Windows is coming soon.

To install Rubinius, use the following steps. For more detailed information,
see [Getting Started](/doc/en/getting-started/).

1. `git clone git://github.com/evanphx/rubinius.git`
1. `cd rubinius`
1. `./configure --prefix=/path/to/install/dir`
1. `rake install`

When the install process finishes, follow the directions to add the Rubinius
executable (bin) directory to your PATH.

Rubinius comes with RubyGems built-in and has rake and rdoc gems
pre-installed. To install the nokogiri gem, for example, run `rbx gem install
nokogiri`.
