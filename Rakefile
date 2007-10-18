# NOTE! When updating this file, also update INSTALL, if necessary.

require 'tsort'

task :default => :tasks

task :tasks do
  puts `rake -T`
end

class Hash
  include TSort
  
  alias tsort_each_node each_key
  
  def tsort_each_child(node, &block)
    fetch(node).each(&block)
  end
end
  
def newer?(file, cmp)
  File.exists?(cmp) and File.mtime(cmp) >= File.mtime(file)
end

@setup_stable = false

def setup_stable
  return if @setup_stable
  @setup_stable = true
  
  @pb = "runtime/stable/bootstrap.rba"
  @pp = "runtime/stable/platform.rba"
  @pc = "runtime/stable/core.rba"
  @pl = "runtime/stable/loader.rbc"
  @pr = "runtime/stable/compiler.rba"

  if ENV['USE_CURRENT']
    puts "Use current versions, not stable."
  else
    ENV['BOOTSTRAP'] = @pb
    ENV['CORE'] = @pc
    ENV['LOADER'] = @pl
    ENV['PLATFORM'] = @pp
    ENV['COMPILER'] = @pr
  end

  @compiler = ENV['COMPILER']
end

def source_name(compiled)
  File.basename(compiled, '.*') + '.rb'
end

def compiled_name(source, dir)
  File.join(dir, File.basename(source, '.*') + '.rbc')
end

# Some files have load order dependencies. To specify a load order
# dependency, include a comment in the file that has the dependency.
# For example, assume files a.rb and b.rb, where a.rb requires that
# b.rb is loaded first. In a.rb, include a comment
#   # depends on: b.rb
#
# The 'depends on:' declaration takes a space separated list of file.
# When the '.load_order.txt' file is created, a topological sort
# (see name caveat in TSort) of the dependencies is performed
# so files that are depended on are loaded first.
#
# If there is a 'depends on:' declarations for a non-existent file,
# or if there are cyclic dependencies, this method will not create
# the '.load_order.txt' file.

def create_load_order(files, output=".load_order.txt")
  d = Hash.new { |h,k| h[k] = [] }
  
  # assume all the files are in the same directory
  dir = File.dirname(files.first)
  found = false
  files.each do |fname|
    name = source_name(fname)
    # Force every entry to be in the hash
    d[name]
    File.open(File.join(dir, name), "r") do |f|
      f.each do |line|
        if m = /#\s*depends on:\s*(.*)/.match(line)
          found = true
          m[1].split.each { |dep| d[name] << dep }
        end
      end
    end
  end
  
  File.open(output, "w") do |f|
    begin
      if found
        list = d.tsort
      else
        list = files.sort
      end
      
      f.puts list.collect { |n| compiled_name(n, dir) }.join("\n")
    rescue IndexError => e
      puts "Unable to generate '.load_order.txt'"
      puts "Most likely, a file includes a 'depends on:' declaration for a non-existent file"
      raise e
    rescue TSort::Cyclic => e
      puts "Unable to generate '.load_order.txt' due to a cyclic dependency\n  (#{e.message})"
      raise e
    end
  end
end

def compile(name, output)
  setup_stable
  
  dir = File.dirname(output)
  unless File.exists?(dir)
    FileUtils.mkdir_p dir
  end
  
  if @compiler
    sh "shotgun/rubinius -I#{@compiler} compile #{name} #{output}", :verbose => false
  else
    sh "shotgun/rubinius compile #{name} #{output}", :verbose => false
  end
end

rule ".rbc" => ".rb" do |t|
  compile(t.source, t.name)
end

#file 'runtime/core/kernel/core/proc.rbc' => 'kernel/core/proc.rb' do |t|
#  p t.prerequisites
#  p t.name
#end

class CodeGroup
  def initialize(files, dir, load_order=true)
    @files = FileList[files]
    @output = nil
    @directory = dir
    map(dir, load_order)
  end
  
  attr_reader :output
  
  def map(dir, load_order)
    
    unless File.exists?(dir)
      Dir.mkdir dir
    end
    
    prc = proc do |t|
      compile(t.prerequisites.first, t.name)
    end

    @output = []

    @files.each do |source|
      runtime = File.join(dir, source.ext("rbc"))
      @output << runtime
      file(runtime => source, &prc)
    end
    
    if load_order
    
      lo = File.join(dir, '.load_order.txt')
    
      file lo => @files do
        create_load_order(@files, lo)
      end
      
      @output << lo
    end

    return @output
  end
  
  def clean
    sh "find #{@directory} -name '*.rbc' -delete"
  end
end

Core = CodeGroup.new 'kernel/core/*.rb', 'runtime/core'
Bootstrap = CodeGroup.new 'kernel/bootstrap/*.rb', 'runtime/bootstrap'
Platform = CodeGroup.new 'kernel/platform/*.rb', 'runtime/platform'
Compiler = CodeGroup.new 'compiler/**/*.rb', 'runtime', false

