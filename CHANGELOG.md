# Rubinius Changelog

## Version: 2.2.10 (2014-06-27)

* Process.spawn and Process.exec duplicate file descriptors when mapping a
	parent file descriptor to a new child file descriptor. (Brian Shirai)

## Version: 2.2.9 (2014-06-10)

* Pre-installed gem binary wrappers are executable files. (Brian Shirai)

## Version: 2.2.8 (2014-06-10)

* Module#ancestors includes singleton classes for singleton classes.
	(Jesse Cooke)
* String#encode supports :universal_newline. (Yorick Peterse)
* Agent works across fork(). (Brian Shirai)
* GC.stat performance improved. (Brian Shirai)
* Lock inflation deadlock is fixed. (Dirkjan Bussink)
* C-API system calls for IO are interruptable. (Dirkjan Bussink)
* RbConfig::CONFIG['NULLCMD'] is set to ':'. (Yorick Peterse)
* Gem binary wrappers for pre-installed gems are executable. (Brian Shirai)

## Version: 2.2.7 (2014-05-20)

* Dir[] and Dir.glob call #to_path on multiple patterns. (Brian Shirai)
* The JIT does not elide Fixnum#hash calculation. (Brian Shirai)
* Enumerator#feed is implemented. (John DeSilva)
* Module#{*}_method_defined? compatibility improved. (Brian Shirai)
* ENV handles non-ASCII values. (Miguel Herranz)
* Improved support for packaging Rubinius. (Jan Alexander Steffens)
* The gems cache directory for building is configurable. (Yorick Peterse)
* Configure checks readline availability. (Sam Kottler)
* Hash#compare_by_identity does not copy String keys. (Lin Jen-Shin)
* Module#const_defined? compatibility is improved. (Brian Shirai)
* Setting a module name is improved. (Kenichi Kamiya, Brian Shirai)
* The C-API provides rb_class_real. (Brian Shirai)
* Special operator symbols are removed. (Kenichi Kamiya)
* Use of -> {} in BasicObject dispatches to Kernel::lambda. (Brian Shirai)
* Marshal.dump of Symbol includes its encoding. (Brian Shirai)
* Marshal.dump of negative Float includes its sign. (Brian Shirai)
* C-API rb_check_convert_type, rb_convert_type, rb_to_integer,
	rb_check_to_integer, rb_to_int, rb_Float, rb_Integer  compatibility is
	improved.  (Brian Shirai)
* Dir returns values encoded to ASCII_8BIT if the filename is invalid
	US_ASCII. (Dirkjan Bussink)
* Multiple assignment arguments following a splat argument are assigned in the
	proper order. (Federico Ravasio)
* STDOUT and STDERR are flushed on process exit. (Federico Ravasio)
* Time#strftime with "%L" is improved. (Federico Ravasio)
* Rational#round is improved. (Federico Ravasio)
* Binding UnboundMethods is improved. (Jesse Cooke, Jacob Rothstein)

## Version: 2.2.6 (2014-03-10)

* Kernel#to_enum uses block as Enumerable#size. (Federico Ravasio)
* Time#zone only encodes a non-nil zone. (Federico Ravasio)
* Kernel.binding returns a Binding for the caller. (Robert Gleeson)
* Failures in readdir_r() are handled. (Dirkjan Bussink)
* Various VM objects are properly (de)constructed. (Dirkjan Bussink)
* The JIT converts splatted nil to []. (Federico Ravasio, Dirkjan Bussink)
* The reference_p() VM predicate returns false for NULL. (Brian Shirai)
* The configure script uses ENV["SHELL"] to find libc.so. (Brian Shirai)
* Dir.glob matches non-ASCII paths. (Federico Ravasio)
* Dir.entries returns paths with encoding. (Federico Ravasio)
* Array#drop accounts for shifted start from Array#shift. (Federico Ravasio)
* Dir.open respects :encoding option. (Brian Shirai)
* String::from_cstr C++ method checks bounds. (Yorick Peterse)
* Compiling with clang++ using -std=c++11 is supported. (Gabriel Southern)
* Basic String#scrub is supported. (Yorick Peterse)
* Signals like SIGSEGV will cause additional logging information to be printed
	and then allow default system proccesing (e.g. core dumps). (Brian Shirai)
* The C-API supports rb_integer_pack with basic flags. (Brian Shirai)

## Version: 2.2.5 (2014-02-08)

