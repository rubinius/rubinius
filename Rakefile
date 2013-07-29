# NOTE! When updating this file, also update INSTALL, if necessary.
# NOTE! Please keep your tasks grouped together.

require './rakelib/configure'

include Rake::DSL if Rake.const_defined? :DSL

if ENV["RUBYLIB"]
  STDERR.puts <<-EOM
WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
WARNING                                                                 WARNING
WARNING   You have the RUBYLIB environment variable set. This can       WARNING
WARNING   cause serious problems building Rubinius, including but       WARNING
WARNING   not limited to causing the build to fail or specs to fail     WARNING
WARNING   or your computer to randomly emit strange beeping sounds      WARNING
WARNING   or burst into flames. Not all these possible catastrophic     WARNING
WARNING   effects have been observed in the wild, but you have been     WARNING
WARNING   warned. We recommend unsetting this environment variable      WARNING
WARNING   and running the build again.                                  WARNING
WARNING                                                                 WARNING
WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  EOM
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

BUILD_CONFIG = {} unless Object.const_defined? :BUILD_CONFIG

def load_configuration
  config_rb = File.expand_path "../config.rb", __FILE__
  config_h  = File.expand_path "../vm/gen/config.h", __FILE__

  unless File.exists?(config_rb) and File.exists?(config_h)
    STDERR.puts "Please run ./configure first"
    exit 1
  end

  load config_rb
  BUILD_CONFIG.replace Rubinius::BUILD_CONFIG
end

load_configuration

unless BUILD_CONFIG[:config_version] == 179
  STDERR.puts "Your configuration is outdated, please run ./configure first"
  exit 1
end

# Yes, this is duplicated from the configure script for now.
unless BUILD_CONFIG[:which_ruby] == :ruby or BUILD_CONFIG[:which_ruby] == :rbx
  STDERR.puts "Sorry, building Rubinius requires MRI or Rubinius"
  exit 1
end

def libprefixdir
  if BUILD_CONFIG[:stagingdir]
    "#{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:libdir]}"
  else
    "#{BUILD_CONFIG[:sourcedir]}/lib"
  end
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
  STDERR.puts "\nUnable to build using the running Ruby executable (#{build_ruby}). Expected #{BUILD_CONFIG[:build_ruby]}\n\n"

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

class SpecRunner

  @at_exit_handler_set = false
  @at_exit_status = 0

  def self.at_exit_status
    @at_exit_status
  end

  def self.set_at_exit_handler
    return if @at_exit_handler_set

    at_exit { exit SpecRunner.at_exit_status }
    @at_exit_handler_set = true
  end

  def self.set_at_exit_status(status)
    @at_exit_status = status
  end

  def initialize
    unless File.directory? BUILD_CONFIG[:runtimedir]
      # Setting these enables the specs to run when rbx has been configured
      # to be installed, but rake install has not been run yet.
      ENV["RBX_RUNTIME"] = File.expand_path "../runtime", __FILE__
      ENV["RBX_LIB"]     = File.expand_path "../lib", __FILE__
      ENV["CFLAGS"]      = "-Ivm/capi"
    end

    ENV.delete("RUBYOPT")

    @handler = lambda do |ok, status|
      self.class.set_at_exit_status(status.exitstatus) unless ok
    end
  end

  def run(suite=:ci_files, flags=nil)
    self.class.set_at_exit_handler

    sh("bin/mspec ci :#{suite} #{ENV['CI_MODE_FLAG'] || flags} -d --agent --background", &@handler)
  end
end

if BUILD_CONFIG[:stagingdir]
  task :default => [:spec, :check_status, :install]
else
  task :default => :spec
end

def check_status
  exit 1 unless SpecRunner.at_exit_status == 0
end

task :check_status do
  check_status
end

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
desc "Build Rubinius"
task :build => %w[build:build gems:install]

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

namespace :clean do
  desc "Cleans up editor files and other misc crap"
  task :crap do
    files = (Dir["*~"] + Dir["**/*~"]).uniq

    rm_f files, :verbose => $verbose unless files.empty?
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

spec_runner = SpecRunner.new

desc "Run CI in default (configured) mode but do not rebuild on failure"
task :spec => %w[build vm:test] do
  spec_runner.run
end

task :travis do
  BUILD_CONFIG[:supported_versions].each do |version|
    sh "./configure --enable-version=#{version}"
    rm_rf BUILD_CONFIG[:prefixdir] + BUILD_CONFIG[:gemsdir]
    sh "rake extensions:clean build vm:test"
    spec_runner.run :travis
  end
  check_status
end

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
