# NOTE! When updating this file, also update INSTALL, if necessary.

$compiler = nil
$verbose = Rake.application.options.trace
$dlext = Config::CONFIG["DLEXT"]

require 'tsort'

task :default => :build

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
  dir = File.dirname(output)

  unless File.exists?(dir)
    FileUtils.mkdir_p dir
  end
  
  compiler = $compiler ? " -I#{$compiler}" : ''

  sh "shotgun/rubinius#{compiler} compile #{name} #{output}",
     :verbose => $verbose
end

task :compiler do
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

  $compiler = ENV['COMPILER']
end

rule ".rbc" => %w[compiler .rb] do |t|
  compile t.source, t.name
end

#file 'runtime/core/kernel/core/proc.rbc' => 'kernel/core/proc.rb' do |t|
#  p t.prerequisites
#  p t.name
#end

class CodeGroup

  def initialize(files, compile_dir, rba_name, load_order=true)
    @files = FileList[files]
    @output = nil
    @compile_dir = compile_dir
    @build_dir = File.join 'runtime', rba_name
    @rba_name = "#{rba_name}.rba"

    if load_order
      @load_order = File.join @compile_dir, '.load_order.txt'
    else
      @load_order = nil
    end

    @output = []

    make_tasks
  end

  attr_reader :output

  def clean
    sh "find #{@compile_dir} -name '*.rbc' -delete"
  end

  def compile_task
    @files.each do |source|
      runtime = File.join(@compile_dir, source.ext("rbc"))

      @output << runtime

      deps = [source, @load_order].compact

      file runtime => deps do |t|
        compile t.prerequisites.first, t.name
      end
    end
  end

  def load_order_task
    return unless @load_order

    file @load_order do
      create_load_order(@files, @load_order)
    end

    @output << @load_order
  end

  def make_tasks
    Dir.mkdir @compile_dir unless File.exists? @compile_dir

    compile_task
    load_order_task
    rba_task

    @output
  end

  def rba_task
    file File.join('runtime', 'stable', @rba_name) => @output do
      files = @output.map do |path|
        path.sub File.join(@build_dir, ''), ''
      end

      Dir.chdir @build_dir do
        zip_name = File.join '..', 'stable', @rba_name
        rm_f zip_name, :verbose => $verbose
        sh "zip #{zip_name} #{files.join ' '}", :verbose => $verbose
      end
    end
  end

end

Core      = CodeGroup.new 'kernel/core/*.rb', 'runtime/core', 'core'
Bootstrap = CodeGroup.new 'kernel/bootstrap/*.rb', 'runtime/bootstrap',
                          'bootstrap'
Platform  = CodeGroup.new 'kernel/platform/*.rb', 'runtime/platform', 'platform'
Compiler  = CodeGroup.new 'compiler/**/*.rb', 'runtime', 'compiler', false

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
task :build => 'build:all'

def install_files(files, destination)
  files.sort.each do |path|
    next if File.directory? path

    file = path.sub %r%^(runtime|lib)/%, ''
    dest_file = File.join destination, file
    dest_dir = File.dirname dest_file
    mkdir_p dest_dir unless File.directory? dest_dir

    install path, dest_file, :mode => 0644, :verbose => true
  end
end

desc "Install rubinius as rbx"
task :install => :config_env do
  sh "cd shotgun; make install"

  mkdir_p ENV['RBAPATH'], :verbose => true
  mkdir_p ENV['CODEPATH'], :verbose => true

  rba_files = Rake::FileList.new('runtime/**/*.rb{a,c}',
                                 'runtime/**/.load_order.txt')

  install_files rba_files, ENV['RBAPATH']

  lib_files = Rake::FileList.new 'lib/**/*'

  install_files lib_files, ENV['CODEPATH']

  mkdir_p File.join(ENV['CODEPATH'], 'bin'), :verbose => true

  Rake::FileList.new("#{ENV['CODEPATH']}/**/*.rb").sort.each do |rb_file|
    sh File.join(ENV['BINPATH'], 'rbx'), 'compile', rb_file, :verbose => true
  end
end

task :config_env do
  File.foreach 'shotgun/config.mk' do |line|
    next unless line =~ /(.*?)=(.*)/
    ENV[$1] = $2
  end
end

desc "Recompile all ruby system files"
task :rebuild => %w[clean:rbc clean:shotgun build:all]

task :clean => %w[clean:rbc clean:shotgun]

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
      rm_f f, :verbose => $verbose
    end
  end

  desc "Cleans up VM building site"
  task :shotgun do
    sh "make clean"
  end  
end

namespace :build do

  task :all => %w[
    build:shotgun
    build:rbc
    build:extensions
  ]

  # This nobody rule lets use use all the shotgun files as 
  # prereqs. This rule is run for all those prereqs and just 
  # (obviously) does nothing, but it makes rake happy.
  rule '^shotgun/.+'

  c_source = FileList[
    "shotgun/config.h",
    "shotgun/lib/*.[ch]",
    "shotgun/lib/*.rb",
    "shotgun/lib/subtend/*.[chS]",
    "shotgun/main.c",
  ]

  file "shotgun/rubinius.bin" => c_source do
    sh "make vm"
  end

  file 'shotgun/mkconfig.sh' => 'configure'
  file 'shotgun/config.mk' => %w[shotgun/config.h shotgun/mkconfig.sh shotgun/vars.mk]
  file 'shotgun/config.h' => %w[shotgun/mkconfig.sh shotgun/vars.mk] do
    sh "./configure"
    raise 'Failed to configure Rubinius' unless $?.success?
  end

  desc "Compiles shotgun (the C-code VM)"
  task :shotgun => %w[configure shotgun/rubinius.bin]

  task :setup_rbc => :compiler

  task :rbc => ([:setup_rbc] + AllPreCompiled)

  # EXTENSIONS
  task :extensions => %w[build:shotgun build:rbc build:syck]

  task :syck => "lib/ext/syck/rbxext.#{$dlext}"

  file "lib/ext/syck/rbxext.#{$dlext}" => FileList[
    'lib/ext/syck/build.rb',
    'lib/ext/syck/*.c',
    'lib/ext/syck/*.h',
  ] do
    sh "./shotgun/rubinius compile lib/ext/syck"
  end

  # OBSOLETE
  task :core => :rbc do
    raise "OBSOLETE. Use 'rake build'"
  end

  task :bootstrap => :rbc do
    raise "OBSOLETE. Use 'rake build'"
  end

  task :platform => :rbc do
    raise "OBSOLETE. Use 'rake build'"
  end

  task :loader => :rbc do
    raise "OBSOLETE. Use 'rake build'"
  end
end

desc "Build task for CruiseControl"
task :ccrb => [:build, 'spec:ci']