* The 'main' object defines define_method. (Federico Ravasio)
* When passed a name with no file extension, #require will search all load
	paths for a file with a '.rb' extension before loading a file with the
	platform's dynamic library extension. (Brian Shirai)
* BasicObject can refer to BasicObject. (Federico Ravasio, Yorick Peterse)
* Standard library gems 'racc', 'minitest', and 'rubysl-test-unit' are now
	pre-installed, in addition to json and the RubySL gems. (Brian Shirai)
* RubyGems is updated to 2.2.2. (Brian Shirai)

## Version: 2.2.4 (2014-02-02)

* The erb command is available by default. (Brian Shirai)
* ARGF.set_encoding is supported. (Brian Shirai)
* Dir.glob handles infinite wildcards. (Yorick Peterse)
* Process.setproctitle is supported. (Yorick Peterse)
* The C-API provides the rb_io_check_io function. (Yorick Peterse)
* The method name as Symbol is returned from def. (Joe Eli Mac, Brian Shirai)
* Symbol is comparable. (Erik Michaels-Ober)
* RubyGems is updated to 2.2.1. (Brian Shirai)
* String#encode! with options compatibility is improved. (Sylvester Keil)
* LLVM 3.4 is supported. (Jan Alexander Steffens)
* Compiling with clang 3.4 is fixed. (Yorick Peterse)
* Linking with other than libstdc++ is supported. (Dirkjan Bussink)
* A GC issue overwriting field values is fixed. (Dirkjan Bussink)
* A memory leak in IO#sysread is fixed. (Dirkjan Bussink)
* String#count is fixed for multi-byte strings. (Federico Ravasio)
* String#replace is fixed for strings with invalid encodings.
	(Federico Ravasio)
* Performance of String#each_char is improved. (Federico Ravasio)
* String#split is improved for multi-byte patterns. (Dirkjan Bussink)
* Thread status is updated after fork(). (Dirkjan Bussink)
* Time handling of zone and encoding is improved. (Dirkjan Bussink)

## Version: 2.2.3 (2013-12-29)

* Use the configured program name in build scripts. (Chad Slaughter)
* Require 'find' for the utility script listing TODOs. (Benny Klotz)
* Use File.exist? in build scripts. (Mike Dorst)
* Provide logger library for bootstrapping Bundler 1.5+. (Brian Shirai)

## Version: 2.2.2 (2013-12-22)

* The --llvm-path configure option checks llvm version. (Gabriel Southern)
* Module#const_get resolves qualified names like '::A::B' (Brian Shirai)
* Bootstrap gem dependencies are included for mini_portile. (Brian Shirai)
* The close_on_exec state is set for new file descriptors. (Dirkjan Bussink)
* The C-API provides the rb_fix_fd_cloexec function. (Dirkjan Bussink)
* The 't' modifier is supported in file modes. (Dirkjan Bussink)
* A deadlock in thread startup is fixed. (Dirkjan Bussink)
* The configured 'make' program is used building llvm. (Dirkjan Bussink)
* A race condition on thread shutdown is fixed. (Dirkjan Bussink)
* An issue is fixed handling memory when parsing a Bignum. (Dirkjan Bussink)
* Improved parsing a Bignum. (Brian Shirai)
* The C-API provides the rb_intern_str function. (Dirkjan Bussink)
* The C-API provides the rb_int_positive_pow function. (Dirkjan Bussink)
* An issue is fixed with Kernel.local_variables and eval. (Kenichi Kamiya)
* An issue is fixed with File.realdirpath. (Kenichi Kamiya)
* Kernel#\__dir__ is implemented. (Kenichi Kamiya)
* An issue is fixed allocating Thread objects. (Dirkjan Bussink)
* An issue is fixed locking objects. (Dirkjan Bussink)
* An encoding issue is fixed with IO#gets(limit). (Ryo Onodera)
* The README is updated with a list of unimplemented standard library gems.
	(Erik Michaels-Ober)
* Removed use of lchmod on Linux. (r-stu31)
* The C-API provides rb_pid_t. (Yorick Peterse)
* The Rubinius::PROGRAM_NAME constant is the name of the Rubinius executable
	set by configure. (Brian Shirai)
* The config.rb.in file is used by the --release-build configure option.
	(Brian Shirai)
* The --stagingdir configure option enables setting the directory used to
	prepare all files for installation. (Brian Shirai)
* Ruby 1.8.7+ is supported for building Rubinius. (Brian Shirai)
* A Changelog file is added. (Brian Shirai)
