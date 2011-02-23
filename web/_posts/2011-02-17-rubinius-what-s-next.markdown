---
layout: post
title: Rubinius, What's Next?
author: Brian Ford
---

On Tuesday, we released version 1.2.1 (see the [Release
notes](http://rubini.us/releases/1.2.1/)). This release weighs in at 256
commits and 21 tickets closed in the 56 calendar days since the release of
1.2.0. Many thanks to those who contributed patches and to everyone who helped
us test it.

While we were working on 1.2.1, we were also working on a Top Secret project
that we've craftily [hidden in plain
sight](https://github.com/evanphx/rubinius/tree/hydra). I'd like to introduce
the work we are doing on the hydra branch and the features you can expect to
see in Rubinius soon.


### Daedalus - A new build system

Rubinius is a fairly complex project. It combines multiple components into a
single system. We have worked hard to contain this complexity and from the
beginning we insisted that building Rubinius be as simple as possible. For
example, Rubinius can be run from the source directory, there is no need to
install it first. Typically, building requires:

    ./configure
    rake

The Rubinius system combines:

1. External libraries written in C/C++, sometimes built with just Makefiles
   and sometimes using autotools.
1. The virtual machine, garbage collector, and JIT compiler written in C++.
1. The virtual machine interpreter instructions, including support code for
   the JIT, and instruction documentation all generated at build time from an
   instruction template.
1. The core library and bytecode compiler written in Ruby.
1. Various C extensions like the Melbourne parser, BigDecimal, Digest, and
   OpenSSL libraries. In the case of the parser, we have to build _two_
   versions, one for the bootstrapping system and one for the Rubinius system
   being built.

It has not been easy to make this work and over the years we have compiled a
list of exactly what we need in a build system. Evan, in typical form, started
hacking out a first pass and created _daedalus_, our new build system. It
features such exotic (and extremely useful) features as SHA-based change
detection, parallel builds, single-process execution, and use-aware
configuration options. Allow me to elaborate.

* A typical build system decides to rebuild a file if the source is newer than
  the build artifact. This can result is unnecessarily building files that
  have been touched by some process (like updating from the source repository)
  but whose contents have not changed.  By basing rebuild decisions on the SHA
  digest of the source, only files that have actually been changed need to be
  built. Since compiling C++ with optimizations is _not_ a fast process,
  eliminating unnecessary building is great for productivity, not to mention
  global warming.
* With a multi-core system, builds can be done faster if they are done in
  parallel. If the build system can accurately determine dependencies, it can
  execute build sub-steps in parallel. Of course, this can cut into YouTube
  and Twitter browsing time, but that's a risk we are willing to take.
* While parallel sub-processes during the build are excellent, the supervising
  process benefits from running as a single process from start to finish.
  Otherwise, configuration data needs to be re-parsed. To support a single
  build process, we need multiple, independent dependency graphs for the
  components. As noted above, we need to build two versions of the parser,
  which requires two dependency graphs _for the same component_! This is
  simply impossible in Rake.
* Use-aware configuration values know that the user has set the value and can
  intelligently merge with newer configuration variables that we create
  without requiring the user to reconfigure. Ultimately, we are aiming for a
  _single_ command build. Just run 'daedalus' and done. There is no step 2.


### Full-on Concurrency

Nobody likes waiting in line. In fact, the more desirable a thing is, the less
we want to stand idly waiting in a line for it, tapping our foot, twiddling
our thumbs. The same could be said about our programs.

Threads give us the ability to add concurrency to our programs. However,
unless the hardware either has multiple CPUs or multiple cores per CPU (or
both), the apparent concurrency will still be executing serially. Since there
are so many multi-core CPUs around these days, our programs should be getting
stuff done in parallel.

Unfortunately, there's a twist. Even with native threads on a multi-core CPU,
the amount of parallelism you get depends on how well you manage locks around
shared data and resources. Sometimes managing these locks is complex and you
opt for one big lock, essentially only allowing one thread at a time to run.
That big lock is usually called a global interpreter lock (GIL) or global VM
lock (GVL).

The Rubinius VM originally had green (user-space) threads, but it has had
native threads with a GIL for a while now. In the hydra branch, Evan and
contributors like Dirkjan Bussink have been working on replacing the GIL with
fine-grained locks so that threads truly execute in parallel. This work has
been going very well, owing in part to the fact that so much code in Rubinius
is actually written in Ruby. Contributors like Chuck Remes have been running
hydra under heavy concurrency loads and Rubinius is performing well.

Rubinius also has experimental support for Fibers and a built-in Actor
library. There is more work to be done but Rubinius is quickly becoming an
excellent platform for concurrency, with a variety of approaches available to
the programmer. Evan has also suggested rewriting the Rubinius IO subsystem to
enable even better APIs for concurrency, all from Ruby.


### Performance

Forget everything anyone has ever told you about Ruby being slow. There are
two things that make Ruby, as implemented, slow: 1) inexperience; 2)
inadequate tools. These two result in one big thing: _doing too much_. Or, as
they say: _No code runs faster than no code_. We have been working for 4+
years to build adequate tools in Rubinius, and there is plenty of experience
in Smalltalk, Self, and other languages for making dynamic languages fast.

