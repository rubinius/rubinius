
def clear_compiler
  ENV.delete 'RBX_BOOTSTRAP'
  ENV.delete 'RBX_CORE'
  ENV.delete 'RBX_LOADER'
  ENV.delete 'RBX_PLATFORM'
end

def make(args = nil)
  if RUBY_PLATFORM =~ /bsd/
    gmake = 'gmake'
  else
    gmake = 'make'
  end
  "#{ENV['MAKE'] || gmake} #{args}"
end

def rbx(*args)
  clear_compiler

  sh('bin/rbx', *args)
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

  inc = "-Iruntime/stable/compiler.rba -rcompiler/compiler"
  flags = "-frbx-safe-math -frbx-kernel"

  if ENV['SYSTEM']
    sh "rbx compile -f #{flags} #{name} #{output}", :verbose => $verbose
  elsif ENV['GDB']
    sh "bin/rbx --gdb #{inc} compile #{flags} #{name} #{output}", :verbose => $verbose
  elsif ENV['NATIVE']
    sh "bin/rbx #{inc} compile #{flags} #{name} #{output}", :verbose => $verbose
  else
    ruby "lib/compiler/mri_compile.rb #{flags} #{name} #{output}"
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
      @load_order = File.join @compile_dir, 'load_order.txt'
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
    FileUtils.mkdir_p @compile_dir unless File.exists? @compile_dir

    compile_task
    load_order_task

    @output
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
