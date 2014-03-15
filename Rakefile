require 'bundler/setup'
require 'redcard'
require './rakelib/configure'
require './rakelib/build_signature'

include Rake::DSL if Rake.const_defined? :DSL

# Wipe out CDPATH, it interferes with building in some cases,
# see http://github.com/rubinius/rubinius/issues#issue/555
if ENV["CDPATH"]
  ENV.delete("CDPATH")
end

$trace ||= false
$VERBOSE = true
$verbose = Rake.application.options.trace || ARGV.delete("-v")
$cleaning = Rake.application.top_level_tasks.include?("clean")

if !$verbose and respond_to?(:verbose)
  verbose(false) if verbose() == :default
end

$:.unshift File.expand_path("../", __FILE__)

BUILD_CONFIG = {} unless Object.const_defined? :BUILD_CONFIG

def load_configuration
  config_rb = File.expand_path "../config.rb", __FILE__
  config_h  = File.expand_path "../vm/gen/config.h", __FILE__

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

unless verify_build_signature or $cleaning
  STDERR.puts "Your configuration is outdated, please run ./configure first"
  exit 1
end

# Yes, this is duplicated from the configure script for now.
unless RedCard.check :ruby, :rubinius
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
    @handler = lambda do |ok, status|
      self.class.set_at_exit_status(status.exitstatus) unless ok
    end
  end

  def run(suite=:ci_files, flags=nil)
    self.class.set_at_exit_handler

    sh("bin/mspec ci :#{suite} #{ENV['CI_MODE_FLAG'] || flags} -t bin/#{BUILD_CONFIG[:program_name]} -d --agent --background", &@handler)
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

def clean_environment
  ENV['GEM_PATH'] = ENV['GEM_HOME'] = ENV['RUBYOPT'] = nil
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

desc "Run CI in default (configured) mode but do not rebuild on failure"
task :spec => %w[build vm:test] do
  clean_environment

  spec_runner = SpecRunner.new
  spec_runner.run
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