Presently, Rubinius typically runs pure Ruby code almost 2 times faster than
MRI 1.9. However, there are also cases where Rubinius is slower. These mostly
involve core libraries that are implemented in C in MRI. There are three main
fronts on which we are attacking performance issues: 1) improving the
algorithms in the Ruby code that implements the core library; 2) continuing to
tune the VM and garbage collector; and 3) improving the JIT compiler. Which
leads me to one of the most exciting things we are working on...


### JIT Intermediate Representation (IR)

The just-in-time (JIT) compiler is the key to making Ruby fast. One of the
biggest challenges with a dynamic language like Ruby is knowing what method is
actually being invoked when a message is sent to an object. Consider the
following code:

{% highlight ruby linenos %}
class A
  def m(x)
    ...
  end
end

class B
  def m(x)
    ...
  end
end

class C
  def work(obj)
    obj.m(y)
  end
end
{% endhighlight %}

What method is being invoked by `obj.m(y)`? There is no way to definitively
know this by looking at the source code. However, when the program is actually
running, we can know precisely what `obj` is and precisely which method `m`
was invoked. This is called type profiling and that is exactly what the Rubinius
VM does. Then the JIT uses the type information to make decisions like whether
to inline a method into another method. When methods are inlined, it gives the
optimizer more data and more possibilities to remove redundant code. The less
code we can run, the faster Ruby will be.

Presently, the JIT compiler converts Rubinius bytecode into LLVM IR and LLVM
handles the thorny task of generating machine code. However, Rubinius bytecode
is designed for fast execution by the virtual machine rather than as a rich
intermediate representation. So Evan has started work on a new JIT IR.

This new IR will help us to express Ruby semantics in a way that enables many
powerful optimizations and will ultimately allow LLVM to generate even better
machine code. Put another way, _Rubinius loves Ruby code!_ Right down to the
metal. There's no fighting a foreign type system or the semantics of a
language at odds with Ruby's rosy view of the world.


### Ruby 1.9

MRI 1.9 introduced two completely different changes to Ruby. The first was a
new implementation based on a bytecode virtual machine. While the virtual
machine replaced the AST-walking interpreter, little else changed
architecturally. Mostly the same core library and garbage collector code
exists in MRI 1.9 as was in MRI 1.8. The second change introduced some new
syntax (minor) and encodings (major). Many of the other changes, for example,
returning Enumerator objects from methods that take blocks, have been
back-ported to Ruby 1.8.7 and are already available in Rubinius.

So, the key to supporting Ruby 1.9 in Rubinius essentially involves supporting
the 1.9 syntax changes and encodings. We have begun implementing the parser
changes and introduced the foundation for Encoding-aware Strings. A good
amount of work remains to be done, but over the next month we expect that
we will be starting to run Ruby 1.9-specific code in Rubinius.


### Tools of Information

It has been said that `printf` is the mother of all debuggers. That
illustrates two points: 1) data is often buried in our program code; and 2) we
should have tools (e.g. a debugger) that enables us to access the data without
manually instrumenting our code.

Presently, Rubinius has a built-in debugger, precise method profiler, memory
analysis tool, and Agent interface that permits querying a running Rubinius
VM--even one running on a remote machine--for a variety of information.

We will be adding the ability to track the location where objects are
allocated to assist finding object leaks or code that is creating unusually
large numbers of objects. We are also working on a tool to graphically display
information like number of running threads, amount of CPU usage, and amount of
memory used while actively monitoring a VM.

