task :vm => 'vm/vm'

############################################################
# Files, Flags, & Constants

ENV.delete 'CDPATH' # confuses llvm_config
LLVM_CONFIG = "vm/external_libs/llvm/Release/bin/llvm-config"
tests       = FileList["vm/test/test_*.hpp"]
srcs        = FileList["vm/*.{cpp,c}"] + FileList["vm/builtin/*.{cpp,c}"]
hdrs        = FileList["vm/*.{hpp,h}"] + FileList["vm/builtin/*.{hpp,h}"]
objs        = srcs.map { |f| f.sub(/c(pp)?$/, 'o') }
dep_file    = "vm/.depends.mf"
vm_objs     = %w[ vm/drivers/cli.o ]
vm_srcs     = %w[ vm/drivers/cli.cpp ]
EX_INC      = %w[ libtommath onig libffi/include
                  libltdl libev llvm/include
                ].map { |f| "vm/external_libs/#{f}" }
INSN_GEN    = %w[ vm/gen/iseq_instruction_names.cpp
                  vm/gen/iseq_instruction_names.hpp
                  vm/test/test_instructions.hpp ]
TYPE_GEN    = %w[ vm/gen/simple_field.rb
                  vm/gen/typechecks.gen.cpp
                  vm/gen/primitives_declare.hpp
                  vm/gen/primitives_glue.gen.cpp ]

LLVM_A      = "vm/external_libs/llvm/Release/lib/libLLVMSystem.a"
EXTERNALS   = %W[ #{LLVM_A}
                  vm/external_libs/libtommath/libtommath.a
                  vm/external_libs/onig/.libs/libonig.a
                  vm/external_libs/libffi/.libs/libffi.a
                  vm/external_libs/libltdl/.libs/libltdl.a
                  vm/external_libs/libev/.libs/libev.a ]
OPTIONS     = {
                LLVM_A => "--enable-targets=host-only --enable-optimized"
              }

INCLUDES    = (EX_INC + %w[vm/test/cxxtest vm .]).map { |f| "-I#{f}" }
FLAGS       = %w(-Wall -ggdb -gdwarf-2)

FLAGS << "-O2" if ENV['FAST']

CC = ENV['CC'] || "gcc"

def compile(obj, src)
  unless defined? $llvm_c then
    $llvm_c = `#{LLVM_CONFIG} --cflags`.split(/\s+/)
    $llvm_c.delete_if { |e| e.index("-O") == 0 }
  end

  flags = (INCLUDES + FLAGS + $llvm_c).join(' ')

  sh "#{CC} #{flags} -c -o #{obj} #{src} 2>&1"
end

############################################################
# Other Tasks

