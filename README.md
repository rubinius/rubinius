[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/rubinius/rubinius?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge) [![Build Status](https://travis-ci.org/rubinius/rubinius.svg?branch=master)](https://travis-ci.org/rubinius/rubinius)

# The Rubinius Language Platform

Rubinius is a modern language platform that supports a number of programming languages.

The philosophy and high-level architecture of Rubinius are described below.

Rubinius runs on macOS and many Unix/Linux operating systems. Microsoft Windows is not supported.

## Code of Conduct

Participation in the Rubinius project is governed by the Rubinius [Code of Conduct](https://rubinius.com/code-of-conduct/).

## Issues & Support

Please [file tickets](http://github.com/rubinius/rubinius/issues) for bugs or problems.

For additional help, visit the [Rubinius Gitter chat room](https://gitter.im/rubinius/rubinius).

## Contributing

We welcome contributions. For more information, see the FAQ below and read the [contributing](https://github.com/rubinius/rubinius/blob/master/.github/contributing.md) file.

## License

All source code in this repository is subject to the terms of the Mozilla Public License, version 2.0 unless stated otherwise. A copy of this license can be found the file "LICENSE" or at <https://www.mozilla.org/MPL/2.0/>.

Contributions made prior to January 3rd, 2016 are licensed under the old BSD 3-clause license. A copy of this license can be found in the file "BSD_LICENSE".

In the event a directory contains a "LICENSE", "LICENSE.txt" or "COPYING" file the license specified in said file applies to the contents of said directory and all sub directories, overwriting the licenses specified above.

## Installing Rubinius

To install Rubinius from source, use the following steps:

1. Ensure that MRI 2.0+, rubygems, rake, and git are installed
2. `git clone git://github.com/rubinius/rubinius.git`
3. `cd rubinius`
4. `./configure --prefix=/path/to/install/dir && make install`

When the install process finishes, follow the directions printed to the terminal to add the Rubinius executable (bin) directory to your PATH.

Certain setups of openssl require you to pass along additional options, `--with-lib-dir` and `--with-include-dir`
pointing to include the relevant openssl directories. 

E.g If you are installing via RVM / homebrew on OS X.
```
rvm install rbx-3 -C \
  --with-lib-dir=$(brew --prefix openssl)/lib \
  --with-include-dir=$(brew --prefix openssl)/include
```

*See https://book.rubinius.com/manuscript/getting_rubinius.html
for a more complete guide to installing Rubinius from source.*

## Philosophy & Architecture

Rubinius is a virtual computing machine. As a computing machine, it possesses several mechanisms that physical computers possess to support computation.

It runs in an environment that provides various capabilities. These are presented to the machine through the Environment object.

It has certain configuration values that influence how the machine operates. This is managed by the Configuration object.

The machine has a boot process that carefully builds up the components in a precise order until the machine is ready to run user code. When the user code completes executing, the machine halts, again following a precise sequence of deconstructing the various components.

The rest of the components of the machine are described below.

### Instructions

The Rubinius instruction set includes different categories of instructions. One key aspect of the Rubinius philosophy is that the instruction set should represent any machine semantic. This contrasts with the way Rubinius has been implemented historically, which relied heavily on C++ "primitives" modeled on the Smalltalk 80 virtual machine. All the primitives are being replaced as the instruction set evolves richer capabilities.

The categories of instructions are:

1. *Stack* instructions that push, pop, and get operands from a stack;
2. *Register* instructions that read from and write to a set of registers;
3. *Assertion* instructions that can halt execution but cannot change the semantics of the program;
4. *Instrumentation* instructions that cannot change the semantics of the program but can emit data from the program;
5. *Parsing Expression Grammar* (PEG) instructions. PEGs are an elegant formalism for describing recognizers;
6. *Foreign Function Interface* (FFI) instructions for binding and calling external functions;
7. *Concurrency* instructions for execution threads and concurrent data;
8. *System* instructions for accessing files, directories, and other systam capabilities.

**TODO: There's plenty of places to help out here if parsers, compilers, and instruction sets interest you.**

### Heaps & Garbage Collector

Rubinius has two kinds of managed objects: object-oriented ones that can support inheriting from a superclass, and data objects that have no concept of object-orientation.

Rubinius has three concepts for heaps, the space where managed objects live:

1. The *open* heap is one where any object in the heap can contain a reference to any other object. Think normal Ruby land;
2. The *closed* heap is one where an object in the heap can contain a reference to an object outside the closed heap, but no object outside can contain a reference to an object in the closed heap;
3. The *isolated* heap is one where no object in the isolated heap can contain a reference to an object outside the heap, and no object outside can contain a reference to an object in the isolated heap.

Threads that use isolated heaps can execute fully independent of any other thread and only must synchronize with the process during boot, fork, and halt. The garbage collector for isolated heaps is run in that thread.

Rubinius uses a single mechanism for garbage collection, the [Immix mark-region collector](http://www.cs.williams.edu/~dbarowy/cs334s18/assets/immix-pldi-2008.pdf).

The Rubinius garbage collector currently runs on a single separate thread and must fully synchronize all threads that mutate managed memory (ie it stops the world). In the future, the single open heap will introduce a second generational area, and the isolated heaps will provide for parallel collection.

**TODO: There's plenty of places to help out here if garbage collectors interest you.**

### CodeDB

The Rubinius CodeDB is where all compiled code for the core and standard libraries is stored. Every executable context (in Ruby, these are scripts, class & module bodies, methods, and blocks) has a unique ID and is cached in the CodeDB. In the future, all user code will also be cached in the CodeDB.

The unique ID of every executable context allows for associating arbitrary dimensions of data with that instance of executable code. For example, type information seen at run time, profile and coverage information, call graphs, and memory allocations data can all be associated with the executable code.

**TODO: There's plenty of places to help out here if databases and code analytics interest you.**

### Console

The Console is an interprocess communication (IPC) mechanism. In contrast to Ruby's IRB, which executes in the same process, the Rubinius Console is intended to be a general purpose mechanism to interact with the virtual machine. The capabilities of the Console include the ability to execute code and return the result, start/step/stop the Debugger, start/stop the Profiler, access data from the Profiler and Diagnostics, and fetch the call graph and object graph data.

**TODO: There's plenty of places to help out here if developer tools interest you.**

### Debugger

The Debugger uses instruction replacement (ie substituting the debug instruction for the existing instruction at that point in the instruction sequence) to cause an executing instance of code to stop and allow you to inspect values and step to the next instruction, or step into or out of another instance of code (eg a method, function, or block).

**TODO: There's plenty of places to help out here if developer tools interest you.**

### Profiler

The Profiler is a randomized-interval sampling profiler that is always running. At a randomized interval, the code instances that are currently executing in a thread have their sample count incremented. In addition, all code instances have a call count that is incremented each time they are invoked. At every call site, the number of times the call site executes is tracked for each type of object seen. The call sites enable deriving the call graph for the program from the object graph because the call sites have a normal (Ruby) object interface.

**TODO: There's plenty of places to help out here if developer tools interest you.**

### Diagnostics & Logging

The Diagnostics facility provides metrics on all components of the virtual machine. The logging facility includes different log levels (ie debug, info, warn, error) and provides a descriptive account of virtual machine lifecycle events.

**TODO: There's plenty of places to help out here if developer tools interest you.**

### Machine-code Compiler

The machine-code compiler is based on LLVM and compiles a managed code instance to machine code. The first generation Rubinius JIT (just-in-time compiler) included type inference, custom code passes, and inlining facilities all implemented in C++. The next generation compiler uses a [nanopass](http://nanopass.org) architecture and builds a single managed code instance that can be compiled to native machine code.

**TODO: There's plenty of places to help out here if native machine code compilers interest you.**

### Data Types & Functions

Rubinius has functions. No, really.

  ```
  $ bin/rbx compile -N plus -B -e 'fun plus(a, b) a + b end'

  ================ :plus =================
  Arguments:   2 required, 0 post, 2 total
  Arity:       2
  Locals:      2: a, b
  Stack size:  4
  Registers:   0
  Literals:    1: :+
  Line:        1
  Lines to IP:

  0000:  push_local                 0    # a
  0002:  push_local                 1    # b
  0004:  send_stack                 :+, 1
  0007:  unwind                     0
  0009:  ret                        0
  ----------------------------------------
  ```

There are several ways that types can be added. First, it's important to distinguish the "behavior" of object-oriented code from the "types" of data. See [Objects are for interactions, functions are for data](https://medium.com/metalanguage/objects-are-for-interactions-functions-are-for-data-936e044cc729).

For objects, instead of types, we want to be able to easily convey that objects should represent themselves differently:

  ```ruby
  class A
    def m(a: Integer(2), b: Integer(3))
      # If the values passed for a and b are not Integers,
      # the Integer() constructor will be called on them.
      # If a value isn't passed for a, the default value is 2.
      # Similarly for b, the default value is 3.
    end
  end
  ```

For functions, the situation is similar, but different:

  ```ruby
  type :int, fun +(a: int, b: int)
    # The type of a and b must be machine integers.
    # The return type is specified by an annotation
  end
  
  fun +(a: int, b: int, return: int)
    # The type of the return value is int.
    # The 'return' argument is elided.
  end
  
  fun +(a: int, b: int): int
    # This form would require modifying the parser.
  end
  ```

Each of these three represent a possibility for implementing types, including the return type for a function. The form that will be implemented hasn't been chosen yet.

Both namespacing and explicit data definition (using either `type` or `data`) are not implemented yet, but are definitely being considered. Examples of these would be:

  ```ruby
  namespace my_funcs
    data size_cat
      value = 'big' | 'small'
      
      fun big?
        value == 'big'
      end
      
      fun small?
        value == 'small'
      end
    end
  end
  
  a = my_funcs::size_cat('big')
  my_funcs::size_cat::small?(a)  # => false
  ```
  
Functions became particularly interesting when they can be co-mingled with object-oriented code. In Rubinius, the lexical scope is represented by a Ruby object much like any other Ruby object. Since Ruby does not contain language features for manipulating the lexical scope, it's a natural place to stash functions so that lookup seems unsurprising.

  ```ruby
  class A
    import "my_funcs"
    
    def m(a, b)
      if small?(a) and big?(b)
        puts "We have a mixed mode"
      end
    end
  end
  
  A.new.m('small', 'big')
  "We have a mixed mode"
  ```

In Ruby, any method call that does not explicitly use a receiver could resolve to a function that has been imported to that lexical scope or an exclosing lexical scope.

While the functions and types described in this part are experimental and may or may not actually exist when you read this, the ideas are legitimate. If you are really excited by these features, let us know.

**TODO: There's plenty of places to help out here if functions and data types interest you.**

### C-API

The C-API provides an element of compatibility with Ruby C-extensions. However, the C-API is deprecated and will likely eventually be removed.

## FAQ

**Q. There's this other \<programming language, project, concept, application> that seems \<better, faster, cheaper>, shouldn't I use that instead?**

A. Yes, absolutely. The sooner the better, really.

**Q. Why are there no tickets tagged for beginners?**

A. We have a lot of respect for your abilities, whether you've ever written a line of code before or not, whether you've ever worked on a compiler, virtual machine, garbage collector, or debugger before.

  Find something that interests you and dive in. If you get stuck, ask a question.

**Q. Why isn't \<my pet feature> done already? When will it be done?**

**A.** Do you have 1,000,000 USD? No, really.

**Q. Why won't you accept my PR to rewrite the virtual machine in \<Rust, Go, Node, TypeScript> or add static typing to the Ruby core library or my other terrific idea?**

**A.** There are a lot of fascinating ideas out there. Fork Rubinius, whip up your idea, show that other people find it useful, and let's talk. You might find you've got a far better project than Rubinius. After all, that's what we're doing, trying to figure out what might be useful.

**Q. Is there more documentation?**

**A.** Yes, there is a [book](https://rubinius.com/book/) that needs a lot of love and attention.

**Q. Can I embed Rubinius into my favorite C/C++ application?**

**A.** Yes, you can! More of the facilities to support interacting with the Machine will be added over time.

  ```c++
  #include "machine.hpp"
  
  int main(int argc, char** argv) {
    rubinius::Machine machine(argc, argv);
    
    machine.boot();
    
    return machine.halt();
  }
  ```
   
**Q. What about the Ruby programming language?**

**A.** Many popular Ruby applications, like Rails, may run on Rubinius, which aims to be compatible with the most recent stable Ruby version.

   Rubinius provides the standard Ruby libraries, with the following exceptions:

  * Continuation
  * Ripper
  * TracePoint
  * Tracer

  The following Ruby features are not supported on Rubinius:

  * Refinements
  * $SAFE levels

**Q. Isn't Rubinius just a Ruby implementation?**

**A.** No, it's not. Rubinius is an experiment that started as a Ruby implementation, but is attempting to look beyond the limitations of Ruby. Consider Rubinius a mostly compatible superset of Ruby for now.

**Q. Why does Rubinius report the Ruby version as 10.0?**

**A.** Rubinius is a time machine. When you use it, you travel into the future. Even this README is in the future.

**Q. What is up with the weird version scheme in Rubinius?**

**A.** Rubinius uses a simple `epoch.sequence` version scheme. For any sequence number `N`, `N+1` will only add new capabilities, or remove something that has been listed as deprecated in `<= N`. Super simple.

**Q. Why does Rubinius not support frozen and tainted?**

**A.** Rubinius has better features; frozen and tainted are considered harmful. To elaborate...

Both frozen and tainted depend on strewing checks throughout the source code. As a classic _weak-link_ system, only one of those checks needs to be misplaced for the guarantees offered by either to fail. Since the number of checks is high, and as new code is written new checks need to be considered, the features inherently constitute unbounded complexity and unbounded risk.

In place of frozen, Rubinius is implementing attributes on classes. One attribute is immutability. The way this works is that every machine instruction has an attribute for whether the instruction would mutate an object. When a method is added to a class, the attributes are checked and any method containing mutating instructions is disallowed. Additionally, dispatch that searches the superclasses of a class marked immutable would also perform this check. Immutability is an indelible attribute that is inherited by all subclasses.

In place of tainted, Rubinius is implementing sanitization functions at the IO boundary, similar to the transcoding facility.

In both cases, the places that the checks must be made are orders of magnitude fewer than in the case of frozen and tainted. The checks are more orderly as well.

**Q. Why doesn't Rubinius allow me to set arbitrary encodings for Strings?**

**A.** Rubinius only uses UTF-8 internally. Any transcoding must be performed at the IO boundary.

**Q. How do I use RubyGems?**

**A.** Rubinius comes with RubyGems built-in. To install a gem, run the following:

    $ rbx -S gem install <gem_name>

**Q. Why doesn't Rubinius install gem binaries in the same place as Ruby?**

**A.** Rubinius is intended to be installed alongside Ruby without causing conflicts. Only the main executable, `rbx`, should be installed into a system directory. Edit your shell PATH to include the directories listed when Rubinius is installed to access other executables like gem binaries.
