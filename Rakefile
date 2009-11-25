# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

$trace ||= false
$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")

if !$verbose and respond_to?(:verbose)
  verbose(false) if verbose() == :default
end

unless File.exists?(File.expand_path("../config.rb", __FILE__)) and
       File.exists?(File.expand_path("../vm/gen/config.h", __FILE__))
  STDERR.puts "Please run ./configure first"
  exit 1
end

$dlext = Config::CONFIG["DLEXT"]
$compiler = nil

RUBINIUS_BASE = File.expand_path(File.dirname(__FILE__))

$: << "lib"

task :default => %w[build vm:test] do
  sh "bin/mspec ci -B full --background"
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => ["build:normal", "gem_bootstrap"]

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc 'Remove rubinius build files'
task :clean => %w[
  vm:clean
  kernel:clean
  clean:crap
  extensions:clean
]

desc 'Remove rubinius build files and external library build files'
task :distclean => %w[
  clean
  vm:distclean
]

desc 'Move the preinstalled gem setup into place'
task :gem_bootstrap do
  unless File.directory?("gems/rubinius")
    sh "mkdir gems"
    sh "cp -r preinstalled-gems/* gems/"
  end
end

def install_bin
  File.join RBX_BINPATH, 'rbx'
end

desc "Uninstall Rubinius"
task :uninstall do
  rm_rf install_bin
  rm_rf RBX_BASE_PATH
  sh "rake -q clean"
end

desc "Install Rubinius"
task :install => %w[
  clean
  install:build
  install:files
] do
  sh "rake -q clean"
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
    sh "rake -q build"
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


    capi_header_files = Rake::FileList.new "vm/capi/*.h"
    install_files capi_header_files, RBX_EXT_PATH

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
      begin
        compile_ruby rb, "#{rb}c"
      rescue Object => e
        puts "Error compiling #{rb}!"
        puts e.backtrace
        puts "Install continuing but please file a ticket"
      end
    end

    install 'vm/vm', install_bin, :mode => 0755, :verbose => true
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
  Use bin/mspec directly. MSpec provides 'pseudo-directories',
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
