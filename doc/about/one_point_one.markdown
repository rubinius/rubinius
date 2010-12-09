---
layout: release
class: release
---

## Overview

The Rubinius team is proud to announce the availability of 1.0.1.

1.0.1 is primarily a bugfix release, meant to address issues found in the 1.0.
We decided to do a 1.0.1 because there were some installations issues found in
1.0 that prevented some new users from using Rubinius.  Additionally, a few
crash bugs were uncovered which we felt warranted a quick release.

As a bonus, this release includes fixes for running a number of popular gems.
The C-API support is extended to cover functionality required by popular gems
such as gherkin, curb, rjb/buildr, and many more.

#### Stats

* **Commits:** 199
* **Days since last release:** 23

#### Authors

* Brian Ford
* Dirkjan Bussink
* Evan Phoenix
* Ivan Samsonov
* Matthew Draper
* Mikko Perttunen
* Wilson Bilkovich

## Summary

#### Critical

* Update StackVariable::last_match_ in the GC. Fixes #287.
* Set the stack start and size properly in a thread. @bugfix
* Revise GIL unlocking to keep Thread::saved_call_frame_ sane
* Handle very large values from getrlimit properly
* Add flag to disallow direct allocation. Fixes #335.

#### Fixes

* Fixed defined?(a && b). Closes #302.
* Fixed Fixnum and Bignum bit shifts.
* Imported lib/matrix.rb from 1.9 with many bug fixes. @bugfix
* Revert Matrix#** behavior for exception to 1.8 version. @bugfix
* Changed libgdtoa g_dfmt() to emit MRI format for minimal, unique String representation of Float.
* Added to_s_minimal primitive for Float#to_s.
* Use send to reach private extend_object methods. Closes #324.
* Don't use a increment operation and the original value in the same expression
* Explicitly cast the return value of stack_pop() to void*
* Initial work to enable compiling with clang
* Use error state for read / write and print out the message
* Generalize jit generation for fixnum comparison operations
* Honor read-only-ness of certain globals. Fixes #283.
* Escape slashes in Regexp#to_s output. Fixes #308.
* Disambiguate a vcall send to the send_method opcode. Fixes #293.
* Evaluate next's argument even though it's ignored. Fixes #307.
* Report path to script bodies relatively. Fixes #308.
* Remove Numeric#==
* Use Kernel.exit! after a forked block returns. Fixes #289.
* Fix fork to allow SystemExit through properly
* Be persistant with write(2)
* Handle object_id's for immediates. Fixes #315.
* Alter protocol for how Proc with a bound method is invoked. Fixes #277.
* Teach Signal.trap the rest of it's tricks. Fixes #314.
* Make sure the arguments are Strings. @bugfix
* Cleanup and speed up Struct
* Add missing functionality to Signal.trap
* Introduce -Xprofile to be used instead of -P
* Cleanup and fix irb's save history feature
* Search Object's includes also. Fixes #328
* Make sure thet the default vis in a script body is private. Fixes #327
* Make STDOUT.reopen(path) effect the system fd. Fixes #322.
* Use the encoding of the Regexp in String#split on characters. Fixes #323
* Be sure to cleanup any temp modifications during #delete_if
* Let --enable-llvm do something. Fixes #336.
* Add flag support to new Dir.glob, always use it
* Handle write() setting EAGAIN properly
* Handle ECONNRESET like eof. Fixes #338
* Add UNIXSocket.pair
* Add Inliner::fixnum_le, and self recursion cuteness
* Simplify the Class#to_s of a metaclass (rails fix)
* Allow Modules to be dup'd, but not String
* Flip include order so that local LLVM wins
* Fix left shifting for fixnums
* Repair Bignum#divmod for cases with negative numbers. Fixes #319.
* Speed up String#[] and String#=~
* A Time is not eql? to a non-time.
* Raise the right exception when alias fails in a module.
* method_defined? shouldn't search Object/Kernel.
* Print error messages to stderr instead of stdout.
* Ensure the proper Regexp constant is found when processing literals
* Introduce 'push_rubinius' instruction and use it like a rented mule
* Factor out some jit code from push_cpath_top and push_rubinius
* Quit as early as possible if we're building on an unsupported platform

#### FFI

* Search all iconv FFI libs.
* Fix FFI::DynamicLibrary::Symbol#inspect
* Exclude info generation from all in libffi. Fixes #286.
* Add missing header files to scan for mode_t
* FFI API revamp, take 1.

#### C-API

* Added rb_ary_delete.
* Changed rb_set_errinfo to accept Qnil like 1.9.
* Added HAVE_STDARG_PROTOTYPES for C-API. Closes #300.
* Added OBJ_FREEZE for bluecloth gem.
* Added more C-API functions to get rjb to load.
* Only set status in rb_protect if it is not NULL.
* More C-API functions for DL.
* Added C-API HAVE_ macros for Hash.
* Moved C-API .h files to include directory.
* Moved C-API specs under spec/ruby/optional.
* Fixed rb_big2dbl.
* Add hacked rb_iterate/rb_each
* Add rb_reg_regcomp
* Annotate rb_raise as not returning to fix gherkin warnings. Fixes #309.
* Add stub of re.h
* Add missing symbols for curb
* Add simplicit rb_thread_blocking_region
* Provide builtin UBF macros
* Flesh out C-API to compile ImageMagick. Fixes #294.
* Make readline multithread aware and minor cleanup
* Check for async events when returning from a native method
* Check async events and raise an exception via rb_thread_select.
* Fix INT2NUM to actually take a long, not an int.
* Implement rb_is_{const,instance_class}_id() in CAPI.
* rb_protect's callback takes a VALUE, not ANYARGS.
* Add apparently-missed prototype for rb_apply()
* Add rb_f_global_variables() and rb_str_new3().
* rb_define_module_under() shouldn't return a parent's module.
* Make rb_num2dbl handle all inputs correctly.
* Set rbconfig's LIBS to "" to to appease mkrf
* Set rbconfig's LIBRUBYARG_SHARED to "", because we don't have a shared library yet.
* Add NativeMethod functors up to a ten-argument version

#### New

* Added NKF ext from MRI 1.8.7. Closes #304.
* Imported DL extension from 1.8.7.

#### EXPERIMENTAL

* Rewire debugger infrastructure, add new simple debugger.
* Add defered breakpoints to the debugger
* Add backtrace and discover ability to QueryAgent
* Started on the console, pulled in our own copy of bert
* Revamp agent API, add a number of variables
* Get jit.inline.blocks running again
* Add Rubinius::Agent and agent loopback connection
* Add -Xjit.sync
