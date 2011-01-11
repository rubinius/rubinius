require 'rakelib/rubinius'
require 'rakelib/build'
require 'rakelib/instruction_parser'
require 'rakelib/generator_task'

require 'tmpdir'
require 'ostruct'

config = OpenStruct.new
config.use_jit = true
config.compile_with_llvm = false

CONFIG = config


task :vm => 'vm/vm'

############################################################
# Files, Flags, & Constants

ENV.delete 'CDPATH' # confuses llvm_config

if CONFIG.compile_with_llvm
  ENV['CC'] = "llvm-gcc"
end

LLVM_ENABLE = (Rubinius::BUILD_CONFIG[:llvm] != :no)


if ENV['TESTS_ONLY']
  tests = FileList[ENV['TESTS_ONLY']]
else
  tests = FileList["vm/test/**/test_*.hpp"]
end

tests.uniq!

subdirs = %w!builtin capi util instruments gc llvm!

srcs        = FileList["vm/*.{cpp,c}"]
subdirs.each do |dir|
  srcs += FileList["vm/#{dir}/*.{cpp,c}"]
end

hdrs        = FileList["vm/*.{hpp,h}"]
subdirs.each do |dir|
  hdrs += FileList["vm/#{dir}/*.{hpp,h}"]
end

objs        = srcs.map { |f| f.sub(/((c(pp)?)|S)$/, 'o') }

dep_file    = "vm/.depends.mf"
vm_objs     = %w[ vm/drivers/cli.o ]
vm_srcs     = %w[ vm/drivers/cli.cpp ]

EX_INC      = %w[ libtommath libgdtoa onig libffi/include
                ].map { |f| "vm/external_libs/#{f}" }

INSN_GEN    = %w[ vm/gen/instruction_names.cpp
                  vm/gen/instruction_names.hpp
                  vm/gen/instruction_sizes.hpp
                  vm/gen/instruction_prototypes.hpp
                  vm/gen/instruction_defines.hpp
                  vm/gen/instruction_locations.hpp
                  vm/gen/instruction_implementations.hpp
                  vm/gen/instruction_visitors.hpp
                  vm/gen/instruction_effects.hpp
                  web/_includes/instructions.markdown
                ]
TYPE_GEN    = %w[ vm/gen/includes.hpp
                  vm/gen/kind_of.hpp
                  vm/gen/object_types.hpp
                  vm/gen/typechecks.gen.cpp
                  vm/gen/primitives_declare.hpp
                  vm/gen/primitives_glue.gen.cpp ]

task :bogo_field_extract => TYPE_GEN

# Files are in order based on dependencies. For example,
# CompactLookupTable inherits from Tuple, so the header
# for compactlookuptable.hpp has to come after tuple.hpp
field_extract_headers = %w[
  vm/builtin/object.hpp
  vm/builtin/integer.hpp
  vm/builtin/fixnum.hpp
  vm/builtin/array.hpp
  vm/builtin/bignum.hpp
  vm/builtin/executable.hpp
  vm/builtin/access_variable.hpp
  vm/builtin/alias.hpp
  vm/builtin/block_environment.hpp
  vm/builtin/block_as_method.hpp
  vm/builtin/bytearray.hpp
  vm/builtin/io.hpp
  vm/builtin/channel.hpp
  vm/builtin/module.hpp
  vm/builtin/class.hpp
  vm/builtin/compiledmethod.hpp
  vm/builtin/data.hpp
  vm/builtin/dir.hpp
  vm/builtin/exception.hpp
  vm/builtin/float.hpp
  vm/builtin/immediates.hpp
  vm/builtin/iseq.hpp
  vm/builtin/list.hpp
  vm/builtin/lookuptable.hpp
  vm/builtin/ffi_pointer.hpp
  vm/builtin/methodtable.hpp
  vm/builtin/nativefunction.hpp
  vm/builtin/packed_object.hpp
  vm/builtin/randomizer.hpp
  vm/builtin/regexp.hpp
  vm/builtin/staticscope.hpp
  vm/builtin/string.hpp
  vm/builtin/symbol.hpp
  vm/builtin/thread.hpp
  vm/builtin/tuple.hpp
  vm/builtin/compactlookuptable.hpp
  vm/builtin/time.hpp
  vm/builtin/taskprobe.hpp
  vm/builtin/nativemethod.hpp
  vm/builtin/system.hpp
  vm/builtin/autoload.hpp
  vm/builtin/proc.hpp
  vm/builtin/variable_scope.hpp
  vm/builtin/location.hpp
  vm/builtin/capi_handle.hpp
  vm/builtin/global_cache_entry.hpp
  vm/builtin/weakref.hpp
  vm/builtin/fiber.hpp
  vm/builtin/thunk.hpp
  vm/builtin/call_unit.hpp
  vm/builtin/call_unit_adapter.hpp
]

