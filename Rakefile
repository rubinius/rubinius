require './rakelib/configure'
require './rakelib/build_signature'

include Rake::DSL if Rake.const_defined? :DSL

# Wipe out CDPATH, it interferes with building in some cases,
# see http://github.com/rubinius/rubinius/issues#issue/555
if ENV["CDPATH"]
  ENV.delete("CDPATH")
end

# Wipe out RUBYGEMS_GEMDEPS, it causes the build to fail with
# "no such file to load -- tsort" when running rbx extconf.rb
if ENV["RUBYGEMS_GEMDEPS"]
  ENV.delete("RUBYGEMS_GEMDEPS")
end

$trace ||= false
$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")
$cleaning = Rake.application.top_level_tasks.include?("clean")

if !$verbose and respond_to?(:verbose)
  verbose(false) if verbose() == :default
end

$:.unshift File.expand_path("../", __FILE__)
$:.unshift File.expand_path("../build/scripts", __FILE__)

BUILD_CONFIG = {} unless Object.const_defined? :BUILD_CONFIG

def load_configuration
  config_rb = File.expand_path "../build/config/config.rb", __FILE__
  config_h  = File.expand_path "../machine/config.h", __FILE__

  unless File.exist?(config_rb) and File.exist?(config_h)
    if $cleaning
      exit 0
    else
      sh "./configure"
      return load_configuration
    end
  end

  load config_rb
  BUILD_CONFIG.replace Rubinius::BUILD_CONFIG
end

load_configuration

unless verify_build_signature or $cleaning or ENV["RBX_IGNORE_BUILD_SIGNATURE"]
  STDERR.puts "Your configuration is outdated, please run ./configure first"
  exit 1
end

unless ENV["RBX_SUPRESS_DEPRECATION"]
  STDERR.puts "              *** DEPRECATION NOTICE ***" \
    "\n\nUse of Rake is deprecated and will be removed in the future.\n" \
    "Use 'build.sh' to configure, build, package, and install Rubinius.\n\n\n"
end

if BUILD_CONFIG[:build_bin]
  ENV["PATH"] = "#{BUILD_CONFIG[:build_bin]}:#{ENV["PATH"]}"
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
  @flags = nil

  def self.at_exit_status
    @at_exit_status
  end

  def self.set_at_exit_handler
    return if @at_exit_handler_set

    at_exit { exit SpecRunner.at_exit_status }
    @at_exit_handler_set = true
  end

  def self.set_at_exit_status(status)
    @at_exit_status = status || 1
  end

  def self.flags
    @flags
  end

  def self.flags=(value)
    @flags = value
  end

  def initialize
    @handler = lambda do |ok, status|
      self.class.set_at_exit_status(status.exitstatus) unless ok
    end
    @start_time = Time.now
  end

  def elapsed_time
    puts "Running for #{Time.now - @start_time} seconds"
  end

  def run(suite=:ci_files)
    self.class.set_at_exit_handler

    cmd = "bin/mspec ci #{suite} #{self.class.flags} -t bin/#{BUILD_CONFIG[:program_name]} -d --background"
    puts cmd

    sh(cmd, &@handler)
  end
end

task :default => [:spec, :install]

def clean_environment
  ENV['GEM_PATH'] = ENV['GEM_HOME'] = nil
  ENV['RUBYOPT'] = "--disable-gems"
end

# See vm.rake for more information
desc "Build Rubinius"
task :build => %w[build:build]

desc "Recompile all ruby system files"
task :rebuild => %w[clean build]

desc 'Remove rubinius build files'
task :clean => %w[
  vm:clean
  core:clean
]

desc 'Remove rubinius build files and external library build files'
task :distclean => %w[
  clean
  core:distclean
  vm:distclean
]

desc "Run specs in default (configured) mode but do not rebuild on failure"
task :spec => %w[build] do
  clean_environment

  spec_runner = SpecRunner.new

  %w[spec/language
     spec/core
     spec/command_line
     spec/instructions
     spec/jit
     spec/ruby/command_line
     spec/ruby/language
     spec/ruby/core/argf
     spec/ruby/core/array
     spec/ruby/core/basicobject
     spec/ruby/core/bignum
     spec/ruby/core/binding
     spec/ruby/core/builtin_constants
     spec/ruby/core/class
     spec/ruby/core/comparable
     spec/ruby/core/complex
     spec/ruby/core/continuation
     spec/ruby/core/dir
     spec/ruby/core/encoding
     spec/ruby/core/enumerable
     spec/ruby/core/enumerator
     spec/ruby/core/env
     spec/ruby/core/exception
     spec/ruby/core/false
     spec/ruby/core/fiber
     spec/ruby/core/file
     spec/ruby/core/filetest
     spec/ruby/core/fixnum
     spec/ruby/core/float
     spec/ruby/core/gc
     spec/ruby/core/hash
     spec/ruby/core/integer
     spec/ruby/core/io
     spec/ruby/core/kernel
     spec/ruby/core/main
     spec/ruby/core/marshal
     spec/ruby/core/matchdata
     spec/ruby/core/math
     spec/ruby/core/method
     spec/ruby/core/module
     spec/ruby/core/mutex
     spec/ruby/core/nil
     spec/ruby/core/numeric
     spec/ruby/core/object
     spec/ruby/core/objectspace
     spec/ruby/core/proc
     spec/ruby/core/process
     spec/ruby/core/random
     spec/ruby/core/range
     spec/ruby/core/rational
     spec/ruby/core/regexp
     spec/ruby/core/signal
     spec/ruby/core/string
     spec/ruby/core/struct
     spec/ruby/core/symbol
     spec/ruby/core/systemexit
     spec/ruby/core/thread
     spec/ruby/core/threadgroup
     spec/ruby/core/time
     spec/ruby/core/true
     spec/ruby/core/unboundmethod
     spec/ruby/optional/capi
     spec/library
  ].each do |path|
    spec_runner.run path
  end

  spec_runner.elapsed_time

  exit 1 unless SpecRunner.at_exit_status == 0
end

desc "Run specs as in the spec task, but with CI formatting"
task :ci do
  SpecRunner.flags = "-V" # show spec file names
  Rake::Task["spec"].invoke
end
