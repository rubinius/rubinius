---
layout: post
title: Profiling JIT-ted Ruby code with OProfile
author: Ryo Onodera
---

You can now profile JIT-ted Ruby code!

Before:

    samples  %        linenr info                 app name                 symbol name
    497      28.7117  instructions.cpp:82         rbx                      rubinius::MachineCode::interpreter(rubinius::State*, rubinius::MachineCode*, rubinius::InterpreterCallFrame*)
    460      26.5742  (no location information)   anon (tgid:4118 range:0xb581c000-0xb589c000) anon (tgid:4118 range:0xb581c000-0xb589c000)
      # => The dark world of JIT-ted Ruby code
    158       9.1277  jit_util.cpp:787            rbx                      rbx_set_local_depth
    113       6.5280  block_environment.cpp:365   rbx                      rubinius::BlockEnvironment::call(rubinius::State*, rubinius::CallFrame*, rubinius::Arguments&, int)
    102       5.8925  (no location information)   rbx                      __x86.get_pc_thunk.bx
    93        5.3726  jit_util.cpp:848            rbx                      rbx_push_local_depth
    71        4.1017  variable_scope.cpp:108      rbx                      rubinius::VariableScope::set_local(int, rubinius::Object*)
    68        3.9284  block_environment.cpp:62    rbx                      rubinius::BlockEnvironment::invoke(rubinius::State*, rubinius::CallFrame*, rubinius::BlockEnvironment*, rubinius::Arguments&, rubinius::BlockInvocation&)
    52        3.0040  variable_scope.cpp:126      rbx                      rubinius::VariableScope::get_local(int)
    47        2.7152  variable_scope.cpp:100      rbx                      rubinius::VariableScope::set_local(rubinius::State*, int, rubinius::Object*)
    45        2.5997  kind_of.hpp:153             rbx                      bool rubinius::kind_of<rubinius::Fiber>(rubinius::Object const*)
    24        1.3865  shared_state.cpp:295        rbx                      rubinius::SharedState::checkpoint(rubinius::ManagedThread*)

After:

    samples  %        linenr info                 app name                 symbol name
    3468     27.0388  instructions.cpp:82         rbx                      rubinius::MachineCode::interpreter(rubinius::State*, rubinius::MachineCode*, rubinius::InterpreterCallFrame*)
    3048     23.7642  loop.rb:7                   5060.jo                  _X_Object#forever$block@1
      # => Wow, the name of JIT-ted Ruby code is shown along side C++ function symbols!
    1044      8.1397  jit_util.cpp:848            rbx                      rbx_push_local_depth
    947       7.3834  (no location information)   rbx                      __x86.get_pc_thunk.bx
    916       7.1417  jit_util.cpp:787            rbx                      rbx_set_local_depth
    781       6.0892  block_environment.cpp:365   rbx                      rubinius::BlockEnvironment::call(rubinius::State*, rubinius::CallFrame*, rubinius::Arguments&, int)
    639       4.9821  block_environment.cpp:62    rbx                      rubinius::BlockEnvironment::invoke(rubinius::State*, rubinius::CallFrame*, rubinius::BlockEnvironment*, rubinius::Arguments&, rubinius::BlockInvocation&)
    421       3.2824  variable_scope.cpp:108      rbx                      rubinius::VariableScope::set_local(int, rubinius::Object*)
    389       3.0329  variable_scope.cpp:126      rbx                      rubinius::VariableScope::get_local(int)
    326       2.5417  variable_scope.cpp:100      rbx                      rubinius::VariableScope::set_local(rubinius::State*, int, rubinius::Object*)
    288       2.2454  shared_state.cpp:295        rbx                      rubinius::SharedState::checkpoint(rubinius::ManagedThread*)
    283       2.2065  kind_of.hpp:153             rbx                      bool rubinius::kind_of<rubinius::Fiber>(rubinius::Object const*)

In short, you can tell how the machine code that the Rubinius JIT generates
performs.

