# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

if ENV["RUBYLIB"]
  STDERR.puts "ERROR: Please unset RUBYLIB to build Rubinius"
  exit 1
end

# Wipe out CDPATH, it interferes with building in some cases,
# see http://github.com/evanphx/rubinius/issues#issue/555
if ENV["CDPATH"]
  ENV.delete("CDPATH")
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

unless BUILD_CONFIG[:config_version] == 29
  STDERR.puts "Your configuration is outdated, please run ./configure first"
  exit 1
end

# Yes, this is duplicated from the configure script for now.
unless BUILD_CONFIG[:which_ruby] == :ruby or BUILD_CONFIG[:which_ruby] == :rbx
  STDERR.puts "Sorry, building Rubinius requires MRI or Rubinius"
  exit 1
end

# Records the full path to the ruby executable that runs this configure
# script. That path will be made available to the rest of the build system
# so the same version of ruby is invoked as needed.
#
# This is duplicated from the configure script for now.
@build_ruby = nil

def build_ruby
  unless @build_ruby
    bin = RbConfig::CONFIG["RUBY_INSTALL_NAME"] || RbConfig::CONFIG["ruby_install_name"]
    bin += (RbConfig::CONFIG['EXEEXT'] || RbConfig::CONFIG['exeext'] || '')
    @build_ruby = File.join(RbConfig::CONFIG['bindir'], bin)
  end
  @build_ruby
end

unless BUILD_CONFIG[:build_ruby] == build_ruby
  STDERR.puts "\nUnable to build using the running Ruby executable.\n\n"

  STDERR.puts "To resolve this issue:"
  if ENV['PATH'] =~ /#{BUILD_CONFIG[:bindir]}/
    STDERR.puts "  * Remove '#{BUILD_CONFIG[:bindir]}' from your PATH."
  elsif build_ruby == File.join(BUILD_CONFIG[:bindir], BUILD_CONFIG[:program_name])
    # This may occur using rbx from the build directory to build a version
    # of rbx to install. The rbx in the build directory will pick up the
    # lib/rubinius/build_config.rb that was just written by configure.
    # Obviously, this chewing gum, duct tape, bailing wire, and toilet paper
    # system needs fixing.
    STDERR.puts "  * Configure using a Ruby executable other than the one in your build directory."
  else
    STDERR.puts "  * Use '#{BUILD_CONFIG[:build_ruby]}' to build."
  end

  exit 1
end

# Set the build compiler to the configured compiler unless
# the compiler is set via CC environment variable.
ENV['CC'] = BUILD_CONFIG[:cc] unless ENV['CC']
ENV['CXX'] = BUILD_CONFIG[:cxx] unless ENV['CXX']

$dlext = RbConfig::CONFIG["DLEXT"]

task :default => %w[build vm:test] do
  unless File.directory? BUILD_CONFIG[:runtime]
    # Setting these enables the specs to run when rbx has been configured
    # to be installed, but rake install has not been run yet.
    ENV["RBX_RUNTIME"] = File.expand_path "../runtime", __FILE__
    ENV["RBX_LIB"]     = File.expand_path "../lib", __FILE__
    ENV["CFLAGS"]      = "-Ivm/capi"
  end

  sh "bin/mspec ci --background --agent"
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => ["build:build", "gem_bootstrap"]

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc "Use to run Rubinius in CI"
task :ci do
  unless system("rake -q")
    puts "<< ERROR IN CI, CLEANING AND RERUNNING >>"
    system "rake -q clean"
    system "find . -name *.rbc -delete"
    sh "rake -q"
  end
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
