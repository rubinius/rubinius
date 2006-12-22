#!/usr/bin/env ruby

# Copyright (c) 2006 Michael Fellinger m.fellinger@gmail.com
# All files in this distribution are subject to the terms of the Ruby license

require 'digest/sha1'
require 'optparse'
require 'readline'
require 'systemu'

class String
  begin
    require 'term/ansicolor'
    include Term::ANSIColor
  rescue LoadError
    def green() self end
    def red() self end
  end
end

Thread.abort_on_exception = true

class Feature
  attr_accessor :filename, :checksum
  def initialize filename
    @checksum = nil
    @filename = filename
    unchange
  end

  def gen_checksum
    Digest::SHA1.hexdigest(File.read(@filename)) rescue nil
  end

  def changed?
    gen_checksum != @checksum
  end

  def changed
    @checksum = nil
  end

  def unchange
    @checksum = gen_checksum
  end
end

class Testcase < Feature
  attr_accessor :specifications, :failures, :features, :mode
  attr_reader :stdout, :stderr, :status, :broken

  def initialize filename, options = {}
    @output = ''
    @failures = nil
    @broken = false
    @specifications = 0
    @features = []
    width = `stty size`.split.last.to_i

    @mode     = options[:mode]    || :rspec
    @execute  = options[:execute] || %( ruby %filename )
    @width    = options[:width]   || (width == 0 ? 60 : width)
    super(filename)

    changed
    run
  end

  def run
    Thread.new do
      loop do
        if changed?
          @failures = nil
          sleep 0.1 while Thread.main[:locked]
          Thread.main[:locked] = true
          test
          Thread.main[:locked] = false
        end
        sleep 1
      end
    end
  rescue Object => ex
    puts ex
    retry
  end

  def test
    until passed?
      setup
      execute
      unchange
      if failed?
        error_report
        sleep 0.1 until changed?
      else
        success_report
      end
    end
  end

  def execute
    p transform_cmd
    p @execute
    @status, @stdout, @stderr = systemu transform_cmd
    @broken = @status.exitstatus == 0 ? false : true
    analyze unless @broken
  end

  def transform_cmd
    @execute.gsub(/(%\w+)/) do |m|
      instance_variable_get("@#{$1[1..-1]}")
    end
  end

  def error_report
    puts "#{@filename} failed.".center(@width)

    puts "From STDOUT:", @stdout unless @stdout.empty?
    puts "From STDERR:", @stderr unless @stderr.empty?
    stats = [@failures, @specifications].map{|n| (n || 0).to_s.rjust(2)}.join('/')
    box = "[failed #{stats} specs]"
    puts "#{@filename.ljust(@width - box.size)}#{box}".red
  end

  def success_report
    stats = @specifications.to_s.rjust(2)
    box = "[passed #{stats} specs]"
    puts "#{@filename.ljust(@width - box.size)}".green
  end

  def changed?
    gen_checksum != @checksum ||
      @features.any?{|f| f.changed?}
  end

  def unchange
    super
    @features.each do |f|
      f.unchange
    end
  end

  def failed?
    @broken or @failures.nil? or @failures > 0
  end

  def passed?
    @failures == 0
  end

  def to_s
    @output
  end

  def setup
    Dir['lib/**/*.{rb,so}'].each do |file|
      @features << Feature.new(file)
    end
    @features.uniq!
  end

  def analyze
    lines = @output.split("\n")

    @specifications = nil
    @failures = nil

    handlers = {
      # Test::Unit
      lambda{
      @specifications = $1.to_i
      @failures = $2.to_i + $3.to_i
    } => /(?:\d+) tests, (\d+) assertions, (\d+) failures, (\d+) errors/,

      # RSpec
      lambda{
      @specifications = $1.to_i
    @failures = $2.to_i
    } => /(\d+) specification[s]?, (\d+) failure[s]?/,
    }

    lines.each do |line|
      handlers.each do |meth, regex|
        if line =~ regex
          meth.call

          puts "Doesn't run at all!" unless @specifications && @failures
          return
        end
      end
    end
  end
end

class Runner
  attr_accessor :files

  def initialize files = ARGV, options = {}
    @files = files

    Thread.main[:locked] = false

    @tcs = @files.map do |file|
      tc = Testcase.new(file, options)
    end
    puts "checking #{@tcs.map{|tc| tc.filename}.join(', ')}"
    puts
    puts "-" * 50

    trapper
    run
  end

  def trapper
    trap('SIGINT') do
      console
      trapper
    end
  end

  def console
    help

    catch(:return) do
      trap('SIGINT') do
        puts
        exit
      end
      while line = Readline.readline('> ', true)
        case line.strip
        when 'resume' : throw(:resume)
        when 'exit' : exit
        else
          process(line)
        end
      end
    end
  end

  def process line
    p eval(line)
  rescue Object => ex
    puts ex
  end

  def restart
    @tcs.each do |tc|
      tc.changed
    end
  end

  def help
    puts %{
      This console lets you execute arbitary ruby, special commands are:
      #{'resume'.green}  - leave the console and resume testing again.
      #{'restart'.green} - leave the console and restart testing.
      #{'help'.green}    - get this help.
    }.strip.gsub(/^\s*(\w*)/, '\1')
  end

  def run
    loop do
      sleep 42
    end
  end
end


options = {}

OptionParser.new do |opts|
  opts.banner = "Usage: autotest [options]"

  opts.on("-u", "--[test-]unit", "Expect Test::Unit Testcases") do |u|
    options[:mode] = :test_unit
  end

  opts.on("-s", "--[r]spec", "Expect RSpec Testcases") do |s|
    options[:mode] = :rspec
  end

  opts.on("-e", "--execute COMMAND", "Pass a command to be executed for the testcases,
                                     %name will be interpolated to instance-variables, so
                                     one can use %filename for example") do |e|
    options[:execute] = e
  end

  opts.on("-w", "--width INTEGER", "set the width of the report") do |w|
    options[:w] = w.to_i
  end
end.parse!

Runner.new(ARGV, options)