Not satisfied yet? Even annotated profile is supported! That means you can even
know how much time it spends on each line of Ruby code or even on each CPU
instruction:

    /* 
     * Total samples for file : "/tmp/loop.rb"
     * 
     *   5658 19.9436
     */


       718  2.5308 :def increment(i)
       482  1.6990 :  i + 1
                   :end
                   :
                   :def forever
                   :  i = 0
      2933 10.3384 :  loop do /* _X_Object#forever$block@1 total:   6990 24.6387 */
      1525  5.3754 :    i = increment(i)
                   :  end
                   :end
                   :
                   :forever

We're using [OProfile](http://oprofile.sourceforge.net/news/), a profiling
software. In this blog post, I'll show you how to profile using it!

### What's OProfile?

It's a very useful profiling tool available on Linux. It's a sampling-based
one. That means there is absolutely no change to Rubinius and your Ruby code to
profile. Just run it as you normally do. Also, the overhead of profiling is
minimal.

This is contrasted to measuring-based profiling. Rubinius's built-in profiler
and ruby-prof are both examples of measuring profilers. This means the overhead
is much bigger and can skew results because of that.

Basically, it works by reporting how many individual profiled items are sampled
compared to the overall total samples. It doesn't measure elapsed time. It's
much like top command's individual processes' CPU % usage with far greater
flexibility. The actual profiled items can be any of C/C++ libraries, C/C++
functions, C/C++ source code lines, or CPU instructions.

So, OProfile can't usually profile Ruby code because it works on CPU
instruction level. From OProfile, it can't tell which Ruby source code line
Rubinius currently executing from the CPU instructions. However, it can profile
JIT-ted Ruby code because Rubinius compiles it very down into the CPU
instructions by definition.

OProfile works as a Linux kernel module. So, it's supported only for Linux.
Sadly, Ubuntu's OProfile and LLVM have bugs relating to this feature.
Apparently, there is no one using this. So you can say, we are really on the
cutting edge. ;)

Anyway, we must work around these problems. But how? There are a few options.
:)

### Setup (PPA: the super simple way; Ubuntu 12.10 only)

