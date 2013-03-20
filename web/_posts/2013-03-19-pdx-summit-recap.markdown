---
layout: post
title: PDX Summit Recap
author: Brian Shirai
---

Last week, Dirkjan spent the week with me at the Engine Yard office in
Portland, OR. We wanted to discuss a variety of topics related to Rubinius
technology. We started the week by making the following list:

* Inline cache
* JIT IR
* Performance of JIT'd code
* Extension API
* Concurrency and garbage collection

We spent the week mostly writing a ton of things on the whiteboard, discussing
them, erasing them, and writing a bunch more. Ultimately, we touched on all
the topics but we did not get to discussing the extension API in depth, so
we'll skip that one for now. I've [previously
blogged](http://rubini.us/2013/03/13/welcome-dirkjan/) about our inline cache
discussion. Here I'll summarize the rest of our discussions.


## Just-in-time Compiler

The just-in-time (JIT) compiler that converts our virtual machine bytecode to
machine code is a fundamental part of making Ruby fast. It is also one of the
most complex components of Rubinius. It requires interfacing with the virtual
machine to get runtime data and coordinating with the garbage collector to
keep other runtime data structures consistent.

We are able to leverage [LLVM](http://llvm.org) for especially difficult parts
of machine code generation, like instruction selection, instruction scheduling
and register allocation. However, the semantics of Ruby are so removed from
those of the underlying CPU that we have a lot of work we still need to do to
improve the quality of the machine code.

One important component of this work is the ability to represent operations
that have Ruby semantics (but are not part of Ruby itself) in a way that we
can perform transformations on those operations. The "not part of Ruby" is
actually a bit fuzzy. The concept of object allocation is certainly a part of
Ruby. But the specific operation of reserving a region in the heap is
invisible to Ruby.

There are a couple operations in particular that are critical to Ruby
performance, especially when emitting machine code from the JIT. Those
operations are type guards and object allocation. If our assumptions for the
particular type of a value hold, we want to emit the fewest number of guards
as possible and we want the largest contiguous sequence of machine
instructions that are valid under a specific guard.

Likewise, if an object is allocated but does not get passed outside of the
code sequence we are compiling to machine code, and we can see there are no
other side effects, we may be able to avoid allocating the object. A good
example of this in Rubinius, where the core library is implemented in Ruby, is
the ByteArray object that is used by String. A ByteArray is a fixed vector of
8-bit bytes. If an intermediate operation on a String creates a ByteArray and
that object isn't used outside the JIT'd method, the JIT should be able to see
through the object creation and recognize that just some machine bytes are
being moved around or transformed. The ByteArray allocation can be elided.

These two examples, type guards and object allocation, are just a couple
things we expect the JIT to do well. There are many others.

Right now, our JIT converts the virtual machine bytecode directly into LLVM
IR. To support more sophisticated JIT transformations, we need a
representation in between bytecode and LLVM IR, which is one of the things we
will be working on.

Another aspect of the JIT that needs an overhauls is the infrastructure around
running the JIT. It runs on a separate native thread with few synchronization
points. Since Rubinius has no global lock, and since we need a variety of
auxiliary threads for system operations (e.g. handling process signals,
running object finalizers, handling debugger events), we have some
infrastructure for handling the lifetimes of these auxiliary threads. This is
a particularly complicated aspect of Rubinius because when Ruby code calls
`fork()` or `exec()`, the Ruby code knows nothing about these auxiliary
threads. We have to be careful that state is maintained across these calls.
This component will be enhanced to better handle auxiliary threads that are
actually proper Ruby threads (all auxiliary threads are fundamentally Ruby
threads but not necessarily visible in Ruby). This will permit us to have
multiple, potentially completely different, JIT threads running
simultaneously.


## Performance

For us as people, when our assumptions fail we often just blunder on making
fools of ourselves. Too bad we don't have a "check your assumptions" mode as
useful as the one we build into the JIT. In the JIT, if our assumptions fail
and we don't catch it, we're going to compute the wrong values or cause the
wrong actions to be performed.

So, in the JIT, when our assumptions are wrong, we must change course. This
course change may be to go down a different path in the machine code or to
exit the machine code entirely and go back to the interpreter. Both paths have
significant performance implications.

A reason to exit the machine code is that one significant aspect of generating
fast machine code is proper use of the type guards. If our assumptions about
the type of a value are wrong, simply exiting back to the interpreter means
that we don't pollute the LLVM IR with types that can inhibit LLVM
optimizations from being as effective. On the other hand, there are runtime
costs associated with both exiting to the interpreter and running the
interpreter instead of machine code. Also, if we exit the generate machine code
too often, we may trigger a de-opt, throwing away that version of the machine
code and requiring the JIT to create a new one.

Another aspect of the performance of the machine code we emit is how we handle
virtual machine operations. Some aspects of running Ruby require very
low-level operations that we call primitives. These are coded in C++ and
interact with internal virtual machine facilities. When compiling machine
code, we want as few conditionals and as few function calls as possible to get
the highest performance. Typically, we call a function that performs the
primitive operation and then returns to the JIT'd machine code. As we move
more and more things into Ruby itself, it is possible for the JIT to inline
operations and get the greatest possible optimizations. This is another area
we will be working on as we improve the JIT.


## Concurrency

One of the goals for Rubinius since the very beginning is to provide excellent
support for concurrency and parallelism in Ruby. We have already removed the
global interpreter lock (GIL/GVL) from Rubinius. However, we have to ensure
that we don't replace it with an effective global lock when we implement
components of the system.

The key to improving lock-freedom throughout the system is to use less locks.
That may seem obvious, but it is not necessarily that simple. As I mentioned
above, the virtual machine actually uses a variety of threads internally. And
with no global lock, any Ruby thread could invoke any virtual machine
operation, like `fork()`, at any time. Multiple Ruby threads could be calling
the same method on the same object at the same time. Considering my previous
post about inline caches, this implies that all sorts of data races and
concurrency bugs could arise just from calling Ruby methods.

In the virtual machine, we must be careful to ensure correctness under
concurrent execution. The classical way of doing so is to use locking.
However, there is tons of research from the '90s about lock-free and wait-free
algorithms. This academic research is slowly trickling down to industry in
various domains. We will be using lock-free implementations of various virtual
machine data structures to improve support for concurrency and parallelism.


## Garbage Collection

One particularly challenging area that requires careful control of concurrency
is the garbage collector. Rubinius provides a generational garbage collector
with very short pause times when collecting the young generation. The
generational architecture allows us to only collect a small region of the heap
instead of the whole heap every time. We also support object allocation using
thread-local buffers to avoid locking and contention when Ruby code is running
in parallel.

However, the garbage collector architecture pauses all threads to perform a
collection. While the pause for a young generation collection is on the order
of a few milliseconds (5-15 in many cases), there is the overhead of forcing
all the various threads to pause.

There is only one way to address this issue and that is to make the garbage
collector concurrent. Fortunately, we can achieve this with some modifications
to our existing garbage collector. When adding concurrency to the garbage
collector, we want to avoid slowing down non-garbage-collector operations,
typically referred to as mutator performance. (Operations in the system can be
viewed as partitioned into two sets: the heap management--allocation and
garbage collection, and the operations that mutate the heap--mutator
operations.)

With modifications to our mature generation collector (based on the Immix
collector), we can perform partial heap collections and preserve the
generational benefits while also limiting stop-the-world pauses to specific
regions of the heap and bounding the pause times.


## Semantic Versioning

[Semantic Versioning](http://semver.org/) is a methodology for versioning
software to support correct dependency management. MRI has never had
particularly strong support for versioning, with semantic changes being added
in patch levels (not even visible in the RUBY_VERSION constant) and major
version increments on special dates completely devoid of semantic relevance.

In Rubinius, we would like to provide a more sound versioning discipline, in
particular, semantic versioning. However, this raises some significant
challenges. Let's consider some of the components of Rubinius:

* Ruby language itself in one of three language modes: 1.8, 1.9, and 2.0
* Ruby core classes
* Ruby standard library
* MRI compatibility "C-API" for supporting C-extensions
* Supporting core classes like ByteArray and Tuple
* Bytecode compiler
* Virtual machine instruction set
* Internal APIs for JIT, garbage collection, concurrency management

The key component on which semantic versioning is based is the public API. But
what is included in that? Is it just Ruby as covered in RubySpec? Is it also
our supporting Ruby classes or Ruby facilities like the parser and bytecode
compiler? Since Rubinius is a language platform used to implement other
programming languages besides Ruby, resolving these questions correctly is
important.

Along with a sound versioning discipline, we also want to promote continuous
delivery of enhancements to Rubinius. Technically, these are compatible goals,
but require that we consider how to organize our development and source
branches when we create backward-incompatible API changes.

As we work toward the Rubinius 2.0 release, we will do our best to implement
semantic versioning. You can help us by testing your applications, gems, or
libraries on Rubinius. One great place to do this, of course, is [Travis
CI](https://travis-ci.org/).


## All Work and No Play

Portland is a beautiful city. It's not very big but has enough city things to
feel like a proper city. I didn't want to keep Dirkjan caged in the office all
day every day, so when the weather looked promising on Friday, we packed our
concurrency and garbage collection conversation up and headed out. First we
went to [Portland State University](http://www.pdx.edu/) to attend a talk at a
programming language club. The talk was on continuation-passing style,
defunctionalization, and transformations. It was somewhat related to our
compiler work and it was certainly interesting. But my real motive was to
introduce Dirkjan to my former coworker [Larry
Diehl](https://twitter.com/larrytheliquid), who is a PhD student at PSU doing
fascinating work in dependently typed programming language theory.

After the talk at PSU, we hopped on the MAX light rail and headed toward the
[Japanese Garden](http://japanesegarden.com/). The bus that would take us the
rest of the way wasn't due for 25 minutes so we decided to walk. The garden is
located in Portland's [Washington Park](http://washingtonparkpdx.org/), which
is located in the west hills of Portland. So, we walked, uphill, for quite a
while. We walked past the Lewis and Clark monument, through the [international
rose garden](http://www.rosegardenstore.org/), past the tennis courts and
finally up another steep hill to the Japanese Garden.

The Japanese Garden is one of the most beautiful and authentic outside of
Japan. We are very lucky to have it here and my wife and I visit it frequently
to enjoy the beautiful views and bask in the tranquility. Since it is early
spring, not many plants were blossoming yet, but we saw some huge koi, some
water falls, beautiful trees, and a stunning view of Portland. Unfortunately,
Mt Hood was hiding in the clouds that day.

I'm not much of a photographer, but here are some pics from the trip.

![Dirkjan photographing from a bridge](http://farm9.staticflickr.com/8516/8573161804_69e037f173_c.jpg)
![Koi!](http://farm9.staticflickr.com/8367/8573160544_466e14d2ec_c.jpg)
![Plants!](http://farm9.staticflickr.com/8378/8572067073_a0555e44bd_c.jpg)
