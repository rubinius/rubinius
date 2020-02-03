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

encoding_database = "machine/encoding_database.hpp"
transcoders_database = "machine/transcoder_database.hpp"

vm_release_h = BUILD_CONFIG[:vm_release_h]

ENV.delete 'CDPATH' # confuses llvm_config

TYPE_GEN    = %w[ machine/includes.hpp
                  machine/kind_of.hpp
                  machine/object_types.hpp
                  machine/typechecks.hpp
                  machine/primitives_declare.hpp
                  machine/invoke_functions.hpp
                  machine/accessor_functions.hpp
                  machine/invoke_resolver.hpp ]

GENERATED = %W[ machine/config_variables.h
                machine/signature.h
                machine/dtrace/probes.h
                #{encoding_database}
                #{transcoders_database}
                #{vm_release_h}
              ] + TYPE_GEN

# Files are in order based on dependencies. For example,
# CompactLookupTable inherits from Tuple, so the header
# for compactlookuptable.hpp has to come after tuple.hpp
field_extract_headers = %w[
  machine/class/basic_object.hpp
  machine/class/object.hpp
  machine/class/integer.hpp
  machine/class/fixnum.hpp
  machine/class/array.hpp
  machine/class/bignum.hpp
  machine/class/executable.hpp
  machine/class/access_variable.hpp
  machine/class/alias.hpp
  machine/class/block_environment.hpp
  machine/class/block_as_method.hpp
  machine/class/byte_array.hpp
  machine/class/io.hpp
  machine/class/channel.hpp
  machine/class/module.hpp
  machine/class/constant_table.hpp
  machine/class/class.hpp
  machine/class/compiled_code.hpp
  machine/class/data.hpp
  machine/class/dir.hpp
  machine/class/exception.hpp
  machine/class/float.hpp
  machine/class/fsevent.hpp
  machine/class/immediates.hpp
  machine/class/iseq.hpp
  machine/class/list.hpp
  machine/class/logger.hpp
  machine/class/lookup_table.hpp
  machine/class/ffi_pointer.hpp
  machine/class/method_table.hpp
  machine/class/native_function.hpp
  machine/class/packed_object.hpp
  machine/class/randomizer.hpp
  machine/class/regexp.hpp
  machine/class/lexical_scope.hpp
  machine/class/encoding.hpp
  machine/class/string.hpp
  machine/class/symbol.hpp
  machine/class/thread.hpp
  machine/class/tuple.hpp
  machine/class/compact_lookup_table.hpp
  machine/class/time.hpp
  machine/class/stat.hpp
  machine/class/native_method.hpp
  machine/class/system.hpp
  machine/class/autoload.hpp
  machine/class/proc.hpp
  machine/class/variable_scope.hpp
  machine/class/location.hpp
  machine/class/constant_cache.hpp
  machine/class/call_site.hpp
  machine/class/weakref.hpp
  machine/class/fiber.hpp
  machine/class/thunk.hpp
  machine/class/atomic.hpp
  machine/class/character.hpp
  machine/class/unwind_state.hpp
  machine/class/jit.hpp
  machine/class/code_db.hpp
  machine/class/trie.hpp
  machine/class/unwind_site.hpp
  machine/class/prediction.hpp
]

transcoders_src_dir = "#{BUILD_CONFIG[:build_libdir]}/oniguruma/enc/trans"

transcoders_lib_dir = "#{BUILD_CONFIG[:builddir]}/#{BUILD_CONFIG[:encdir]}"
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
    stage:bin
    stage:extra_bins
    stage:capi_include
    core:build
    gems:install
    stage:site
    stage:lib
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

task :run_field_extract do
  ruby 'build/scripts/field_extract.rb', *field_extract_headers
end

files TYPE_GEN, field_extract_headers + ["build/scripts/field_extract.rb", :run_field_extract] do
end

encoding_extract = 'build/scripts/encoding_extract.rb'

file encoding_database => encoding_extract do |t|
  dir = "#{BUILD_CONFIG[:build_libdir]}/oniguruma"
  ruby encoding_extract, dir, t.name
end

transcoders_extract = 'build/scripts/transcoders_extract.rb'

file transcoders_database => [transcoders_lib_dir, transcoders_extract] do |t|
  ruby transcoders_extract, transcoders_src_dir, t.name
end

task vm_release_h do |t|
  write_release t.name
end

file 'machine/config_variables.h' => %w[library/rubinius/configuration.rb build/config/config.rb] do |t|
  puts "GEN #{t.name}"
  ruby 'build/scripts/config_vars.rb', t.name
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

# Generate files for instructions and interpreters

file "method_primitives.hpp" => field_extract_headers
file "invoke_primitives.hpp" => field_extract_headers
file "accessor_primitives.hpp" => field_extract_headers
file "method_resolver.hpp" => field_extract_headers
file "invoke_resolver.hpp" => field_extract_headers

namespace :vm do
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
    ].exclude("machine/config.h", "machine/paths.h")

    files.each do |filename|
      rm_rf filename, :verbose => $verbose
    end
  end

  desc "Clean up, including all external libs"
  task :distclean => :clean
end