# TODO: all external libraries should have proper dependencies.
libgdtoa_files = FileList["vm/external_libs/libgdtoa/*.[ch]"]

file "vm/external_libs/libgdtoa/libgdtoa.a" => libgdtoa_files

EXTERNALS   = %W[ vm/external_libs/libtommath/libtommath.a
                  vm/external_libs/libgdtoa/libgdtoa.a
                  vm/external_libs/onig/.libs/libonig.a
                  vm/external_libs/libffi/.libs/libffi.a ]

INCLUDES      = EX_INC + %w[vm/test/cxxtest vm .]

extra = %w!/usr/local/include /opt/local/include!

extra.each do |dir|
  INCLUDES << dir if File.directory?(dir)
end

INCLUDES.map! { |f| "-I#{f}" }

# Default build options
BASIC_FLAGS     = %W[ -pipe -Wall -fno-omit-frame-pointer ]

FLAGS = BASIC_FLAGS.dup

# Check compiler preferences (LLVM may still override.)
CC          = ENV['CC'] || "gcc"
CXX         = ENV["CXX"] || "g++"

if RUBY_PLATFORM =~ /darwin/i
  if `sw_vers` =~ /10\.4/
    FLAGS.concat %w(-DHAVE_STRLCAT -DHAVE_STRLCPY)
  end

  # This flag makes the executable non-relocatable (and
  # slightly faster), but 4.3 does not support it.
  # TODO: Look for workarounds.
  unless `#{CC} -v 2>&1` =~ /gcc version 4\.3/i
    FLAGS << "-mdynamic-no-pic"
  end
end

if LLVM_ENABLE
  EXTERNALS << "vm/external_libs/udis86/libudis86/.libs/libudis86.a"
  INCLUDES << "-Ivm/external_libs/udis86"
  FLAGS << "-DENABLE_LLVM"
  STDERR.puts "LLVM inclusion enabled."
=begin
  llvm_flags = `#{LLVM_CONFIG} --cflags`.split(/\s+/)
  llvm_flags.delete_if { |e| e.index("-O") == 0 }
  FLAGS.concat llvm_flags
  FLAGS << '-D__STDC_LIMIT_MACROS' unless FLAGS.include? '-D__STDC_LIMIT_MACROS'
  FLAGS << '-D__STDC_CONSTANT_MACROS' unless FLAGS.include? '-D__STDC_CONSTANT_MACROS'
=end
end

Rubinius::BUILD_CONFIG[:defines].each do |flag|
  FLAGS << "-D#{flag}"
end

BUILD_PRETASKS = []

if ENV['DEV']
  BUILD_PRETASKS << "build:debug_flags"
end

def compile_c(obj, src, output_kind="c")
  flags = llvm_flags + INCLUDES + FLAGS

  if CONFIG.compile_with_llvm
    flags << "-emit-llvm"
  end

  # GROSS
  if src == "vm/test/runner.cpp"
    flags.delete_if { |f| /-O.*/.match(f) }
  end

  flags = flags.join(" ")

  # Now include CFLAGS and CXXFLAGS
  if str = ENV['CXXFLAGS']
    flags << " #{str}"
  elsif str = ENV['CFLAGS']
    flags << " #{str}"
  end

  # Make sure we are C99 compatible.
  flags << " -D__STDC_LIMIT_MACROS" unless flags =~ /__STDC_LIMIT_MACROS/

  if $verbose
    sh "#{CC} #{flags} -#{output_kind} -o #{obj} #{src} 2>&1"
  else
    puts "CC #{src}"
    sh "#{CC} #{flags} -#{output_kind} -o #{obj} #{src} 2>&1", :verbose => false
  end
