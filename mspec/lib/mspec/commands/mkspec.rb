#! /usr/bin/env ruby

MSPEC_HOME = File.expand_path(File.dirname(__FILE__) + '/../../..')

require 'fileutils'
require 'rbconfig'
require 'mspec/version'
require 'mspec/utils/options'
require 'mspec/utils/name_map'


class MkSpec
  attr_reader :config

  def initialize
    @config = {
      :constants => [],
      :requires  => [],
      :base      => "spec/ruby/1.8/core"
    }
    @map = NameMap.new
  end

  def options(argv=ARGV)
    options = MSpecOptions.new "mkspec [options]"

    options.on("-c", "--constant", "CONSTANT",
               "Class or Module to generate spec stubs for") do |name|
      config[:constants] << name
    end
    options.on("-b", "--base", "DIR",
               "Directory to generate specs into") do |directory|
      config[:base] = File.expand_path directory
    end
    options.on("-r", "--require", "LIBRARY",
               "A library to require") do |file|
      config[:requires] << file
    end
    options.version MSpec::VERSION
    options.help

    options.doc "\n How might this work in the real world?\n"
    options.doc "   1. To create spec stubs for every class or module in Object\n"
    options.doc "     $ mkspec\n"
    options.doc "   2. To create spec stubs for Fixnum\n"
    options.doc "     $ mkspec -c Fixnum\n"
    options.doc "   3. To create spec stubs for Complex in 'superspec/complex'\n"
    options.doc "     $ mkspec -c Complex -rcomplex -b superspec"
    options.doc ""

    options.parse argv
  end

  def create_directory(mod)
    subdir = @map.dir_name mod, config[:base]

    if File.exist? subdir
      unless File.directory? subdir
        puts "#{subdir} already exists and is not a directory."
        return nil
      end
    else
      FileUtils.mkdir_p subdir
    end

    subdir
  end

  def write_requires(dir, file)
    /\A#{Regexp.escape config[:base]}\/?(.*)/ =~ dir
    parents = '../' * ($1.split('/').length + 1)

    File.open file, 'w' do |f|
      f.puts "require File.dirname(__FILE__) + '/#{parents}spec_helper'"
      config[:requires].each do |lib|
        f.puts "require '#{lib}'"
      end
    end
  end

  def write_spec(file, meth, exists)
    if exists
      out = `#{ruby} #{MSPEC_HOME}/bin/mspec-run --dry-run -fs -e '#{meth}' #{file}`
      return if out =~ /#{Regexp.escape meth}/
    end

    File.open file, 'a' do |f|
      f.puts <<-EOS

describe "#{meth}" do
  it "needs to be reviewed for spec completeness"
end
EOS
    end

    puts file
  end

  def create_file(dir, mod, meth, name)
    file = File.join dir, @map.file_name(meth, mod)
    exists = File.exist? file

    write_requires dir, file unless exists
    write_spec file, name, exists
  end

  def run
    config[:requires].each { |lib| require lib }
    constants = config[:constants]
    constants = @map.filter(Object.constants) if constants.empty?

    @map.map({}, constants).each do |mod, methods|
      name = mod.chop
      next unless dir = create_directory(name)

      methods.each { |method| create_file dir, name, method, mod + method }
    end
  end
  
  ##
  # Determine and return the path of the ruby executable.

  def ruby
    ruby = File.join(Config::CONFIG['bindir'],
                     Config::CONFIG['ruby_install_name'])

    ruby.gsub! File::SEPARATOR, File::ALT_SEPARATOR if File::ALT_SEPARATOR

    return ruby
  end

  def self.main
    ENV['MSPEC_RUNNER'] = '1'

    script = new
    script.options
    script.run
  end
end

