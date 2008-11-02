# All the tasks to manage the kernel

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

require 'tsort'

class Hash
  include TSort

  # This keeps things consistent across all platforms
  def tsort_each_node(&block)
    keys.sort.each(&block)
  end

  def tsort_each_child(node, &block)
    fetch(node).each(&block)
  end
end

def create_load_order(files, output=".load_order.txt")
  d = Hash.new { |h,k| h[k] = [] }

  # assume all the files are in the same directory
  dir = File.dirname(files.first)
  found = false
  files.each do |fname|
    name = File.basename(fname)
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

  puts "Generating #{output}..." if $verbose

  File.open(output, "w") do |f|
    begin
      if found
        list = d.tsort
      else
        list = files.sort
      end

      list.each do |n|
        f.puts File.basename(n, ".*") + ".rbc"
      end
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

require 'lib/compiler/mri_compile'

def compile_ruby(src, rbc, check_mtime = false, kernel = false)
  if check_mtime and File.readable?(rbc)
    return if File.mtime(rbc) >= File.mtime(src)
  end

  dir = File.dirname rbc
  FileUtils.mkdir_p dir unless File.directory? dir

  flags = kernel ? ["rbx-safe-math", "rbx-kernel"] : []
  mri_compile src, rbc, false, flags
end

loose =  []
modules = Hash.new { |h,k| h[k] = [] }

Dir["kernel/**/*.rb"].each do |path|
  _, mod, file = path.split("/")

  if file
    modules[mod] << path
  else
    loose << mod
  end
end

all_kernel = []
all_kernel << 'kernel/bootstrap/rubinius_config.rb'
all_kernel << 'kernel/bootstrap/ruby_config.rb'

modules['bootstrap'] << 'kernel/bootstrap/rubinius_config.rb'
modules['bootstrap'] << 'kernel/bootstrap/ruby_config.rb'
modules.each do |name, files|
  files.each do |rb|
    rbc = "runtime/#{name}/#{File.basename(rb)}c"
    all_kernel << rbc

    file rbc => rb do
      compile_ruby rb, rbc, false, true
    end
  end
end

all_kernel << 'runtime/loader.rbc'

file 'runtime/loader.rbc' => 'kernel/loader.rb'

rule ".rbc" do |t|
  rbc = t.name
  src = t.prerequisites.first

  compile_ruby src, rbc
end

COMPILER_SOURCES = Dir["lib/compiler/*.rb"] +
  %w(strscan stringio racc/parser ruby_lexer ruby_parser
     ruby_parser_extras sexp sexp_processor).map { |f| "lib/#{f}.rb" }
COMPILER_MTIME = COMPILER_SOURCES.map { |f| File::Stat.new(f).mtime }.max
compiler = []

COMPILER_SOURCES.each do |rb|
  compiler << "#{rb}c"
  file "#{rb}c" => rb do
    compile_ruby rb, "#{rb}c", false, true
  end
end

task :kernel => 'kernel:build'

namespace :kernel do
  task :check_compiler do
    kernel_mtime = all_kernel.select do |f|
      f =~ /rbc$/ and File.exist? f
    end.map do |f|
      File::Stat.new(f).mtime
    end.min

    Rake::Task['kernel:clean'].invoke if !kernel_mtime or COMPILER_MTIME > kernel_mtime unless ENV['NOCLEAN']
  end

  task :show do
    p modules
    p loose
  end

  task :build => ['kernel:check_compiler', all_kernel, compiler,
                  'runtime/platform.conf'].flatten do
    modules.each do |name, files|
      create_load_order files, "runtime/#{name}/.load_order.txt"
    end
  end

  desc "clean up rbc files"
  task :clean do
    rm_rf %w[runtime/bootstrap runtime/platform runtime/common runtime/delta],
          :verbose => $verbose

    files_to_delete = []
    files_to_delete += Dir["*.rbc"] + Dir["**/*.rbc"] + Dir["**/.*.rbc"]
    files_to_delete += Dir["**/.load_order.txt"]
    files_to_delete += ["runtime/platform.conf"]

    rm_f files_to_delete, :verbose => $verbose
  end

  desc "Create dot(1) files of dependencies in bootstrap, common, delta"
  task :graph do
    begin
      $: << File.expand_path("~/Work/p4/zss/src/ZenHacks/dev/lib/")
      require 'graph'
    rescue LoadError
      abort 'install ZenHacks'
    end

    %w[bootstrap common delta].each do |path|
      graph = Graph.new
      graph.prefix << 'rankdir="LR"'

      Dir[File.join('kernel', path, '*.rb')].each do |file|
        depends = nil
        open file do |io| depends = io.gets end

        if depends =~ /^# depends on: (.*)/ then
          $1.split.each do |depend|
            graph[file] << File.join('kernel', path, depend)
          end
        else
          graph[file] << 'Nothing'
        end

        open "#{path}_depends.dot", 'w' do |io|
          io << graph
        end
      end
    end
  end

end