file 'runtime/loader.rbc' => 'kernel/loader.rb' do
  compile 'kernel/loader.rb', 'runtime/loader.rbc'
end

AllPreCompiled = Core.output + Bootstrap.output + Platform.output + Compiler.output
AllPreCompiled << "runtime/loader.rbc"

# spec tasks
desc "Run all 'known good' specs (task alias for spec:ci)"
task :spec => 'spec:ci'

namespace :spec do
  namespace :setup do
    # Setup for 'Subtend' specs. No need to call this yourself.
    task :subtend do
      Dir["spec/subtend/**/Rakefile"].each do |rakefile|
        sh "rake -f #{rakefile}"
      end
    end
  end

  desc "Run continuous integration examples"
  task :ci do
    target = ENV['SPEC_TARGET'] || 'rbx'
    system %(shotgun/rubinius -e 'puts "rbx build: \#{Rubinius::BUILDREV}"') if target == 'rbx'
    sh "bin/ci -t #{target}"
  end

  spec_targets = %w(compiler core language library parser rubinius)
  # Build a spec:<task_name> for each group of Rubinius specs
  spec_targets.each do |group|
    desc "Run #{group} examples"
    task group do
      sh "bin/mspec spec/#{group}"
    end
  end

  desc "Run subtend (Rubinius C API) examples"
  task :subtend => "spec:setup:subtend" do
    sh "bin/mspec spec/rubinius/subtend"
  end

  # Specdiffs to make it easier to see what your changes have affected :)
  desc 'Run specs and produce a diff against current base'
  task :diff => 'diff:run'

  namespace :diff do
    desc 'Run specs and produce a diff against current base'
    task :run do
      system 'bin/mspec -f ci -o spec/reports/specdiff.txt spec' 
      system 'diff -u spec/reports/base.txt spec/reports/specdiff.txt'
      system 'rm spec/reports/specdiff.txt'
    end

    desc 'Replace the base spec file with a new one'
    task :replace do
      system 'bin/mspec -f ci -o spec/reports/base.txt spec' 
    end
  end

  task :r2r do
    puts ARGV.inspect
  end
end

desc "Build everything that needs to be built"
task :build => ['build:all']

desc "Recompile all ruby system files"
task :rebuild => ['clean:rbc', 'clean:shotgun', 'build:all']

desc "Remove all ruby system files"
task :distclean => 'clean:rbc'

desc "Remove all stray compiled Ruby files"
task :pristine do
  FileList['**/*.rbc'].each do |fn|
    next if /^runtime/.match(fn)
    FileUtils.rm fn rescue nil
  end
end

namespace :clean do
  
  desc "Remove all compile system ruby files (runtime/)"
  task :rbc do
    AllPreCompiled.each do |f|
      File.unlink f rescue nil
    end
  end

  desc "Cleans up VM building site"
  task :shotgun do
    sh "make clean"
  end  
end

namespace :build do
  
  task :all => ["build:shotgun", "build:rbc"]
  
  # This nobody rule lets use use all the shotgun files as 
  # prereqs. This rule is run for all those prereqs and just 
  # (obviously) does nothing, but it makes rake happy.
  rule '^shotgun/.+' do
  end
  
  c_source = FileList["shotgun/lib/*.[ch]", "shotgun/main.c",
            "shotgun/lib/*.rb", "shotgun/lib/subtend/*.[chS]"]
  
  file "shotgun/rubinius.bin" => c_source do
    sh "make vm"
  end
  
  task :extensions => ["build:shotgun", "build:rbc"] do
    sh "./shotgun/rubinius compile lib/ext/syck"
  end
    
  file "shotgun/config.h" do
    sh "./configure"
    raise 'Failed to configure Rubinius' unless $?.success?
  end

  task :configure => ["shotgun/config.h"]

  desc "Compiles shotgun (the C-code VM)"
  task :shotgun => [:configure, "shotgun/rubinius.bin"]
  
  task :setup_rbc do
    setup_stable
  end
  
  task :rbc => ([:setup_rbc] + AllPreCompiled)
  
  task :core => :rbc do
    puts "OBSOLETE. Use 'rake build'"
  end
  
  task :bootstrap => :rbc do
    puts "OBSOLETE. Use 'rake build'"
  end
  
  task :platform => :rbc do
    puts "OBSOLETE. Use 'rake build'"
  end
  
  task :loader => :rbc do
    puts "OBSOLETE. Use 'rake build'"
  end
end

# dev tasks
namespace :dev do
  desc "Make a snapshot of the runtime files for your own safety"
  task :setup do
    puts "OBSOLETE. You no longer need to do this."
  end
end

