require 'rakelib/instruction_parser'
require 'rakelib/generator_task'
require 'rakelib/release'

require 'tmpdir'
require 'ostruct'

require 'daedalus'

config = OpenStruct.new
config.use_jit = true
config.compile_with_llvm = false

CONFIG = config
DEV_NULL = RUBY_PLATFORM =~ /mingw|mswin/ ? 'NUL' : '/dev/null'
VM_EXE = RUBY_PLATFORM =~ /mingw|mswin/ ? 'vm/vm.exe' : 'vm/vm'

############################################################
# Files, Flags, & Constants

encoding_database = "vm/gen/encoding_database.cpp"
transcoders_database = "vm/gen/transcoder_database.cpp"

vm_release_h = BUILD_CONFIG[:vm_release_h]

ENV.delete 'CDPATH' # confuses llvm_config

INSN_GEN    = %w[ vm/gen/instruction_names.cpp
                  vm/gen/instruction_names.hpp
                  vm/gen/instruction_sizes.hpp
                  vm/gen/instruction_prototypes.hpp
                  vm/gen/instruction_defines.hpp
                  vm/gen/instruction_locations.hpp
                  vm/gen/instruction_implementations.hpp
                  vm/gen/instruction_visitors.hpp
                  vm/gen/instruction_effects.hpp
                ]
TYPE_GEN    = %w[ vm/gen/includes.hpp
                  vm/gen/kind_of.hpp
                  vm/gen/object_types.hpp
                  vm/gen/typechecks.gen.cpp
                  vm/gen/primitives_declare.hpp
                  vm/gen/glue_functions.cpp
                  vm/gen/jit_functions.cpp
                  vm/gen/invoke_functions.cpp
                  vm/gen/accessor_functions.cpp
                  vm/gen/glue_resolver.cpp
                  vm/gen/jit_resolver.cpp
                  vm/gen/invoke_resolver.cpp ]

GENERATED = %W[ vm/gen/config_variables.h
                vm/gen/signature.h
                vm/dtrace/probes.h
                #{encoding_database}
                #{transcoders_database}
                #{vm_release_h}
              ] + TYPE_GEN + INSN_GEN

# Files are in order based on dependencies. For example,
# CompactLookupTable inherits from Tuple, so the header
# for compactlookuptable.hpp has to come after tuple.hpp
field_extract_headers = %w[
  vm/builtin/basic_object.hpp
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
  vm/builtin/byte_array.hpp
  vm/builtin/io.hpp
  vm/builtin/channel.hpp
  vm/builtin/module.hpp
  vm/builtin/constant_table.hpp
  vm/builtin/class.hpp
  vm/builtin/compiled_code.hpp
  vm/builtin/data.hpp
  vm/builtin/dir.hpp
  vm/builtin/exception.hpp
  vm/builtin/float.hpp
  vm/builtin/fsevent.hpp
  vm/builtin/immediates.hpp
  vm/builtin/iseq.hpp
  vm/builtin/list.hpp
  vm/builtin/logger.hpp
  vm/builtin/lookup_table.hpp
  vm/builtin/ffi_pointer.hpp
  vm/builtin/method_table.hpp
  vm/builtin/native_function.hpp
  vm/builtin/packed_object.hpp
  vm/builtin/randomizer.hpp
  vm/builtin/regexp.hpp
  vm/builtin/constant_scope.hpp
  vm/builtin/encoding.hpp
  vm/builtin/string.hpp
  vm/builtin/symbol.hpp
  vm/builtin/thread.hpp
  vm/builtin/tuple.hpp
  vm/builtin/compact_lookup_table.hpp
  vm/builtin/time.hpp
  vm/builtin/stat.hpp
  vm/builtin/native_method.hpp
  vm/builtin/system.hpp
  vm/builtin/autoload.hpp
  vm/builtin/proc.hpp
  vm/builtin/variable_scope.hpp
  vm/builtin/location.hpp
  vm/builtin/constant_cache.hpp
  vm/builtin/call_site.hpp
  vm/builtin/mono_inline_cache.hpp
  vm/builtin/poly_inline_cache.hpp
  vm/builtin/call_custom_cache.hpp
  vm/builtin/respond_to_cache.hpp
  vm/builtin/weakref.hpp
  vm/builtin/fiber.hpp
  vm/builtin/thunk.hpp
  vm/builtin/call_unit.hpp
  vm/builtin/call_unit_adapter.hpp
  vm/builtin/atomic.hpp
  vm/builtin/character.hpp
  vm/builtin/thread_state.hpp
  vm/builtin/jit.hpp
]

transcoders_src_dir = File.expand_path "../../vendor/oniguruma/enc/trans", __FILE__

libdir = "#{BUILD_CONFIG[:build_prefix]}"
transcoders_lib_dir = "#{libdir}/#{BUILD_CONFIG[:encdir]}"
directory transcoders_lib_dir

TRANSCODING_LIBS = []

Dir["#{transcoders_src_dir}/*.c"].each do |name|
  name.sub!(/\.c$/, ".#{$dlext}")
  target = File.join transcoders_lib_dir, File.basename(name)

  task name do
  end

  file target => name do |t|
    cp t.prerequisites.first, t.name, :preserve => true, :verbose => $verbose
  end

  TRANSCODING_LIBS << target