end

def ld(t)
  link_opts = llvm_link_flags()

  link_opts += ' -Wl,--export-dynamic' if RUBY_PLATFORM =~ /linux|openbsd/i
  link_opts += ' -rdynamic'            if RUBY_PLATFORM =~ /bsd/
  link_opts += ' -lstdc++'

  # Include LDFLAGS
  if str = ENV['LDFLAGS']
    link_opts << " #{str}"
  end

  ld = ENV['LD'] || 'g++'

  if CONFIG.compile_with_llvm
    objs = t.prerequisites.find_all { |f| f =~ /o$/ }.join(' ')

    sh "llvm-link -f -o vm/tmp.bc #{objs}"
    sh "opt -O3 -f -o vm/objs.bc vm/tmp.bc"
    sh "rm vm/tmp.bc"
    sh "llc -filetype=asm -f -o vm/objs.s vm/objs.bc"
    sh "rm vm/objs.bc"
    flags = INCLUDES + FLAGS
    sh "#{CC} #{flags.join(' ')} -c -o vm/objs.o vm/objs.s"
    sh "rm vm/objs.s"

    o = (["vm/objs.o"] + t.prerequisites.find_all { |f| f =~ /a$/ }).join(' ')
  else
    o = t.prerequisites.find_all { |f| f =~ /[oa]$/ }.join(' ')
  end

  l  = ex_libs.join(' ')

  if $verbose
    sh "#{ld} #{link_opts} -o #{t.name} #{o} #{l}"
  else
    puts "LD #{t.name}"
    sh "#{ld} #{link_opts} -o #{t.name} #{o} #{l}", :verbose => false
  end
end

############################################################
# Other Tasks

