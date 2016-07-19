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
VM_EXE = RUBY_PLATFORM =~ /mingw|mswin/ ? 'machine/vm.exe' : 'machine/vm'

############################################################
# Files, Flags, & Constants

encoding_database = "machine/gen/encoding_database.cpp"
transcoders_database = "machine/gen/transcoder_database.cpp"

vm_release_h = BUILD_CONFIG[:vm_release_h]

ENV.delete 'CDPATH' # confuses llvm_config

INSN_GEN    = %w[ machine/gen/instruction_names.cpp
                  machine/gen/instruction_names.hpp
                  machine/gen/instruction_sizes.hpp
                  machine/gen/instruction_prototypes.hpp
                  machine/gen/instruction_defines.hpp
                  machine/gen/instruction_locations.hpp
                  machine/gen/instruction_implementations.hpp
                  machine/gen/instruction_visitors.hpp
                  machine/gen/instruction_effects.hpp
                ]
TYPE_GEN    = %w[ machine/gen/includes.hpp
                  machine/gen/kind_of.hpp
                  machine/gen/object_types.hpp
                  machine/gen/typechecks.gen.cpp
                  machine/gen/primitives_declare.hpp
                  machine/gen/glue_functions.cpp
                  machine/gen/jit_functions.cpp
                  machine/gen/invoke_functions.cpp
                  machine/gen/accessor_functions.cpp
                  machine/gen/glue_resolver.cpp
                  machine/gen/jit_resolver.cpp
                  machine/gen/invoke_resolver.cpp ]

GENERATED = %W[ machine/gen/config_variables.h
                machine/gen/signature.h
                machine/dtrace/probes.h
                #{encoding_database}
                #{transcoders_database}
                #{vm_release_h}
              ] + TYPE_GEN + INSN_GEN

