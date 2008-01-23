
def make(args = nil)
  if RUBY_PLATFORM =~ /bsd/
    gmake = 'gmake'
  else
    gmake = 'make'
  end
  "#{ENV['MAKE'] || gmake} #{args}"
end

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

  puts "Generating #{output}..."

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

def compile(name, output=nil, check_mtime=false)
  if output
    dir = File.dirname(output)

    unless File.exists?(dir)
      FileUtils.mkdir_p dir
    end

    if check_mtime and File.exists?(output) and File.mtime(output) > File.mtime(name)
      return
    end
  end
  
  inc = "-Iruntime/stable/compiler.rba -rcompiler/init"
  flags = "-frbx-safe-math -frbx-kernel"

  if ENV['GDB']
    sh "shotgun/rubinius --gdb #{inc} compile #{flags} #{name} #{output}", :verbose => $verbose
  else
    sh "shotgun/rubinius #{inc} compile #{flags} #{name} #{output}", :verbose => $verbose
  end
end

def compile_dir(dir)
  (Dir["#{dir}/*.rb"] + Dir["#{dir}/**/*.rb"]).each do |file|
    compile file, "#{file}c", true
  end
end

class CodeGroup

  def initialize(files, compile_dir, rba_name, load_order=true)
    if files.is_a?(FileList)
      @files = files
    else
      @files = FileList[files]
    end

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

      deps = [source].compact

      file runtime => deps do |t|
        compile t.prerequisites.first, t.name
      end
    end
  end

  def load_order_task
    return unless @load_order

    file @load_order => @files do
      create_load_order(@files, @load_order)
    end
    task "build:load_order" => @files do
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
