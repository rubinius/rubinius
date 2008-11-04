#!/usr/bin/env ruby
#
# A tiny spec-style runner for working on a parser.

require 'pp'

begin
  require 'sexp'
rescue LoadError
  require 'rubygems'
  require 'sexp'
end

def sydney_parser(hash)
  hash['SydneyParser'] || hash['RubyParser']
end

def ruby_parser(hash)
  hash['RubyParser']
end

def raw_parse_tree(hash)
  hash['RawParseTree']
end

def to_sexp_sydney_raw(source)
  sexp = source.parse '(string)', 1
  Sexp.from_array sexp
end

def to_sexp_sydney_unified(source)
  source.to_sexp_sydney_parser '(string)'
end

def to_sexp_ruby_parser(source)
  RubyParser.new.process source, '(string)'
end

def to_sexp_x(cmd, str, source)
  name = "__parser_spec_source__.rb"
  File.open(name, "w") do |f|
    f.puts str % source
  end
  eval `cat #{name} | #{cmd}`
ensure
  File.delete name rescue nil
end

# Runs the example after fetching the sexp from +sexp+
# method. Compares the sexp to +standard+ from +hash+.
def run(node, hash, standard, sexp, cmd, str)
  src = hash['Ruby']
  expected = send(standard, hash).to_a

  if cmd
    actual = send sexp, cmd, str, src
  else
    actual = send sexp, src
  end
  actual = actual.to_a unless actual.instance_of? Array

  if actual != expected
    return false, "#{node}\n\n#{src}\n\nexpected\n#{actual.pretty_inspect}\n" \
                  "to equal\n#{expected.pretty_inspect}\n\n"
  else
    return true, nil
  end
end

# Shows the sexp generate by the +sexp+ method from
# the +source+ string.
def show(source, sexp, cmd, str)
  if cmd
    result = send sexp, cmd, str, source
  else
    result = send sexp, source
  end
  result = result.to_a if sexp == :to_sexp_sydney_raw
  pp result
end

def usage
  puts "usage: run [options] (FILE|DIRECTORY|GLOB)+"
  puts ""
  puts "-R      Test RubyParser"
  puts "-s      Test raw SydneyParser"
  puts "-S      Test rewritten SydneyParser"
  puts "-x CMD  Test sexp from invoking CMD (see -t) against raw ParseTree"
  puts "-X CMD  Test sexp from invoking CMD (see -t) against rewritten SydneyParser"
  puts "-t STR  Substitute into template STR at %s"
  puts "-d      Display the sexp converted from reading STDIN"
  puts "-V      Display more info while running"
  puts "-h      Show this message"
  puts ""
  exit 1
end

# defaults
processor = :run
standard = :sydney_parser
sexp = :to_sexp_sydney_unified
template = %Q{%s}
command = nil
verbose = false

files = []
while x = ARGV.shift
  if File.file? x
    files << x
  elsif File.directory? x
    files.concat Dir["#{x}/**/*_spec.rb"]
  else
    case x
    when "-s"
      standard = :raw_parse_tree
      sexp = :to_sexp_sydney_raw
    when "-S"
      standard = :sydney_parser
      sexp = :to_sexp_sydney_unified
    when "-R"
      begin
        require 'ruby_parser'
      rescue LoadError
        puts "Unable to load RubyParser.\n" \
             "Consider running the script with ruby -I<whatever> -rruby_parser"
        exit 1
      end
      sexp = :to_sexp_ruby_parser
      standard = :ruby_parser
    when "-x"
      command = ARGV.shift
      standard = :raw_parse_tree
      sexp = :to_sexp_x
    when "-X"
      command = ARGV.shift
      standard = :sydney_parser
      sexp = :to_sexp_x
    when "-t"
      template = ARGV.shift
    when "-d"
      processor = :show
    when "-V"
      verbose = true
    when "-h"
      usage
    else
      puts "Unknown option: #{x}"
      usage
    end
  end
end

if processor == :show
  str = STDIN.read
  show(str, sexp, command, template) unless str.empty?
else
  if files.empty?
    puts "No files given"
    usage
  end

  report = []
  total = failures = errors = 0

  start = Time.now
  files.each do |name|
    total += 1
    print "\n#{name.ljust(30)} " if verbose
    load name
    begin
      node = File.basename(name, "_spec.rb").split("/").last
      status, output = run(node, test_case, standard, sexp, command, template)
      if status
        print "."
      else
        print "F"
        failures += 1
        report << output
      end
    rescue Object => e
      print "E"
      errors += 1
      report << "#{name}\n#{e.inspect}\n#{e.backtrace.join("\n")}"
    end
  end
  finish = Time.now

  unless report.empty?
    puts "\n"
    report.each_with_index do |e, i|
      puts "\n#{i+1})\n"
      puts e
    end
  end

  puts "\nFinished in #{finish-start} seconds"
  puts "\n#{total} examples, #{failures} failures, #{errors} errors"
end