I am also curious about correlating this VM information with external data to
enable play-back review.  For example, I would like to monitor RubySpec runs
and correlate which spec is running with the VM data. I imagine a simple
monotonic reference ID provided by the VM would be useful in correlating these
two otherwise unrelated pieces of data. The RubySpec runner would request the
ID before running each spec and the Agent monitor would request the ID when
gathering VM data. Later the two data sets could easily be merged.

When you find yourself manually instrumenting some code, consider what data
you are trying to get your hands on and let us know the scenario.  We'll
likely be able to build a tool that will open up new vistas into the behavior
of your Ruby programs.


### Windows<sup>&reg;</sup>

However one may feel about Windows as an operating system, it is undeniable
that the vast majority of people in the world use Windows. We believe those
people have an inalienable right to use Rubinius, too.

Thanks to the wonderful, hard-working
[MinGW-w64](http://mingw-w64.sourceforge.net/) folks, we are able to compile
the Rubinius VM into a native Windows executable. Presently, the VM will
compile, link, and attempt to load the Ruby core library. More
platform-specific work is needed to load the library. The next step after that
will be getting the RubySpecs to run and start fixing issues.

Since the Windows work is being done on the hydra branch, the other features
discussed above will be available on Windows as soon as we complete them.


### Multi-language-ualization

The Rubinius VM began as an effort to create a modern, first-class environment
for running programs written in Ruby. However, it turns out that Ruby is a
terrific language for writing subsystems for other programming languages.
Actually, this should come as no surprise; Ruby is a fabulous general purpose
programming language.

To support experimenting with writing other languages that run on the Rubinius
VM, Evan has started to put together a Language Toolkit. This includes things
like a built-in PEG parser, convenient ways to create methods from Rubinius
bytecode, and decoupling method dispatch from Ruby semantics.

Hopefully, Evan will introduce us to all this in a future blog post, but
here is a taste of what you can do:

{% highlight ruby linenos %}
class Hello
  dynamic_method :world do |g|
    g.push :self
    g.push_literal "Hello, world"
    g.send :puts, 1, true
    g.ret
  end
end

Hello.new.world
{% endhighlight %}

Of course, that is much more concisely written in Ruby, but combine this
ability with a built-in PEG parser and you can be experimenting with your own
fascinating syntax in a matter of minutes.

Check out the Rubinius [Projects page](http://rubini.us/projects/) for some of
these language experiments. One language in particular is
[Fancy](https://github.com/bakkdoor/fancy), which is fully bootstrapped (i.e.
the Fancy compiler is now written in Fancy) on Rubinius.


### Documentation

One the one hand, Rubinius just runs Ruby code, and you shouldn't need any
special knowledge to run your application on Rubinius. On the other hand, as
I've discussed above, there are some specific Rubinius features that may be
very helpful to you. However, they can only be as helpful as the documentation
we have for them.

Before we released 1.2.0 in December last year, I spent quite a bit of time
getting a new documentation system in place. Since then, we've had
contributors help with translations to Russian, Polish, Spanish, and German.
Adam Gardiner started documenting the garbage collector algorithms. Yehuda
Katz (you may have heard the name) has contributed documentation for [the
bytecode compiler](http://rubini.us/doc/en/bytecode-compiler/) complete with
diagrams!. Chuck Remes wrote up a great piece on the [memory
analysis](http://rubini.us/doc/en/tools/memory-analysis/) tool.

We really appreciate these contributions. We understand the need for great
documentation and we have been creating better support for it. In many cases,
all that is needed is to just open a file and start writing. Of course, one
cannot expect to understand much about Rubinius without digging into the code.
If there is a particular part of Rubinius that you are curious about, jump in
the #rubinius channel on freenode.net and ask us questions. We can point you
in the right direction and help clarify things. If nothing else, let us know
which part of the missing documentation is most important to you and we can
start filling that in.


### How you can help

There you have it, some super exciting things coming very soon for Rubinius
and for Ruby! We would love to have your help making Rubinius even better. The
most important thing you can do is try running your Ruby code. Give us
feedback. Let us know what features or tools would make your life easier. Help
us to build them.

Rubinius adopts Ruby's rosy view of the world. We want to empower you to solve
your hardest problems with Ruby, and have fun doing it.