end

############################################################
# Other Tasks

# Build options.
namespace :build do

  # Issue the actual build commands. NEVER USE DIRECTLY.
  task :build => %W[
    #{VM_EXE}
    compiler:generate
    stage:bin
    stage:extra_bins
    stage:capi_include
    kernel:build
    stage:kernel
    stage:runtime
    stage:lib
    gems:melbourne
    gems:extensions
    stage:documentation
    stage:manpages
  ] + TRANSCODING_LIBS
end

# Compilation tasks

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

file 'vm/codegen/field_extract.rb'    => 'vm/gen'

task :run_field_extract do
  ruby 'vm/codegen/field_extract.rb', *field_extract_headers
end

files TYPE_GEN, field_extract_headers + %w[vm/codegen/field_extract.rb] + [:run_field_extract] do
end

encoding_extract = 'vm/codegen/encoding_extract.rb'

file encoding_database => encoding_extract do |t|
  dir = File.expand_path "../../vendor/oniguruma", __FILE__
  ruby encoding_extract, dir, t.name
end

transcoders_extract = 'vm/codegen/transcoders_extract.rb'

file transcoders_database => [transcoders_lib_dir, transcoders_extract] do |t|
  ruby transcoders_extract, transcoders_src_dir, t.name
end

task vm_release_h do |t|
  write_release t.name, BUILD_CONFIG[:version], BUILD_CONFIG[:release_date]
end

file 'vm/gen/config_variables.h' => %w[library/rubinius/configuration.rb config.rb] do |t|
  puts "GEN #{t.name}"
  ruby 'vm/codegen/config_vars.rb', t.name
end

file 'vm/dtrace/probes.h' do |t|
  if Rubinius::BUILD_CONFIG[:dtrace]
    sh %[dtrace -h -o vm/dtrace/probes.h -s vm/dtrace/probes.d]
  end
end

if jobs = ENV['JOBS']
  @parallel_jobs = jobs.to_i
  if @parallel_jobs < 1
    STDERR.puts "Illegal number of parallel jobs: #{jobs}. Setting to 1."
    @parallel_jobs = 1
  end
elsif File.exist? ".be_gentle"
  @parallel_jobs = 1
else
  @parallel_jobs = nil
end

task VM_EXE => GENERATED do
  blueprint = Daedalus.load "rakelib/blueprint.rb"
  blueprint.build VM_EXE, @parallel_jobs
end

task 'vm/test/runner' => GENERATED do
  blueprint = Daedalus.load "rakelib/blueprint.rb"
  blueprint.build "vm/test/runner", @parallel_jobs
end

# Generate files for instructions and interpreters

file "gen/method_primitives.cpp" => field_extract_headers
file "gen/jit_primitives.cpp" => field_extract_headers
file "gen/invoke_primitives.cpp" => field_extract_headers
file "gen/accessor_primitives.cpp" => field_extract_headers
file "gen/method_resolver.cpp" => field_extract_headers
file "gen/jit_resolver.cpp" => field_extract_headers
file "gen/invoke_resolver.cpp" => field_extract_headers

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

namespace :vm do
  desc 'Run all VM tests.  Uses its argument as a filter of tests to run.'
  task :test, :filter do |task, args|
    ENV['SUITE'] = args[:filter] if args[:filter]
    ENV['VERBOSE'] = '1' if $verbose
    sh 'vm/test/runner', :verbose => $verbose
  end

  task :test => %w[ vm/test/runner ]

  desc "Clean up vm build files"
  task :clean do
    begin
      blueprint = Daedalus.load "rakelib/blueprint.rb"
      blueprint.clean
    rescue
      # Ignore clean failures
    end

    files = FileList[
      GENERATED,
      'vm/dtrace/probes.o',
      'vm/gen/*',
      'vm/test/runner',
      'vm/test/runner.cpp',
      'vm/test/runner.o',
      VM_EXE,
      BUILD_CONFIG[:program_name],
      'bin/rbx',
      'bin/ruby',
      'bin/rake',
      'bin/ri',
      'bin/rdoc',
      'bin/irb',
      'bin/gem',
      'vm/.deps',
    ].exclude("vm/gen/config.h", "vm/gen/paths.h")

    files.each do |filename|
      rm_rf filename, :verbose => $verbose
    end
  end

  desc "Clean up, including all external libs"
  task :distclean => :clean

  desc "Show which primitives are missing"
  task :missing_primitives do
    kernel_files = FileList['kernel/**/*.rb'].join " "
    kernel_primitives = `grep 'Rubinius.primitive' #{kernel_files} | awk '{ print $3 }'`
    kernel_primitives = kernel_primitives.gsub(':', '').split("\n").sort.uniq

    cpp_primitives = `grep 'Rubinius.primitive' vm/builtin/*.hpp | awk '{ print $4 }'`
    cpp_primitives = cpp_primitives.gsub(':', '').split("\n").sort.uniq

    missing = kernel_primitives - cpp_primitives

    puts missing.join("\n")
  end
end