# Build options.
namespace :build do

  # The top-level build task uses this, so undocumented.
  task :normal      => %w[ build:normal_flags build:profiler_flags build:build ]

  desc "Show methods that are not inlined in the optimized build"
  task :inline      => %w[ build:inline_flags build:profiler_flags build:build ]

  desc "Build debug image for GDB. No optimizations, more warnings."
  task :debug       => %w[ build:debug_flags build:stats_flags
                           build:profiler_flags build:build ]

  task :stats       => %w[ build:normal_flags build:stats_flags
                           build:profiler_flags build:build ]

  desc "Build to check for possible problems in the code. See build:help."
  task :strict      => %w[ build:strict_flags build:build ]

  desc "Build to enforce coding practices. See build:help for info."
  task :ridiculous  => %w[ build:ridiculous_flags build:build ]

  desc "Build without any debug code"
  task :nodebug => %w[ build:nodebug_flags build:build ]

  desc "Generate dependency file"
  task :depends     => dep_file

  # Stupid rake.
  unless Rake.application.top_level_tasks.include? "clean"
    import dep_file
  end

  desc "Build LLVM"
  task :llvm do
    if LLVM_ENABLE and Rubinius::BUILD_CONFIG[:llvm] == :svn
      unless File.file?("vm/external_libs/llvm/Release/bin/llvm-config")
        sh "cd vm/external_libs/llvm; REQUIRES_RTTI=1 ./configure #{llvm_config_flags}; REQUIRES_RTTI=1 #{make}"
      end
    end
  end

  # Issue the actual build commands. NEVER USE DIRECTLY.
  task :build => BUILD_PRETASKS +
                 %w[
                     build:llvm
                     vm
                     kernel:build
                     build:ffi:preprocessor
                     extensions
                   ]

  # Flag setup

  task :normal_flags do
    FLAGS.concat %w[ -ggdb3 -O2 -Werror ]
  end

  task :inline_flags => :normal_flags do
    FLAGS.concat %w[ -Winline -Wuninitialized ]
  end

  # -Wuninitialized requires -O, so it is not here.
  task :debug_flags => "build:normal_flags" do
    FLAGS.delete "-O2"
    FLAGS.concat %w[ -O0 -fno-inline -DRBX_DEBUG ]
  end

  task :nodebug_flags => "build:normal_flags" do
    FLAGS << "-DNDEBUG"
  end

  task :stats_flags do
    FLAGS.concat %w[ -DRBX_GC_STATS ]
  end

  task :profiler_flags do
    FLAGS.concat %w[ -DRBX_PROFILER ] unless ENV['RBX_NO_PROFILER']
  end

  task :strict_flags => "build:debug_flags" do
    FLAGS.concat %w[ -Wextra -Wno-unused-parameter ]
  end

  task :ridiculous_flags => "build:strict_flags" do
    FLAGS.concat %w[ -Weffc++ ]
  end

  desc "Print more information about the build task options."
  task :help do
    puts <<-ENDHELP

  There are five ways to build the VM. Each of the last three
  special tasks is cumulative with the one(s) above it:


   Users
  -------

  build             Build normal, optimized image.


   Developers
  ------------

  build:inline      Use to check if any methods requested to be
                    inlined were not. The image is otherwise the
                    normal optimized one.

  build:debug       Use when you need to debug in GDB. Builds an
                    image with debugging symbols, optimizations
                    are disabled and more warnings emitted by the
                    compiler. Also adds defines to turn on stats.

  build:stats       Use when you want to enable various VM stats,
                    like garbage collector or JIT stats. The build
                    is optimized but adds defines to turn on stats.

  build:strict      Use to check for and fix potential problems
                    in the codebase. Shows many more warnings.

  build:ridiculous  Use to enforce good practices. Adds warnings
                    for not abiding by "Efficient C++" guidelines
                    and treats all warnings as errors. It may be
                    necessary to disable some warning types for
                    this build.

  Notes:
    - If you do not want to use --trace but do want to see the exact
      shell commands issued for compilation, invoke Rake thus:

        `rake build:debug -- -v`

    ENDHELP
  end

  namespace :ffi do

    FFI_PREPROCESSABLES = %w[ lib/etc.rb
                              lib/fcntl.rb
                              lib/syslog.rb
                              lib/zlib.rb
                            ]

    # Generate the .rb files from lib/*.rb.ffi
    task :preprocessor => FFI_PREPROCESSABLES

    FFI::Generator::Task.new FFI_PREPROCESSABLES

  end

end


# Compilation tasks

