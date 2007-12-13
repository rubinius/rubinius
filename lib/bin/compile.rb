class ExtensionCompiler
  
  def initialize(extra=[], flags=[])
    @includes = []
    @link_flags = []
    @compile_flags = ["-g"]
    
    extra.each do |e|
      if e.prefix?("-I")
        @includes << e
      elsif e.prefix("-C,")
        @compile_flags << e[3..-1]
      else
        @link_flags << e
      end
    end
    
    @output_name = nil
    @preserve_objects = false
    
    @files = []
    @flags = flags
    flags.each do |i|
      if m = /output=(.*)/.match(i)
        @output_name = m[1]
      end
      
      if i == "-p"
        @preserve_objects = true
      end
    end

    if $DEBUG
      @preserve_objects = true
    end 
    
    if File.exists?("shotgun/lib/subtend/ruby.h")
      @includes << "-I#{Dir.pwd}/shotgun/lib/subtend"
    end
    
  end
  
  def add_file(file)
    @files << file
    @output_name = file unless @output_name
  end
  
  def set_output(output)
    @output_name = output
  end
  
  def add_flag(flags, link=true)
    if link
      @link_flags << flag
    else
      @compile_flags << flag
    end
  end
  
  def add_include(inc)
    @includes << "-I#{inc}"
  end
  
  attr_reader :output
  
  def calculate_output
    m = /(.*)\.[^\.]+/.match(@output_name)
    if m
      @output = "#{m[1]}.#{Rubinius::LIBSUFFIX}"
    else
      @output = "#{@output_name}.#{Rubinius::LIBSUFFIX}"
    end
  end
    
  def windows?
    Rubinius::OS == :win32
  end
  
  def gcc?
    Rubinius::COMPILER == :gcc
  end
  
  def vcc?
    Rubinius::OS == :win32 and Rubinius::COMPILER == :microsoft
  end
  
  def mingw?
    Rubinius::OS == :win32 and Rubinius::COMPILER == :gcc  
  end
  
  def cygwin?
    Rubinius::OS == :cygwin
  end
  
  def darwin?
    Rubinius::OS == :darwin
  end
  
  def compile_options    
    str = (@includes + @compile_flags).join(" ")

    if Rubinius::PLATFORM == :amd64
      str += ' -fPIC '
    end

    str
  end
    
  def compiler
    if ENV['CC']
      return ENV['CC']
    end
    
    Rubinius::COMPILER_PATH
  end
  
  # Adapted from RubyInline
  def system_link_options
    if vcc?
      "-link /LIBPATH:\"#{@libdir}\" /DEFAULTLIB:\"#{@lib}\" /INCREMENTAL:no /EXPORT:#{@init}"
    elsif mingw?
      "-Wl,--enable-auto-import -L#{@libdir}"
    elsif cygwin?
      "-L/usr/local/lib"
    elsif darwin?
      "-dynamic -bundle -undefined suppress -flat_namespace"
    else
      "-shared"
    end
  end
    
  def link_options
    opts = @link_flags.dup
    opts << system_link_options
    
    return opts.join(" ")
  end
  
  def compile_files
    @objects = []
    @files.each do |file|
      cmd = "#{compiler} #{compile_options} -c -o #{file}.o #{file}"
      puts cmd if $VERBOSE
      system cmd
      @objects << "#{file}.o"
    end
  end
  
  def compile(report=true)
    compile_files
    calculate_output
    cmd = "#{compiler} #{link_options} #{@objects.join(' ')} -o #{@output}"
    puts cmd if $VERBOSE
    system cmd
    
    return unless report
    
    if File.exists?(@output)
      unless @preserve_objects
        puts "Cleaning up objects..."
        @objects.each { |o| system "rm #{o}" }
      end
      
      puts "Created #{@output}" if report
    else
      if report
        puts "Unable to compile extension into #{@output}. Check compiler log."
      end
      exit 1
    end
    
  end
  
  class DSL
    def initialize(ec)
      @ec = ec
    end
    
    def name(name)
      @ec.set_output name
    end
    
    def files(glob)
      Dir[glob].each { |f| @ec.add_file f }
    end
    
    def flags(*args)
      args.each { |a| @ec.add_flag a }
    end
    
    def libs(*args)
      args.each { |a| @ec.add_flags "-l#{a}", true }
    end
    
    def includes(*args)
      args.each { |a| @ec.add_include a }
    end
    
    def setup
      module Kernel
        def extension
          yield $ec_dsl if block_given?
          $ec_dsl
        end
      end
      
      $ec_dsl = self
    end
  end
end

flags = []
while ARGV[0] and ARGV[0].prefix? "-f"
  flags << ARGV.shift[2..-1]
end

file = ARGV.shift

if File.directory?(file)
  rec = File.join(file, "build.rb")
  unless File.exists?(rec)
    puts "No build instructions found in #{file}"
    exit 1
  end
  puts "Building from instructions at #{rec}" if $VERBOSE
  ext = ExtensionCompiler.new(flags, ARGV)
  dsl = ExtensionCompiler::DSL.new(ext)
  dsl.setup
  cur = Dir.pwd
  Dir.chdir file
  load "build.rb"
  ext.compile
elsif file.suffix?(".c")
  puts "Compiling extension #{file}..."
  ext = ExtensionCompiler.new(flags, ARGV)
  ext.add_file file
  ext.compile
else
  if File.exists?(file)
    puts "Compiling #{file}..."
    compile(file, ARGV.shift, flags)
  else
    puts "Unable to compile '#{file}'"
  end
end
