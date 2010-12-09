---
layout: release
class: release
---

## Overview

The Rubinius team is proud to announce the availability of 1.1.

1.1 is the first feature release since 1.0, and contains a number of new
features, performance improvements, compatibility adjustements, and bugfixes.

#### Stats

* **Commits:** 546
* **Days since last release:** 112

#### Authors

* Evan Phoenix
* Brian Ford
* Dirkjan Bussink
* Jake Douglas
* Jose Narvaez
* Jeremy Evans
* Ian Leitch
* Paolo Capriotti
* Gavin Stark
* Duncan Mac-Vicar P
* Richard Musiol
* Tim Carey-Smith
* Ivan Evtukhovich
* Chuck Remes
* Konstantin Haase
* Alex Khamutov

## Summary

#### Additions

* Enable block inlining within the JIT
* Implement a new GIL algorithm to prevent starvation
* Add Debugger APIs and reference CLI debugger
* Overhaul finalizers, support for resurecting finalizers
* Basic 'rbx docs' command support.
* Add 'rbx report' and support for VM and ruby crashes
* Add CM::Script#eval_source
* Rewrote Array#pack and String#unpack
* Add code to detect bad extensions. Recompile your extensions.
* Add dbm, sdbm, and gdbm extensions
* Implement support for -n, -p, and -a. Fixes #353.
* Add and use --agent when running the specs
* Add String#secure_compare @x-api
* Add heapdump capability for memory debugging
* Add automatic object ivar packing (improves memory usage)

#### Fixes