rule '.o' do |t|
  obj   = t.name
  src   = t.prerequisites.find { |f| f =~ /#{File.basename obj, '.o'}\.((c(pp)?)|S)$/}

  compile_c obj, src
end

rule '.S' do |t|
  obj = t.name
  src = obj.gsub(/\.S$/, ".cpp")

  compile_c obj, src, "S"
end

def files(targets, dependencies=nil, &block)
  targets.each do |target|
    if dependencies then
      file target => dependencies, &block
    else
      file target, &block
    end
  end
end

directory "vm/gen"
file  "vm/type_info.o"    => "vm/gen/typechecks.gen.cpp"
file  "vm/primitives.hpp" => "vm/gen/primitives_declare.hpp"
files Dir["vm/builtin/*.hpp"], INSN_GEN
files vm_objs, vm_srcs

objs.zip(srcs).each do |obj, src|
  file obj => src
end

objs.zip(srcs).each do |obj, src|
  file "#{File.basename obj, '.o'}.S" => src
end

files EXTERNALS do |t|
  path = File.join(*split_all(t.name)[0..2])
  configure_path = File.join(path, 'configure')

  if File.exist? configure_path then
    sh "cd #{path}; ./configure && #{make}"
  else
    sh "cd #{path}; #{make}"
  end
end

file 'vm/primitives.o'                => 'vm/codegen/field_extract.rb'
file 'vm/primitives.o'                => TYPE_GEN
file 'vm/codegen/field_extract.rb'    => 'vm/gen'

task :run_field_extract do
  field_extract field_extract_headers
end

files TYPE_GEN, field_extract_headers + %w[vm/codegen/field_extract.rb] + [:run_field_extract] do
end

file 'vm/vm' => EXTERNALS + objs + vm_objs do |t|
  ld t
end

file 'vm/gen/primitives_glue.gen.cpp' => hdrs

file 'vm/test/runner.cpp' => tests + objs do
  tests = tests.sort
  puts "GEN vm/test/runner.cpp" unless $verbose
  tests << { :verbose => $verbose }
  sh("vm/test/cxxtest/cxxtestgen.pl", "--error-printer", "--have-eh",
     "--abort-on-fail", "-include=string.h", "-include=stdlib.h",
     "-include=vm/test/test_setup.h",
     "-o", "vm/test/runner.cpp", *tests)
end

file 'vm/test/runner.o' => 'vm/test/runner.cpp' # no rule .o => .cpp

file 'vm/test/runner' => EXTERNALS + objs + %w[vm/test/runner.o] do |t|
  ld t
end

# A simple JIT tester driver

file 'vm/drivers/compile.o' => 'vm/drivers/compile.cpp'

file 'vm/compile' => EXTERNALS + objs + %w[vm/drivers/compile.o] do |t|
  ld t
end

# Generate files for instructions and interpreters

iparser = InstructionParser.new "vm/instructions.def"

def generate_instruction_file(parser, generator, name)
  puts "GEN #{name}"
  parser.parse
  parser.send generator, name
end

insn_deps = %w[  vm/gen
                 vm/instructions.def
                 rakelib/instruction_parser.rb
              ]

file "lib/compiler/opcodes.rb" => insn_deps do |t|
  generate_instruction_file iparser, :generate_opcodes, t.name
end

file "lib/compiler/generator_methods.rb" => insn_deps do |t|
  generate_instruction_file iparser, :generate_generator_methods, t.name
end

file "vm/gen/instruction_names.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_names_header, t.name
end

file "vm/gen/instruction_names.cpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_names, t.name
end

file "vm/gen/instruction_prototypes.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_prototypes, t.name
end

file "vm/gen/instruction_sizes.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_sizes, t.name
end

file "vm/gen/instruction_defines.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_defines, t.name
end

file "vm/gen/instruction_locations.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_locations, t.name
end

file "vm/gen/instruction_implementations.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_implementations , t.name
end

file "vm/gen/instruction_visitors.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_visitors, t.name
end

file "vm/gen/instruction_effects.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_stack_effects, t.name
end

file "web/_includes/instructions.markdown" => insn_deps do |t|
  generate_instruction_file iparser, :generate_documentation, t.name
end

namespace :vm do
  desc 'Run all VM tests.  Uses its argument as a filter of tests to run.'
  task :test, :filter do |task, args|
    ENV['SUITE'] = args[:filter] if args[:filter]
    ENV['VERBOSE'] = '1' if $verbose
    sh 'vm/test/runner', :verbose => $verbose
  end

  task :test => %w[ build:debug_flags vm/test/runner ]

  task :coverage do
    Dir.mkdir "vm/test/coverage" unless File.directory? "vm/test/coverage"
    link_opts = llvm_link_flags()

    flags = (INCLUDES + FLAGS + llvm_flags).join(' ')

    puts "CC/LD vm/test/coverage/runner"
    begin
      path = "vm/instructions.cpp"
      sh "#{CXX} -fprofile-arcs -ftest-coverage #{flags} -o vm/test/coverage/runner vm/test/runner.cpp vm/*.cpp vm/builtin/*.cpp #{path} #{link_opts} #{(ex_libs + EXTERNALS).join(' ')}"

      puts "RUN vm/test/coverage/runner"
      sh "vm/test/coverage/runner"
      if $verbose
        sh "vm/test/lcov/bin/lcov --directory . --capture --output-file vm/test/coverage/app.info"
      else
        sh "vm/test/lcov/bin/lcov --directory . --capture --output-file vm/test/coverage/app.info > /dev/null 2>&1"
      end

      puts "GEN vm/test/coverage/index.html"
      if $verbose
        sh "cd vm/test/coverage; ../lcov/bin/genhtml app.info"
      else
        sh "cd vm/test/coverage; ../lcov/bin/genhtml app.info > /dev/null 2>&1"
      end
    ensure
      sh "rm -f *.gcno *.gcda"
    end
  end

  desc "Clean up vm build files"
  task :clean do
    files = FileList[
      objs,
      vm_objs,
      dep_file,
      TYPE_GEN,
      INSN_GEN,
      'vm/gen/*',
      'vm/test/runner',
      'vm/test/runner.cpp',
      'vm/test/runner.o',
      'vm/vm',
      'vm/.deps'
    ].exclude("vm/gen/config.h")

    files.each do |filename|
      rm_rf filename, :verbose => $verbose
    end
  end

  desc "Clean up, including all external libs"
  task :distclean => :clean do
    EXTERNALS.each do |lib|
      path = File.join(*(lib.split(File::SEPARATOR)[0..-2].reject{|i| i =~ /^\./}))
      system "cd #{path}; #{make} clean || true"
    end
  end

  desc "Show which primitives are missing"
  task :missing_primitives do
    kernel_files = FileList['kernel/**/*.rb'].join " "
    kernel_primitives = `grep 'Ruby.primitive' #{kernel_files} | awk '{ print $3 }'`
    kernel_primitives = kernel_primitives.gsub(':', '').split("\n").sort.uniq

    cpp_primitives = `grep 'Ruby.primitive' vm/builtin/*.hpp | awk '{ print $4 }'`
    cpp_primitives = cpp_primitives.gsub(':', '').split("\n").sort.uniq

    missing = kernel_primitives - cpp_primitives

    puts missing.join("\n")
  end
end

############################################################$
# Importers & Methods:

require 'rake/loaders/makefile'
require 'rakelib/dependency_grapher'

generated = (TYPE_GEN + INSN_GEN).select { |f| f =~ /pp$/ }

object_sources = srcs + vm_srcs + generated

# vm/.depends.mf depends on all source and include files. If any of
# those files are newer than vm/.depends.mf, all dependencies are
# recalculated.

file dep_file => hdrs + object_sources do |t|
  warn "Updating dependencies..."

  directories = ".", "vm", "vm/capi/include"
  defines = FLAGS.join ' '

  grapher = DependencyGrapher.new object_sources, directories, defines
  grapher.process

  File.open t.name, "w" do |file|
    grapher.print_dependencies file
  end
end

def ex_libs # needs to be method to delay running of llvm_config
  unless defined? $ex_libs then
    $ex_libs = EXTERNALS.reverse

    if File.directory?("/usr/local/lib")
      $ex_libs << "-L/usr/local/lib"
    end

    if File.directory?("/opt/local/lib")
      $ex_libs << "-L/opt/local/lib"
    end

    $ex_libs << "-ldl" unless RUBY_PLATFORM =~ /bsd|haiku/
    $ex_libs << "-lcrypt" if RUBY_PLATFORM =~ /bsd/ && RUBY_PLATFORM !~ /openbsd/
    $ex_libs << "-lcrypto -pthread -lssl" if RUBY_PLATFORM =~ /openbsd/
    $ex_libs << "-lexecinfo" if RUBY_PLATFORM =~ /bsd/ && Rubinius::BUILD_CONFIG[:defines].include?('HAS_EXECINFO')
    $ex_libs << "-lrt -lcrypt" if RUBY_PLATFORM =~ /linux/
    $ex_libs << "-lnetwork" if RUBY_PLATFORM =~ /haiku/

    $ex_libs += llvm_lib_files
  end
  $ex_libs
end

def field_extract(headers)
  headers += [{ :verbose => $verbose}]

  ruby('vm/codegen/field_extract.rb', *headers)
end
