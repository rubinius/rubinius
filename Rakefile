# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

if ENV["RUBYLIB"]
  STDERR.puts "ERROR: Please unset RUBYLIB to build Rubinius"
  exit 1
end

$trace ||= false
$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")

if !$verbose and respond_to?(:verbose)
  verbose(false) if verbose() == :default
end

$:.unshift File.expand_path("../", __FILE__)

config_rb = File.expand_path "../config.rb", __FILE__
config_h  = File.expand_path "../vm/gen/config.h", __FILE__

unless File.exists?(config_rb) and File.exists?(config_h)
  STDERR.puts "Please run ./configure first"
  exit 1
end

require config_rb
BUILD_CONFIG = Rubinius::BUILD_CONFIG

unless BUILD_CONFIG[:config_version] == 21
  STDERR.puts "Your configuration is outdated, please run ./configure first"
  exit 1
end

# Yes, this is duplicated from the configure script for now.
unless BUILD_CONFIG[:which_ruby] == :ruby or BUILD_CONFIG[:which_ruby] == :rbx
  STDERR.puts "Sorry, building Rubinius requires MRI or Rubinius"
  exit 1
end

bin = RbConfig::CONFIG["RUBY_INSTALL_NAME"] || RbConfig::CONFIG["ruby_install_name"]
bin << (RbConfig::CONFIG['EXEEXT'] || RbConfig::CONFIG['exeext'] || '')
build_ruby = File.join(RbConfig::CONFIG['bindir'], bin)

unless BUILD_CONFIG[:build_ruby] == build_ruby
  STDERR.puts "Sorry, but you need to build with the same Ruby version it was configured with"
  STDERR.puts "Please run ./configure again"
  exit 1
end

# Set the build compiler to the configured compiler unless
# the compiler is set via CC environment variable.
ENV['CC'] = BUILD_CONFIG[:compiler] unless ENV['CC']
ENV['CXX'] = BUILD_CONFIG[:compiler] unless ENV['CXX']

$dlext = RbConfig::CONFIG["DLEXT"]

task :default => %w[build vm:test] do
  unless File.directory? BUILD_CONFIG[:runtime]
    # Setting these enables the specs to run when rbx has been configured
    # to be installed, but rake install has not been run yet.
    ENV["RBX_RUNTIME"] = File.expand_path "../runtime", __FILE__
    ENV["RBX_LIB"]     = File.expand_path "../lib", __FILE__
    ENV["CFLAGS"]      = "-Ivm/capi"
  end

  sh "bin/mspec ci #{ENV['CI_MODE_FLAG']} --background --agent"
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => ["build:normal", "gem_bootstrap"]

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

def run_ci
  unless system("rake -q")
    puts "<< ERROR IN CI, CLEANING AND RERUNNING >>"
    system "rake -q clean"
    system "find . -name *.rbc -delete"
    sh "rake -q"
  end
end

desc "Run CI in default (configured) mode"
task :ci do
  run_ci
end

# These tasks run the specs in the specified mode regardless of
# the default mode with which Rubinius was configured.
desc "Run CI in 1.8 mode"
task :ci18 do
  ENV['CI_MODE_FLAG'] = "-T -X19=no"
  run_ci
end

desc "Run CI in 1.9 mode"
task :ci19 do
  ENV['CI_MODE_FLAG'] = "-T -X19"
  run_ci
end

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

namespace :clean do
  desc "Cleans up editor files and other misc crap"
  task :crap do
    files = (Dir["*~"] + Dir["**/*~"]).uniq

    rm_f files, :verbose => $verbose unless files.empty?
  end
end

desc 'Move the preinstalled gem setup into place'
task :gem_bootstrap do
  unless File.directory?("gems/rubinius")
    sh "mkdir -p gems/rubinius"
    sh "cp -r preinstalled-gems/bin gems/bin"
    sh "cp -r preinstalled-gems/data gems/rubinius/preinstalled"
  end
end

desc "Run the Rubinius documentation website"
task :docs do
  require 'kernel/delta/options'
  require 'rbconfig'
  require 'webrick'
  require 'lib/rubinius/documentation'

  Rubinius::Documentation.main
end

desc "Documents why no spec tasks exist"
task :spec do
  puts <<-EOM

  The spec and spec:xxx commands are deprecated (and removed).
  Use bin/mspec directly. MSpec provides 'pseudo-directories',
  which are labels that refer to sets of specs to run. Refer
  to spec/default.mspec and the MSpec docs for full details.

  The following are likely scenarios for running the specs.
  Unless -t <target> is passed to mspec, bin/rbx is run.

  Run the CI specs that are run with the default 'rake' command

    bin/mspec ci

  Run all the RubySpec specs but not Rubinius-specific ones:

    bin/mspec

  Run all the RubySpec Array specs:

    bin/mspec core/array
      OR
    bin/mspec spec/ruby/core/array

  Run spec/ruby/core/array/append_spec.rb:

    bin/mspec core/array/append
      OR
    bin/mspec spec/ruby/core/array/append_spec.rb

  Run all the compiler specs:

    bin/mspec :compiler

  Run all the [language, core, library, capi] specs:

    bin/mspec :language
    bin/mspec :core
    ...

  Run all the spec/ruby specs using the 'ruby' executable on your path

    bin/mspec -tr :ruby

  EOM
end

desc "Print list of items marked to-do in kernel/ (@todo|TODO)"
task :todos do
  sh "grep", "-Rn", "@todo", "kernel"
  sh "grep", "-Rn", "TODO", "kernel"
end
