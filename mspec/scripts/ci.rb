#
# ci - continuous integration tool
#

require 'optparse'

module CI
  VERSION = '0.5.0'
end

action = :run
patterns = []
includes = ['-Ispec']
requires = []
target = 'shotgun/rubinius'
name = nil
format = 'DottedFormatter'
clean = false
verbose = false
marker = nil
flags = []
# The exclude directory defaults to '.spec' in the directories containing
# the spec files. Override by setting CI_EXCLUDES_DIR or with the 
# --excludes-dir command line switch. If the value is an absolute path
# (e.g. starts with '/'), write the exclude directories relative to that
# path. Otherwise, create the exclude directory relative to the directories
# containing the spec files.
excludes_dir = Object.const_defined?(:CI_EXCLUDES_DIR) ? Object.const_get(:CI_EXCLUDES_DIR) : '.spec'

opts = OptionParser.new("", 24, '   ') do |opts|
  opts.banner = "ci [options] (FILE|DIRECTORY|GLOB)+"
  opts.separator ""

  opts.on("-c", "--create", "Create the exclude file for failing specs") do
    action = :create
    format = 'CIFormatter'
  end
  opts.on("-R", "--run", "Run the specs excluding the expected failures") do
    action = :run
  end
  opts.on("-i", "--invert", "Run the specs using only the expected failures") do
    action = :invert
  end
  opts.on("-t", "--target TARGET", String, 
          "Use TARGET to run the specs: r:ruby|r19:ruby19|x:rbx|j:jruby") do |t|
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
          "Use FORMAT for reporting: s:specdox|d:dotted|c:CI|h:html|i:immediate") do |f|
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
    when 'q', 'quiet'
      format = 'QuietFormatter'
    else
      puts "Unknown format: #{f}"
      puts opts
      exit
    end
  end
  opts.on("-E", "--excludes-dir DIR", String,
          "Use DIR for the files containing spec descriptions to exclude" ) do |d|
    excludes_dir = d
  end
  opts.on("-I", "--include DIR", String,
          "Pass DIR through as the -I option to the target") do |d|
    includes << "-I#{d}"
  end
  opts.on("-r", "--require LIBRARY", String,
          "Pass LIBRARY through as the -r option to the target") do |f|
    requires << "-r#{f}"
  end
  opts.on("-n", "--name RUBY_NAME", String,
          "Override the name used to determine the implementation") do |n|
    name = "RUBY_NAME = \"#{n}\";"
  end
  opts.on("-T", "--targetopt OPT", String,
          "Pass OPT as a flag to the target implementation") do |t|
    flags <<  t
  end
  opts.on("-C", "--clean", "Remove all compiled spec files first") do
    clean = true
  end
  opts.on("-V", "--verbose", "Output the name of each file processed") do
    verbose = true
  end
  opts.on("-m", "--marker MARKER", String,
          "Output MARKER for each file processed. Overrides -V") do |m|
    marker = m
    verbose = true
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
patterns = CI_FILE_SPECS if patterns.empty?
files = []
patterns.each do |item|
  stat = File.stat(File.expand_path(item))
  files << item if stat.file?
  files.concat(Dir[item+"/**/*_spec.rb"].sort) if stat.directory?
end
max_name_width = files.inject(0) { |max, f| f.size > max ? f.size : max }

# remove all compiled spec files to catch compiler bugs
if clean
  files.each do |name|
    cname = "#{name}c"
    File.delete(cname) if File.exist?(cname)
  end
end

code = <<-EOC
ENV['MSPEC_RUNNER'] = '1'
#{name}
require 'fileutils'
require 'spec/spec_helper'

$VERBOSE=nil

def exclude_dir(file, dir=#{excludes_dir.inspect})
  if dir[0] == ?/
    m = file.match %%r[.*spec/(.*)/.*_spec.rb]
    m ? File.join(dir, m[1]) : dir
  else
    File.join(File.dirname(file), dir)
  end
end

def exclude_name(file)
  File.join(exclude_dir(file), File.basename(file, '.*').sub(/_spec$/, '_excludes') + '.txt')
end

def create_exclude_file(file)
  File.open(exclude_name(file), "w")
end

def mk_exclude_dir(file)
  dir = exclude_dir(file)
  FileUtils.mkdir_p(dir) unless File.exist?(dir)
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

all_excludes = read_excludes("spec/data/critical.txt")

set_spec_runner(#{format})
spec_runner.formatter.print_start
#{files.inspect}.each do |file|
  mk_exclude_dir(file)
  spec_runner.formatter.out = %s
  excludes = read_excludes(exclude_name(file))
  spec_runner.%s(*all_excludes)
  %s
  begin
    STDERR.print(#{marker.inspect} || "\\n\#{file.ljust(#{max_name_width})}") if #{verbose}
    load file
  rescue Exception => e
    puts "\#{e} loading \#{file}"
  ensure
    spec_runner.clear_filters
  end
  %s
end
# reset formatter.out to stdout
spec_runner.formatter.out = $stdout
spec_runner.formatter.summary
EOC

case action
when :create
  code = code % ['create_exclude_file(file)', 'except', '', 'spec_runner.formatter.out.close']
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

exec("#{target} #{flags.join(' ')} #{includes.join(" ")} #{requires.join(" ")} tmp/last_ci.rb")