I prepared a [PPA](https://launchpad.net/~ryoqun/+archive/ppa) just for you.
Add it to your system. To be specific, run this:

    $ sudo add-apt-repository ppa:ryoqun/ppa
    $ sudo apt-get update
    $ sudo apt-get install oprofile llvm-3.1
    $ sudo apt-get dist-upgrade # Upgrade preinstalled libllvm3.1 to the PPA

By default, Rubinius doesn't use system-provided LLVM, so re-configure Rubinius
to use it and re-build Rubinius:

    $ cd /path/to/rubinius-git-repository
    $ rake clean
    $ ./configure --llvm-config llvm-config-3.1
    $ rake

Done!

### Setup (manual build: the hard way)

If you really want to build LLVM and OProfile manually or if you're using an
other Linux distribution and the distribution doesn't provide OProfile-enabled
 LLVM packages, manually build LLVM and OProfile like this:

(I tested this on Ubuntu 12.10. Minor adjustments may be needed to build on
your environment)

Build and Install OProfile:

    $ sudo apt-get build-dep oprofile # do equivalent thing on your distro.
    $ cd /path/to/working-dir-to-build-things
    $ wget http://prdownloads.sourceforge.net/oprofile/oprofile-0.9.8.tar.gz
    $ tar -xf oprofile-0.9.8.tar.gz
    $ cd oprofile-0.9.8
    $ ./autogen.sh
    $ ./configure --prefix /usr # LLVM has a build issue. So install to /usr
    $ make
    $ sudo make install
    $ adduser oprofile # this is needed for profiling JITted code
    $ opreport --version
      # => opreport: oprofile 0.9.8 compiled on Mar  8 2013 00:57:08

Build LLVM with OProfile support enabled and rebuild Rubinius:

    $ sudo apt-get build-dep llvm # do equivalent thing on your distro.
    $ cd /path/to/working-dir-to-build-things
    $ wget http://llvm.org/releases/3.2/llvm-3.2.src.tar.gz
    $ tar -xf llvm-3.2.src.tar.gz
    $ cd llvm-3.2.src
    $ ./configure --enable-optimized --disable-assertions --with-oprofile
    $ make
    $ sudo make install

If the compilation of `OProfileWrapper.cpp` fails like this:

    OProfileWrapper.cpp: In member function ‘bool llvm::OProfileWrapper::checkForOProfileProcEntry()’:
    OProfileWrapper.cpp:141:62: error: ‘read’ was not declared in this scope
    OProfileWrapper.cpp:142:24: error: ‘close’ was not declared in this scope

Apply this patch, then, re-`make` and continue:

    diff --git a/lib/ExecutionEngine/OProfileJIT/OProfileWrapper.cpp b/lib/ExecutionEngine/OProfileJIT/OProfileWrapper.cpp
    index d67f537..7c0d395 100644
    --- a/llvm-3.1-3.1/lib/ExecutionEngine/OProfileJIT/OProfileWrapper.cpp
    +++ b/llvm-3.1-3.1/lib/ExecutionEngine/OProfileJIT/OProfileWrapper.cpp
    @@ -29,6 +29,7 @@
     #include <dirent.h>
     #include <sys/stat.h>
     #include <fcntl.h>
    +#include <unistd.h>
     
     namespace {
     

Phew, finally rebuild Rubinius!:

    $ cd /path/to/rubinius-git-repository
    $ rake clean
    $ rm -rf vendor/llvm # If you build Rubinius with vendorized LLVM.
    $ ./configure
    $ rake

### Start OProfile

OProfile can be configured by a command called `opcontrol`, not by a
configuration file, which are common.

    $ sudo opcontrol --deinit
    $ sudo modprobe oprofile timer=1      # Needed only inside VirtualBox VMs
    $ echo 0 | sudo tee /proc/sys/kernel/nmi_watchdog
    $ sudo opcontrol --no-vmlinux
    $ sudo opcontrol --separete all
    $ sudo opcontrol --start
    $ sudo opcontrol --dump               # Flushes buffered raw profile data
    $ opreport --merge all --threshold 1  # Prints the report of profile

Setup is complete if you see an output from `opreport` like this:

    CPU: CPU with timer interrupt, speed 1858.39 MHz (estimated)
    Profiling through timer interrupt
              TIMER:0|
      samples|      %|
    ------------------
        92127 67.6157 no-vmlinux
        21920 16.0880 rbx
         7950  5.8348 libc-2.15.so
         4068  2.9857 runner
         3429  2.5167 libstdc++.so.6.0.17
         2139  1.5699 cc1
         1416  1.0393 vm

Congratulations!

### Run Ruby code!

To annotate Ruby code correctly, your current directory must be the top
directory of the Rubinius git repository:

    $ cd /path/to/rubinius-git-repository
    $ ./bin/benchmark ./benchmark/core/hash/bench_access.rb

### Generate profile report

Let's check the profile report of the above benchmark.

NOTE: try to run `opcontrol --dump` at least once while running Rubinius if
JIT-ted Ruby code doesn't show in the profile report. OProfile automatically
dumps periodically, but if your Ruby process terminates too quickly, you
should manually run `opcontrol --dump` while it's still alive.

    $ sudo opcontrol --dump && sudo opjitconv /var/lib/oprofile/ 0 0
    $ opreport --merge all --threshold 1 image:./bin/rbx --symbols --debug-info \
        > /tmp/hash-access-symbols
    $ opannotate --merge all --threshold 0.5 image:./bin/rbx --source \
        > /tmp/hash-access-source

I'll omit but you can generate profile report of annotated assembly by passing
`--assembly` to `opannotate` instead of `--source`.

### Reset current profile

By default, OProfile keeps the profile data indefinitely. To reset it, run
this:

    $ sudo opcontrol --reset

### How to read the profile result

Open `/tmp/hash-access-symbols`, the content should be like this:

    CPU: CPU with timer interrupt, speed 1860.42 MHz (estimated)
    Profiling through timer interrupt
    samples  %        linenr info                 app name                 symbol name
    3571     31.5990  (no location information)   no-vmlinux               /no-vmlinux
    619       5.4774  bench_access.rb:16          8295.jo                  _X_Object#__block__$block@7
    599       5.3004  bench_access.rb:24          8295.jo                  _X_Object#__block__$block@11
    531       4.6987  bench_access.rb:8           8295.jo                  _X_Object#__block__$block@1
    457       4.0439  (no location information)   rbx                      __x86.get_pc_thunk.bx
    318       2.8139  jit_util.cpp:787            rbx                      rbx_set_local_depth
    298       2.6369  bench_access.rb:42          8295.jo                  _X_Object#__block__$block@20
    277       2.4511  object.cpp:362              rbx                      rubinius::Object::hash(rubinius::State*)
    236       2.0883  jit_primitives.cpp:4494     rbx                      jit_stub_object_hash
    236       2.0883  tuple.cpp:36                rbx                      rubinius::Tuple::put(rubinius::State*, int, rubinius::Object*)
    222       1.9644  inline_cache.cpp:696        rbx                      rubinius::InlineCache::check_cache_poly(rubinius::State*, rubinius::InlineCache*, rubinius::CallFrame*, rubinius::Arguments&)
    197       1.7432  jit_util.cpp:848            rbx                      rbx_push_local_depth
    187       1.6547  variable_scope.cpp:126      rbx                      rubinius::VariableScope::get_local(int)
    179       1.5839  compiledcode.cpp:212        rbx                      rubinius::CompiledCode::specialized_executor(rubinius::State*, rubinius::CallFrame*, rubinius::Executable*, rubinius::Module*, rubinius::Arguments&)
    162       1.4335  hash.rb:220                 8295.jo                  _X_Hash#key_index@15
    147       1.3008  hash.rb:325                 8295.jo                  _X_Hash#values_at$block@13
    143       1.2654  object.cpp:400              rbx                      rubinius::Object::hash_prim(rubinius::State*)
    137       1.2123  (no location information)   libc-2.15.so             /lib/i386-linux-gnu/libc-2.15.so
    127       1.1238  variable_scope.cpp:100      rbx                      rubinius::VariableScope::set_local(rubinius::State*, int, rubinius::Object*)
    111       0.9822  inline_cache.cpp:576        rbx                      rubinius::InlineCache::check_cache_reference(rubinius::State*, rubinius::InlineCache*, rubinius::CallFrame*, rubinius::Arguments&)
    108       0.9557  vm.cpp:175                  rbx                      rubinius::VM::new_object_typed_dirty(rubinius::Class*, unsigned int, rubinius::object_type)
    99        0.8760  vm.cpp:207                  rbx                      rubinius::VM::new_young_tuple_dirty(unsigned int)
    97        0.8583  array18.rb:6                8295.jo                  _X_Array#map@12
    93        0.8229  integer.cpp:84              rbx                      rubinius::Integer::from(rubinius::State*, int)
    90        0.7964  hash.rb:137                 8295.jo                  _X_Hash#[]@23
    76        0.6725  array.cpp:205               rbx                      rubinius::Array::set(rubinius::State*, int, rubinius::Object*)
    72        0.6371  linkedlist.cpp:27           rbx                      LinkedList::remove(LinkedList::Node*)
    68        0.6017  tuple.cpp:64                rbx                      rubinius::Tuple::create(rubinius::State*, int)
    57        0.5044  method_primitives.cpp:12327 rbx                      rubinius::Primitives::object_hash(rubinius::State*, rubinius::CallFrame*, rubinius::Executable*, rubinius::Module*, rubinius::Arguments&)

As you can guess, symbol names beginning with `_X_` is the JIT-ted Ruby code.
Many Ruby benchmark blocks are JIT-ted (like
`_X_Object#__block__$block@7` at `bench_access.rb:16`) and listed near the top.
Also, `_X_Array#map@12` is a JIT-ted code of `Array#map`. I'll explain the
format of the report using it as an example:

    97        0.8583  array18.rb:6                8295.jo                  _X_Array#map@12

`97` is the number of counts OProfile found what Rubinius was executing, while
periodically sampling it.

`0.8583` is the percentage to the total number of samples OProfile collected.
Note that summing all entries up doesn't equal to 100%, because `opreport` only
reported the top part of whole profile result (by `--threshold 0.5`).

[`array18.rb:6`](https://github.com/rubinius/rubinius/blob/1d7d7b2e2880478776476089d4dd93fd97aff122/kernel/bootstrap/array18.rb#L6)
is the source location where this method is defined (this is same as
`Method#source_location`).

`8295.jo` is a special `app name` for JIT-ted code. Usually `app name` is the
name of file C/C++ functions reside in (shared libraries or executables).
So, for normal Rubinius' C++ functions, `app name` is just `rbx` because they
reside in `./bin/rbx`. For JIT-ted Ruby code, there can be no meaningful `app
name`, because JIT-ted Ruby code is generated at runtime and they reside in no
file, but only in memory. So, OProfile uses `*.jo` as `app name` in such cases.
And, `8295` means this is profiled when running the process of PID 8295.

`_X_Array#map@12` is the name of symbol for this entry of profile report
(duh!).

Open `/tmp/hash-access-source`, the content should be like this (I'll omit some
unimportant part, because this file is big):

    /* 
     * Total samples for file : "kernel/common/hash18.rb"
     * 
     *   4870 12.8904
     */

    <credited to line zero>     19  0.0503 :
                   :# -*- encoding: us-ascii -*-
                   :
                   :class Hash
                   :
                   :  include Enumerable
                   :
    ...
                   :
                   :  class Bucket
                   :    attr_accessor :key
                   :
       633  1.6755 :    def match?(key, key_hash) /* _X_Hash::Bucket#match?@16     75  0.1985, _X_Hash::Bucket#match?@16     45  0.1191, total:    120  0.3176 */
        40  0.1059 :      case key
       218  0.5770 :      when Symbol, Fixnum
       319  0.8444 :        return key.equal?(@key)
                   :      end
                   :
                   :      @key_hash == key_hash and (Rubinius::Type::object_equal(key, @key) or key.eql?(@key))
                   :    end
                   :  end
    ...
                   :end
    ...

You might wonder why `Hash::Bucket#match?` has so many counts while this didn't
appear in the report for symbols. The reason is that the method is inlined to
each its callers (maybe many top-appearing JIT-ted Ruby blocks). How cool this
is! You can really know which Ruby code is actually taking too much time.

### My last favor

I wish the official Ubuntu packages are fixed and there is no need to add any
PPA. I want it-just-works. I reported affecting bugs to Ubuntu's bug tracking
system. So, please vote it up for fixing (vote them with "affects you?",
please!!):

- [Bug 1154025](https://launchpad.net/bugs/1154025) Install PIC version of libbfd.a
- [Bug 1148682](https://launchpad.net/bugs/1148682) symbol lookup error: /usr/lib/libopagent.so: undefined symbol: bfd_init
- [Bug 1148529](https://launchpad.net/bugs/1148529) OProfile support is disabled

### Further profiling

There is new profiling tool on Linux: `perf` and `operf` (OProfile's new
command). I don't know them well, but certainly they will be useful for
profiling Rubinius.

As a last note, this is partially based on [Unladen Swallow's
information](http://code.google.com/p/unladen-swallow/wiki/UsingOProfile)
of profiling LLVM-based JIT-ted code by OProfile.

Happy profiling!