rule '.o' do |t|
  obj   = t.name
  src   = t.prerequisites.find { |f| f =~ /#{File.basename obj, '.o'}\.c(pp)?$/}

  compile obj, src
end

def files targets, dependencies = nil, &block
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
files objs, EXTERNALS
files vm_objs, vm_srcs

objs.zip(srcs).each do |obj, src|
  file obj => src
end

objs += ["vm/instructions.o"]

files EXTERNALS do |t|
  path = File.join(*split_all(t.name)[0..2])
  configure_path = File.join(path, 'configure')

  if File.exist? configure_path then
    sh "cd #{path}; ./configure #{OPTIONS[t.name]} && make"
  else
    sh "cd #{path}; make"
  end
end

file 'vm/instructions.rb' => 'vm/instructions_gen.rb'

files INSN_GEN, %w[vm/gen vm/instructions.rb] do
  ruby 'vm/instructions.rb', :verbose => $verbose
end

files TYPE_GEN, %w[vm/gen vm/instructions.rb vm/field_extract.rb] do
  field_extract
end

file 'vm/vm' => EXTERNALS + objs + vm_objs do |t|
  link t
end

file 'vm/gen/primitives_glue.gen.cpp' => hdrs

file 'vm/test/runner.cpp' => tests do
  tests += [{ :verbose => true }]
  sh("vm/test/cxxtest/cxxtestgen.pl", "--error-printer", "--have-eh",
     "--abort-on-fail", "-o", "vm/test/runner.cpp", *tests)
end

file 'vm/test/runner.o' => 'vm/test/runner.cpp' # no rule .o => .cpp

file 'vm/test/runner' => EXTERNALS + objs + %w[vm/test/runner.o] do |t|
  link t
end

file "vm/instructions.o" => %w[vm/llvm/instructions.cpp vm/instructions.rb] do
  ruby "vm/rubypp.rb vm/llvm/instructions.cpp vm/llvm/.instructions.cpp"
  compile "vm/instructions.o", "vm/llvm/.instructions.cpp"
  rm_f "vm/llvm/.instructions.cpp", :verbose => $verbose
end

file "vm/instructions.bc" => "vm/llvm/instructions.cpp" do
  ruby "vm/rubypp.rb vm/llvm/instructions.cpp vm/llvm/.instructions.cpp"
  sh "llvm-g++ -emit-llvm -Ivm -Ivm/external_libs/libffi/include -c -o vm/instructions.bc vm/llvm/.instructions.cpp", :verbose => true
  rm_f "llvm/.instructions.cpp", :verbose => $verbose
end

namespace :vm do
  desc 'Run all VM tests'
  task :test => 'vm/test/runner' do
    ENV['VERBOSE'] = '1' if $verbose 
    sh 'vm/test/runner', :verbose => $verbose
  end

  desc "Clean up vm build files"
  task :clean do
    files = [
      objs, dep_file,
      'vm/test/runner', 'vm/test/runner.cpp',
      'vm/gen',
      'vm/vm'
    ]

    files.each do |filename|
      rm_f filename, :verbose => $verbose
    end
  end

  desc "Clean up, including all external libs"
  task :distclean => :clean do
    EXTERNALS.each do |lib|
      path = File.join(*lib.split(File::SEPARATOR)[0..2])
      system "cd #{path}; make clean"
    end
  end

  desc "Show which primitives are missing"
  task :missing_primitives do
    cpp_primitives = `grep 'Ruby.primitive' vm/*.hpp | awk '{ print $4 }'`

    cpp_primitives = cpp_primitives.gsub(':', '').split("\n").sort.uniq

    shotgun_primitives = File.read('vm/shotgun_primitives.txt')
    shotgun_primitives = shotgun_primitives.split("\n").sort.uniq

    missing = shotgun_primitives - cpp_primitives

    puts missing.join("\n")
  end
end

############################################################$
# Importers & Methods:

require 'rake/loaders/makefile'

file dep_file => srcs + hdrs + vm_srcs do |t|
  flags = (INCLUDES + FLAGS).join(' ')
  flags << " -D__STDC_LIMIT_MACROS"

  dep = nil
  Dir.chdir 'vm' do
    dep = `makedepend -f- -- #{flags} -- #{t.prerequisites} 2>/dev/null`
  end
  dep.gsub!(%r% /usr/include\S+%, '') # speeds up rake a lot
  dep.gsub!(%r%^\S+:[^ ]%, '')

  File.open t.name, 'w' do |f|
    f.puts dep
  end
end

import dep_file

def ex_libs # needs to be method to delay running of llvm_config
  unless defined? $ex_libs then
    $ex_libs = EXTERNALS + [ "-ldl" ]
    $ex_libs << "-lrt" if RUBY_PLATFORM =~ /linux/

    llvm_libfiles = `#{LLVM_CONFIG} --libfiles all`.split(/\s+/)
    llvm_libfiles = llvm_libfiles.select { |f| File.file? f }

    pwd = File.join Dir.pwd, '' # add /
    llvm_libfiles = llvm_libfiles.map { |f| f.sub pwd, '' }
    $ex_libs += llvm_libfiles
  end
  $ex_libs
end

def field_extract
  order = %w[vm/builtin/object.hpp vm/objects.hpp]
  order += File.read("vm/objects.hpp").scan(/builtin[^"]+/).map { |f| "vm/#{f}" }
  order << { :verbose => $verbose}
p order
  ruby('vm/field_extract.rb', *order)
end

def link t
  $link_opts ||= `#{LLVM_CONFIG} --ldflags`.split(/\s+/).join(' ')

  ld = ENV['LD'] || 'g++'
  o  = t.prerequisites.find_all { |f| f =~ /o$/ }.join(' ')
  l  = ex_libs.join(' ')

  sh "#{ld} #{$link_opts} -o #{t.name} #{o} #{l}"
end

