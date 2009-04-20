# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

$trace ||= false
$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")

if !$verbose and respond_to?(:verbose)
  verbose(false) if verbose() == :default
end

$dlext = Config::CONFIG["DLEXT"]
$compiler = nil

RUBINIUS_BASE = File.expand_path(File.dirname(__FILE__))

$: << "lib"

task :default => %w[build vm:test] do
  system "bin/mspec ci -B full --background"
end

desc "Compile the given ruby file into a .rbc file"
task :compile_ruby, :file do |task, args|
  file = args[:file]
  raise ArgumentError, 'compile_ruby requires a file name' if file.nil?

  rbc = file + 'c'

  compile_ruby file, rbc
end
task :compile_ruby => 'kernel:build' # HACK argument + dependency is broken

desc "Run the given ruby file with the vm"
task :run_ruby, :file do |task, args|
  file = args[:file]
  raise ArgumentError, 'compile_ruby requires a file name' if file.nil?

  rbc = file + 'c'

  compile_ruby file, rbc

  ENV['PROBE'] = 'yes' if $verbose
  ENV['RBX_RUNTIME'] = File.join File.dirname(__FILE__), 'runtime'

  sh 'vm/vm', rbc
end
task :run_ruby => %w[kernel:build vm/vm] # HACK argument + dependency is broken

desc "Compile files with MRI because vm takes forever to do it"
task :precompile => %w[extensions] do
  FileList['{mspec,kernel,lib,spec}/**/*.rb'].each do |rb|
    next unless File.file? rb

    rbc = "#{rb}c"

    if File.exist? rbc then
      rbc_mtime = File.mtime rbc
      next if rbc_mtime >= COMPILER_MTIME or rbc_mtime >= File.mtime(rb)
    end

    mri_compile rb, rbc
  end
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => "build:normal"

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc 'Remove rubinius build files'
task :clean => %w[
  vm:clean
  kernel:clean
  clean:crap
  extension:clean
  configure:clean
]

desc 'Remove rubinius build files and external library build files'
task :distclean => %w[
  clean
  vm:distclean
]

def install_bin
  File.join RBX_BINPATH, 'rbx'
end

desc "Uninstall Rubinius"
task :uninstall do
  rm_rf install_bin
  rm_rf RBX_BASE_PATH
  sh "rake clean"
end

desc "Install Rubinius"
task :install => %w[
  clean
  install:build
  install:files
] do
  sh "rake clean"
  puts "Install complete."
  puts "The install versions of files have been cleaned."
  puts "Run 'rake build' to rebuild development versions."
end

namespace :install do
  # Internal task, not documented with desc. Shells out
  # to perform the build. See reason in doc/build_system.txt.
  task :build do
    ENV['RBX_PREFIX'] = ENV['PREFIX'] || "/usr/local"
    ENV['RBX_RUNTIME'] = File.join(Dir.pwd, 'runtime')
    sh "rake build"
  end

  # Internal task, not documented with desc. Performs the
  # actual file installation enabling the :install task
  # to clean up after itself.
  task :files do
    mkdir_p RBX_BASE_PATH, :verbose => true
    mkdir_p RBX_RBA_PATH, :verbose => true
    mkdir_p RBX_EXT_PATH, :verbose => true
    mkdir_p RBX_BIN_PATH, :verbose => true
    mkdir_p RBX_LIB_PATH, :verbose => true

    install "vm/subtend/ruby.h", File.join(RBX_EXT_PATH, "ruby.h"),
      :mode => 0644, :verbose => true

    File.open File.join(RBX_EXT_PATH, "defines.h"), "w" do |f|
      f.puts "// This file left empty"
    end

    File.open File.join(RBX_EXT_PATH, "missing.h"), "w" do |f|
      f.puts "// This file left empty"
    end

    core_files = Rake::FileList.new('runtime/index',
                                    'runtime/platform.conf',
                                    'runtime/**/*.rb{a,c}',
                                    'runtime/**/load_order.txt')
    install_files core_files, RBX_RBA_PATH

    lib_files = Rake::FileList.new 'lib/**/*'
    install_files lib_files, RBX_LIB_PATH

    Rake::FileList.new("#{RBX_LIB_PATH}/**/*.rb").sort.each do |rb|
      compile_ruby rb, "#{rb}c"
    end

    install 'vm/vm', install_bin, :mode => 0755, :verbose => true
  end
end

def rbx_compile(from)
  sh "bin/rbx compile -f #{from}"
end

desc 'Compare an rbc file made by MRI with one made by rbx'
task :compare, :file do |task, args|
  file = args[:file]
  raise "Please supply something that exists" unless File.exist? file

  mri_compile file, file + 'c.mri'
  rbx_compile file

  File.open(file + 'c.mri') do |mri|
    File.open(file + 'c') do |rbx|
      while m = mri.gets and r = rbx.gets
        puts "Line of Failure: #{rbx.lineno}" and break unless m === r
      end
    end
  end

end

namespace :clean do
  desc "Cleans up editor files and other misc crap"
  task :crap do
    files = (Dir["*~"] + Dir["**/*~"]).uniq

    rm_f files, :verbose => $verbose unless files.empty?
  end
end

desc "Documents why no spec tasks exist"
task :spec do
  puts <<-EOM

  The spec and spec:xxx commands are deprecated (and removed).
  Use bin/mspec directly. MSpec provides 'psuedo-directories',
  which are labels that refer to sets of specs to run. Refer
  to spec/default.mspec, spec/full.mspec and the MSpec docs
  for full details.

  The following are likely scenarios for running the specs.
  Unless -t <target> is passed to mspec, bin/rbx is run.

  Run the CI specs that are run with the default 'rake' command

    bin/mspec ci

  Run _all_ the CI spec:

    bin/mspec ci -B full

  Run all the frozen specs:

    bin/mspec

  Run all the frozen Array specs:

    bin/mspec core/array
      OR
    bin/mspec spec/frozen/core/array

  Run spec/frozen/core/array/append_spec.rb:

    bin/mspec core/array/append
      OR
    bin/mspec spec/frozen/core/array/append_spec.rb

  Run all the compiler specs:

    bin/mspec :compiler

  Run all the [language, core, library, capi] specs:

    bin/mspec :language
    bin/mspec :core
    ...

  Run all the spec/ruby specs using the MRI on your path
  (assuming you have run 'rake rubyspec:update'):

    bin/mspec -tr :ruby

  EOM
end
