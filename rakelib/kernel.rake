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

def compile_ruby(src, rbc)
  dir = File.dirname rbc
  FileUtils.mkdir_p dir unless File.directory? dir

  ruby "lib/compiler/mri_compile.rb -frbx-kernel #{src} #{rbc}",
       :verbose => $verbose
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
  files.each do |file|
    compiled = "runtime/#{name}/#{File.basename(file)}c"
    all_kernel << compiled

    file compiled => file
  end
end

all_kernel << 'runtime/loader.rbc'

file 'runtime/loader.rbc' => 'kernel/loader.rb'

rule ".rbc" do |t|
  rbc = t.name
  src = t.prerequisites.first

  compile_ruby src, rbc
end

compiler = []

Dir["lib/compiler/*.rb"].each do |rb|
  compiler << "#{rb}c"
  file "#{rb}c" => rb do
    compile_ruby rb, "#{rb}c"
  end
end

namespace :kernel do

  task :show do
    p modules
    p loose
  end

  task :build => all_kernel + compiler + %w[runtime/platform.conf] do
    modules.each do |name, files|
      create_load_order files, "runtime/#{name}/.load_order.txt"
    end
  end

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