# Files are in order based on dependencies. For example,
# CompactLookupTable inherits from Tuple, so the header
# for compactlookuptable.hpp has to come after tuple.hpp
field_extract_headers = %w[
  machine/builtin/basic_object.hpp
  machine/builtin/object.hpp
  machine/builtin/integer.hpp
  machine/builtin/fixnum.hpp
  machine/builtin/array.hpp
  machine/builtin/bignum.hpp
  machine/builtin/executable.hpp
  machine/builtin/access_variable.hpp
  machine/builtin/alias.hpp
  machine/builtin/block_environment.hpp
  machine/builtin/block_as_method.hpp
  machine/builtin/byte_array.hpp
  machine/builtin/io.hpp
  machine/builtin/channel.hpp
  machine/builtin/module.hpp
  machine/builtin/constant_table.hpp
  machine/builtin/class.hpp
  machine/builtin/compiled_code.hpp
  machine/builtin/data.hpp
  machine/builtin/dir.hpp
  machine/builtin/exception.hpp
  machine/builtin/float.hpp
  machine/builtin/fsevent.hpp
  machine/builtin/immediates.hpp
  machine/builtin/iseq.hpp
  machine/builtin/list.hpp
  machine/builtin/logger.hpp
  machine/builtin/lookup_table.hpp
  machine/builtin/ffi_pointer.hpp
  machine/builtin/method_table.hpp
  machine/builtin/native_function.hpp
  machine/builtin/packed_object.hpp
  machine/builtin/randomizer.hpp
  machine/builtin/regexp.hpp
  machine/builtin/constant_scope.hpp
  machine/builtin/encoding.hpp
  machine/builtin/string.hpp
  machine/builtin/symbol.hpp
  machine/builtin/thread.hpp
  machine/builtin/tuple.hpp
  machine/builtin/compact_lookup_table.hpp
  machine/builtin/time.hpp
  machine/builtin/stat.hpp
  machine/builtin/native_method.hpp
  machine/builtin/system.hpp
  machine/builtin/autoload.hpp
  machine/builtin/proc.hpp
  machine/builtin/variable_scope.hpp
  machine/builtin/location.hpp
  machine/builtin/constant_cache.hpp
  machine/builtin/call_site.hpp
  machine/builtin/weakref.hpp
  machine/builtin/fiber.hpp
  machine/builtin/thunk.hpp
  machine/builtin/atomic.hpp
  machine/builtin/character.hpp
  machine/builtin/thread_state.hpp
  machine/builtin/jit.hpp
  machine/builtin/code_db.hpp
  machine/builtin/diagnostics.hpp
  machine/builtin/trie.hpp
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
    core:build
    stage:core
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

directory "machine/gen"

file 'machine/codegen/field_extract.rb'    => 'machine/gen'

task :run_field_extract do
  ruby 'machine/codegen/field_extract.rb', *field_extract_headers
end

files TYPE_GEN, field_extract_headers + %w[machine/codegen/field_extract.rb] + [:run_field_extract] do
end

encoding_extract = 'machine/codegen/encoding_extract.rb'

file encoding_database => encoding_extract do |t|
  dir = File.expand_path "../../vendor/oniguruma", __FILE__
  ruby encoding_extract, dir, t.name
end

transcoders_extract = 'machine/codegen/transcoders_extract.rb'

file transcoders_database => [transcoders_lib_dir, transcoders_extract] do |t|
  ruby transcoders_extract, transcoders_src_dir, t.name
end

task vm_release_h do |t|
  write_release t.name
end

file 'machine/gen/config_variables.h' => %w[library/rubinius/configuration.rb config.rb] do |t|
  puts "GEN #{t.name}"
  ruby 'machine/codegen/config_vars.rb', t.name
end

file 'machine/dtrace/probes.h' do |t|
  if Rubinius::BUILD_CONFIG[:dtrace]
    sh %[dtrace -h -o machine/dtrace/probes.h -s machine/dtrace/probes.d]
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

task 'machine/test/runner' => GENERATED do
  blueprint = Daedalus.load "rakelib/blueprint.rb"
  blueprint.build "machine/test/runner", @parallel_jobs
end

# Generate files for instructions and interpreters

file "gen/method_primitives.cpp" => field_extract_headers
file "gen/jit_primitives.cpp" => field_extract_headers
file "gen/invoke_primitives.cpp" => field_extract_headers
file "gen/accessor_primitives.cpp" => field_extract_headers
file "gen/method_resolver.cpp" => field_extract_headers
file "gen/jit_resolver.cpp" => field_extract_headers
file "gen/invoke_resolver.cpp" => field_extract_headers

iparser = InstructionParser.new "machine/instructions.def"

def generate_instruction_file(parser, generator, name)
  puts "GEN #{name}"
  parser.parse
  parser.send generator, name
end

insn_deps = %w[  machine/gen
                 machine/instructions.def
                 rakelib/instruction_parser.rb
              ]

file "lib/compiler/opcodes.rb" => insn_deps do |t|
  generate_instruction_file iparser, :generate_opcodes, t.name
end

file "lib/compiler/generator_methods.rb" => insn_deps do |t|
  generate_instruction_file iparser, :generate_generator_methods, t.name
end

file "machine/gen/instruction_names.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_names_header, t.name
end

file "machine/gen/instruction_names.cpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_names, t.name
end

file "machine/gen/instruction_prototypes.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_prototypes, t.name
end

file "machine/gen/instruction_sizes.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_sizes, t.name
end

file "machine/gen/instruction_defines.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_defines, t.name
end

file "machine/gen/instruction_locations.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_locations, t.name
end

file "machine/gen/instruction_implementations.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_implementations , t.name
end

file "machine/gen/instruction_visitors.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_visitors, t.name
end

file "machine/gen/instruction_effects.hpp" => insn_deps do |t|
  generate_instruction_file iparser, :generate_stack_effects, t.name
end

namespace :vm do
  desc 'Run all VM tests.  Uses its argument as a filter of tests to run.'
  task :test, :filter do |task, args|
    ENV['SUITE'] = args[:filter] if args[:filter]
    ENV['VERBOSE'] = '1' if $verbose
    sh 'machine/test/runner', :verbose => $verbose
  end

  task :test => %w[ machine/test/runner ]

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
      'machine/dtrace/probes.o',
      'machine/gen/*',
      'machine/test/runner',
      'machine/test/runner.cpp',
      'machine/test/runner.o',
      VM_EXE,
      BUILD_CONFIG[:program_name],
      'bin/rbx',
      'bin/ruby',
      'bin/rake',
      'bin/ri',
      'bin/rdoc',
      'bin/irb',
      'bin/gem',
      'machine/.deps',
    ].exclude("machine/gen/config.h", "machine/gen/paths.h")

    files.each do |filename|
      rm_rf filename, :verbose => $verbose
    end
  end

  desc "Clean up, including all external libs"
  task :distclean => :clean
end
