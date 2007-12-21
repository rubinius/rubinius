#
# ci - continuous integration tool
#

require 'optparse'

module CI
  VERSION = '0.5.0'
end

action = :run
patterns = []
target = 'shotgun/rubinius'
format = 'CIFormatter'
clean = false
verbose = false
status = false
ci_files = "tmp/files.txt"
flags = []

opts = OptionParser.new("", 24, '   ') do |opts|
  opts.banner = "ci [options] (FILE|DIRECTORY|GLOB)+"
  opts.separator ""

  opts.on("-c", "--create", "Create the exclude file for failing specs") do
    action = :create
  end
  opts.on("-r", "--run", "Run the specs excluding the expected failures") do
    action = :run
  end
  opts.on("-i", "--invert", "Run the specs using only the expected failures") do
    action = :invert
    format = 'DottedFormatter'
  end
  opts.on("-t", "--target TARGET", String, 
          "Implementation that will run the specs: r:ruby|r19:ruby19|x:rbx|j:jruby") do |t|
    case t
    when 'r', 'ruby'
      target = 'ruby'
    when 'r19', 'ruby19'
      target = 'ruby19'
    when 'x', 'rbx', 'rubinius'
      target = 'shotgun/rubinius'
    when 'j', 'jruby'
      target = 'jruby'
    else
      target = t
    end
  end
  opts.on("-f", "--format FORMAT", String, 
          "Formatter for reporting: s:specdox|d:dotted|c:CI|h:html|i:immediate") do |f|
    case f
    when 's', 'specdox', 'specdoc'
      format = 'SpecdocFormatter'
    when 'h', 'html'
      format = 'HtmlFormatter'
    when 'd', 'dot', 'dotted'
      format = 'DottedFormatter'
    when 'c', 'ci', 'integration'
      format = 'CIFormatter'
    when 'i', 'immediate'
      format = 'ImmediateFormatter'
    else
      puts "Unknown format: #{f}"
      puts opts
      exit
    end
  end
  opts.on("-T", "--targetopt OPT", String,
          "Pass OPT as a flag to the target implementation") do |t|
    flags <<  t
  end
  opts.on("-C", "--clean", "Remove all compiled spec files first") do
    clean = true
  end
  opts.on("-V", "--verbose", "Output each file processed when running") do
    verbose = true
  end
  opts.on("-s", "--status", "Output a dot for every file run") do
    status = true
  end
  opts.on("-g", "--gdb", "Run under gdb") do
    flags << '--gdb'
  end
  opts.on("-A", "--valgrind", "Run under valgrind") do
    flags << '--valgrind'
  end
  opts.on("-v", "--version", "Show version") do
    puts "Continuous Integration Tool #{CI::VERSION}"
    exit
  end
  opts.on("-h", "--help", "Show this message") do
    puts opts
    exit
  end

  patterns = opts.parse ARGV
end

# only these directories are included because other directories have specs
# that cause compilation to fail. ideally, spec/**/*_spec.rb would be run.
patterns = ['spec/core', 'spec/language', 'spec/parser'] if patterns.empty?
files = []
patterns.each do |item|
  stat = File.stat(File.expand_path(item))
  files << item if stat.file?
  files.concat(Dir[item+"/**/*_spec.rb"].sort) if stat.directory?
end

# remove all compiled spec files to catch compiler bugs
if clean
  files.each do |name|
    cname = "#{name}c"
    File.delete(cname) if File.exist?(cname)
  end
end

file_list = files.map { |i| i.inspect }.join(",\n")

file_list = "[\n#{file_list}\n]"

code = <<-EOC
ENV['MSPEC_RUNNER'] = '1'
require 'spec/spec_helper'

$VERBOSE=nil

def exclude_name(file)
  File.join(File.dirname(file), '.spec', 
    File.basename(file, '.*').sub(/_spec$/, '_excludes') + '.txt')
end

def create_exclude_file(file)
  File.open(exclude_name(file), "w")
end

def mk_exclude_dir(file)
  dir = File.join(File.dirname(file), '.spec')
  Dir.mkdir(dir) unless File.exist?(dir)
end

def read_excludes(file)
  if File.exist?(file)
    File.open(file) do |f|
      f.readlines.map { |l| Regexp.new(Regexp.escape(l.chomp)) }
    end
  else
    []
  end
end

all_excludes = read_excludes("spec/excludes.txt")

#{files.inspect}.each do |file|
  mk_exclude_dir(file)
  excludes = read_excludes(exclude_name(file))
  formatter = #{format}.new(%s)
  spec_runner.formatter = formatter
  spec_runner.%s(*all_excludes)
  %s
  STDERR.puts file if #{verbose}
  STDERR.print "," if #{status}
  load file
  formatter.summary
  spec_runner.clear_filters
  %s
end
STDERR.puts "" if #{status}
EOC

case action
when :create
  code = code % ['create_exclude_file(file)', 'except', '', 'formatter.out.close']
when :run
  code = code % ['STDOUT', 'except', 'spec_runner.except(*excludes)', '']
when :invert
  code = code % ['STDOUT', 'only', 'spec_runner.only(*excludes)', '']
else
  puts "Unknown action: #{action}"
  puts opts
  exit
end

Dir.mkdir "tmp" unless File.directory?("tmp")
File.open("tmp/last_ci.rb", "w") do |f|
  f << code
end

exec("#{target} #{flags.join(' ')} -Ispec tmp/last_ci.rb")