* Improve FFI compatibility
* Support reading and writing callback type fields
* Fix a few more FFI incompat issues
* Improve rails 3.[01] compatibility
* Upgrade to RubyGems 1.3.7
* Handle control flow edge case in JIT. Fixes #478.
* A few simple fixups for stringscanner. Fixes #481.
* Fix parser edge case
* Support rb_sys_fail being passed NULL
* Verify that the ruby to build with is the one it was configured with
* Fixed Module#define_method to return a lambda Proc
* Add Kernel#type even though it's deprecated. Fixes #469.
* Move Etc::Passwd and Etc::Group to Struct
* Add no-op rubysig.h to make extensions happy
* Add unblock support to rb_thread_blocking_region. Credit: kudo. Fixes #461.
* Add missing capi functions needed for hpricot
* Implemented rb_struct_aset, _aref.
* Add a rb_thread_critical and a null SIGALRM handler.
* Flush/update CAPI handles better. Fixes #455.
* Track the location of globals for rb_global_variable. Fixes #462.
* Handle check EINTR in rb_thread_select properly. Fixes #462.
* Ignore inlined methods when considering what to JIT
* Ignore an error when all input was read. Fixes #454.
* Temporary fix for perf regression in Array#include?.
* Rubinius eagerly raises on frozen objects.
* Normalizing all access of Tuple and ByteArray.
* Update array handles the first time you see one. Fixes #455.
* Fix docs and const-ness in capi. Fixes #457.
* Dir.glob also takes an array or a null delim string. Fixes #458.
* Fixed #require edge case. Closes #365.
* Performance fix. The JIT was accidently always using the reset pointer.
* Raise a TypeError if attempting to inherit from a metaclass.
* Fixed class variable with metaclass. Closes #411.
* Ensure String.pattern receives a positive size.
* Fully implement rb_iterate
* Added rb_hash_delete_if.
* Deal with being interrupted while writing better. Fixes #373.
* Fixes Float#divmod bug. Closes #451.
* Added RBIGNUM_SIGN.
* Added define for RB_NUM_COERCE_FUNCS_NEED_OPID. Closes #447.
* Fixed rb_hash implementation. Closes #446.
* Use the specified socket type instead of Socket::AF_UNSPEC, fixes #442
* Fix fallback to ObjectMemory allocation if no new slab can be allocated
* Use RbConfig instead of Config
* Rework Process::Status specs to be simplified and run them on Windows
* Keep reading if read(2) gets EINTR
* Speed up Kernel#` by using a specialized primitive
* Use same function signature as MRI for rb_reg_new
* Check for and run #to_ary when passed to yield with 2+ args. Fixes #374.
* Don't expand -I paths. Fixes #434.
* Remove old, stale minitest
* Slight cleanup of Socket and adding Socket.getnameinfo
* Add support for rb_gv_get("$~")
* Initial version.h for C-API.
* Add the C API rb_reg_nth_match method
* Add C API function rb_reg_new
* Fixed compiling empty script with \_\_END\_\_ data. Closes #433.
* Remove unused libltdl.
* Fixes for Process::Status
* Fix Thread.critical= allows all non-bool arguments
* Use getrusage for Process.times
* Disable the agent in forks and cleanup before exec
* Check the handle against our known set of handles before marking in rb_gc_mark_maybe()
* Validate StatiScopes in constant caches. Fixes #426.
* Pass the current block into an sclass. Fixes #425.
* Handle frozen strings used for a StringIO
* Fix MatchData#string returns a frozen copy of the match string
* Allow IO.setup to be passed a numeric mode
* Fix IO#write_nonblock to raise on an unwritable IO even if data to write is 0 bytes
* Enhance Rubinius::Fiber
* Module#class_variable_set now raises a TypeError when self is frozen
* Fix IO.foreach with a nil separator, it should yield the entire contents at once
* Remove Ar since it's not used anymore
* Allow splat arguments in define_method. Fixes #419.
* Preserve path seperators in Dir.glob. Fixes #420.
* add rb_need_block() to rbx capi
* Kernel.extend now raises a TypeError, when called on a frozen object
* Fix profiler graph output and handling of blocks
* Cleanup of Bignum#%, significant performance improvement
* Don't allow initializing a class twice
* Move Hash#setup to Hash#\_\_setup\_\_ to allow overriding
* Cleanup IO.popen and IO::BidirectionalPipe
* Only run ->dfree if ->data isn't NULL
* Fix Module#remove_const to not call Module#const_missing
* Temporarily disable running finalizers at exit.
* Test the proper scope when determining if long return is possible
* Deal with Fixnum and Symbol speciallize in #methods. Fixes #403
* Handle #inspect calling #to_s in the right cases. Fixes #405
* Handle block arguments to a lambda specially. Fixes #398.
* File#truncate now raises an IOError if file is not opened for writing
* Fix R::CompiledMethod#add_metadata to allow multiple keys
* Don't use self in a block that might be instance_evald. Fixes #399.
* Expose the SystemExit to at_exit handlers. Fixes #395.
* Minor performanece fix, don't initialize an object body twice
* Have Proc#inspect use the exact same format as MRI
* Support -1 as the free function
* Add Rubinius specific #\_\_finalize\_\_ API for resurrectable finalizers
* Fix Struct.specialize_initialize for subclasses of Struct
* Add rb_throw to the C API.
* Fix classic strncmp bug in libffi
* Fixed be_computed_by matcher to display inspected value.
* Add implementation for rb_yield_values
* Include errno.h to ruby.h (MRI includes it in rubyio.h).
* Speed/foolproof Kernel#dup. Add custom object_class inline logic.
* Fixed File.size and .size?.
* Fix File#size to conform to specs. Closes #392.
* Add a primitive to calculate the MRI format backtrace. @perf @rspec
* Check frozen on Hash#initialize.
* Fix for next with ensure (pair Evan).
* Fixed compiler specs for next running ensure.
* Fixed next running ensures.
* Specs for next running ensure blocks.
* Fix -Xjit.profile to provide JIT specific output
* Remove extranious cast_array from bytecode
* Convert Hash to use #each_entry instead of #to_iter. @perf
* Add missing OFFT2NUM. Fixes #390.
* Time objects return T_DATA to match MRI C API
* Remove custom sorting logic in Enumerable, use Array.
* Added rb_set_kcode.
* Fix warnings compiling Melbourne ext in 1.9.
* Generalize building with MRI 1.8/1.9 or rbx.
* Add --cc to configure Script, so you can do 'rvm install rbx -C --cc=clang', as with MRI and REE.
* Fix building with gcc 4.4.1.
* Run at_exit blocks and finalizers before exiting a fork. Fixes #372.
* Add -Xgc.honor_start to control if GC.start is honored
* Use thread-safe Array#delete_if. Fixes #361.
* Fixed path in IO.reopen. Closes #360.
* Minimize array contents for IO.select. Fixes #359
* Fix TCPServer.for_fd and UNIXSocket.for_fd. Fixes #354. Fixes #355.
* Fix ugly signal bug related to pipe fds.
* Fix IO#write_nonblock to conform to specs
* Allow CallFrame::print_backtrace to see included modules
* Fix IO#write_nonblock to actually not block
* Rework how signals are delivered. Fixes #356
* Conform WeakRef API to MRI. Closes #370.
* Added io/nonblock.rb from MRI 1.8.7. Test passes. Closes #346.
* Added rb_to_int. Closes #367.
* Added stub weakref.rb. Closes #368.
* Adjust checking for GCC version to build on Fedora 8.
* Dir.new needs to convert path to a String also.
* Dir#new should call #to_str under 1.8 and #to_path under 1.9
* Fixed Dir.open to call StringValue. Closes #362.
* Dir should be calling to_str on non-String arguments in Ruby 1.8
* kronos' fix for Bignum#\<\<. Closes #350.
* Use correct module namespacing in Numeric specs.
* Further conform Numeric to MRI. Closes #349.
* More specs for Numeric#<=>.
* Reworked profiler graph output change.
* Add NUM2OFFT, fixes #348
* Ensure argument is not modified by File.expand_path. Closes #351.
* Spec for alias+super interaction with included modules
* Validate the starting Module of a super call. @crash
