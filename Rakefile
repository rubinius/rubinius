# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

if ENV["RUBYLIB"]
  STDERR.puts "ERROR: Please unset RUBYLIB to build Rubinius"
  exit 1
end

# Wipe out CDPATH, it interferes with building in some cases,
# see http://github.com/rubinius/rubinius/issues#issue/555
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

unless BUILD_CONFIG[:config_version] == 138
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
$CC = ENV['CC']

def run_specs(flags=nil)
  unless File.directory? BUILD_CONFIG[:runtime]
    # Setting these enables the specs to run when rbx has been configured
    # to be installed, but rake install has not been run yet.
    ENV["RBX_RUNTIME"] = File.expand_path "../runtime", __FILE__
    ENV["RBX_LIB"]     = File.expand_path "../lib", __FILE__
    ENV["CFLAGS"]      = "-Ivm/capi"
  end

  ENV.delete("RUBYOPT")

  sh "bin/mspec ci #{ENV['CI_MODE_FLAG'] || flags} --background --agent"
end

task :default => :spec

task :github do
  cur = `git config remote.origin.url`.strip
  if cur == "git://github.com/evanphx/rubinius.git"
    sh "git config remote.origin.url git://github.com/rubinius/rubinius.git"
    puts "\nSwitch to git://github.com/rubinius/rubinius.git"
  else
    sh "git config remote.origin.url git@github.com:rubinius/rubinius.git"
    puts "\nSwitch to github.com:rubinius/rubinius.git"
  end
end

# See vm.rake for more information
desc "Build everything that needs to be built at default level."
task :build => ["build:build", "gem_bootstrap"]

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
task :ci => %w[build vm:test] do
  run_ci
end

# These tasks run the specs in the specified mode regardless of
# the default mode with which Rubinius was configured.
desc "Run CI in 1.8 mode"
task :ci18 => %w[build vm:test] do
  ENV['CI_MODE_FLAG'] = "-T -X18"
  run_ci
end

desc "Run CI in 1.9 mode"
task :ci19 => %w[build vm:test] do
  ENV['CI_MODE_FLAG'] = "-T -X19"
  run_ci
end

desc "Run CI in 2.0 mode"
task :ci20 => %w[build vm:test] do
  ENV['CI_MODE_FLAG'] = "-T -X20"
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
  pre_gems = Dir["preinstalled-gems/data/specifications/*.gemspec"].sort
  ins_gems = Dir["gems/rubinius/specifications/*.gemspec"].sort
  unless pre_gems == ins_gems
    FileUtils.rm_rf "gems/rubinius"
    FileUtils.mkdir_p "gems/rubinius"
    FileUtils.cp_r "preinstalled-gems/bin", "gems/bin"
    FileUtils.cp_r "preinstalled-gems/data", "gems/rubinius/preinstalled"
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

desc "Run the CI specs in 1.8 mode but do not rebuild on failure"
task :spec18 => %w[build vm:test] do
  run_specs "-T -X18"
end

desc "Run the CI specs in 1.9 mode but do not rebuild on failure"
task :spec19 => %w[build vm:test] do
  run_specs "-T -X19"
end

desc "Run the CI specs in 2.0 mode but do not rebuild on failure"
task :spec20 => %w[build vm:test] do
  run_specs "-T -X20"
end

desc "Run CI in default (configured) mode but do not rebuild on failure"
task :spec => %w[spec18 spec19]

desc "Print list of items marked to-do in kernel/ (@todo|TODO)"
task :todos do

  # create array with files to be checked
  filesA = Dir['kernel/**/*.*']

  # search for @todo or TODO
  filesA.sort!.each do |filename|
    File.open(filename) do |file|
      file.each do |line|
        puts "#{filename} #{file.lineno.to_s}:\t#{line.strip}" if line.include?("@todo") or line.include?("TODO")
      end
    end
  end
end
