$: << "#{Dir.pwd}/lib"

class ExtensionCompiler

  def initialize(flags = [], extra = [])
    @files = []
    @includes = []
    @link_flags = []
    @compile_flags = []

    @output_name = nil
    @preserve_objects = true if $DEBUG

    # @todo   Figure some sensible way to do this. --rue
    #flags << "-C,-std=c99"
    flags << "-h" unless extra.empty?

    flags.each do |flag|
      case flag
      when /^-I/
        @includes << flag

      when /^-C,/
        @compile_flags << flag[3..-1]

      when /^-Wl,/
        @link_flags << flag[4..-1]

      when /output=(.*?)/
        @output_name = $1

      when /^-p$/
        @preserve_objects = true

      when /^-d$/
        $VERBOSE = 2

      when /^-h$/
        puts  "Options:\n\n" \
              "         -I<includedir>      # E.g. -I/usr/local/include\n" \
              "         -C,<compiler flag>  # E.g. -C,-ggdb3\n" \
              "         -Wl,<linker flag>   # E.g. -Wl,-zelda\n" \
              "         -p                  # Preserve objects\n" \
              "         -d                  # Verbose output\n" \
              "         output=<filename>   # E.g. output=moo.so"

        exit 1

      else
        flags << "-h"

      end
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
      @link_flags << flags
    else
      @compile_flags << flags
    end
  end

  def add_include(inc)
    @includes << "-I#{inc}"
  end

  attr_reader :output

  def calculate_output
    m = /(.*)\.[^\.]+/.match(@output_name)
    if m
      @output = "#{m[1]}#{Rubinius::LIBSUFFIX}"
    else
      @output = "#{@output_name}#{Rubinius::LIBSUFFIX}"
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

    if Rubinius::PLATFORM == :amd64 || ( Rubinius::PLATFORM == :x86 && gcc? )
      str += ' -fPIC '
    end

    str
  end

  def compiler
    if ENV['CC']
      return ENV['CC']
    end

    # Meh
    `which gcc`.chomp
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
      out = file.sub /\.c$/, '.o'

      cmd = "#{compiler} #{compile_options} -c -o #{out} #{file}"
      puts cmd if $VERBOSE
      system cmd

      unless $?.success?
        puts "\n\n ** Compiling failed. Aborting.\n\n"
        exit 1
      end

      @objects << out
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
      args.each { |a| @ec.add_flag "-l#{a}", true }
    end

    def includes(*args)
      args.each { |a| @ec.add_include a }
    end

    def setup
      $ec_dsl = self
    end
  end
end

module Kernel
  def extension
    yield $ec_dsl if block_given?
    $ec_dsl
  end
end

rbx_flags = []
ext_flags = []
flags = []

while ARGV[0] and ARGV[0].prefix? "-f"
  body = ARGV.shift[2..-1]
  if body.empty?
    flags << '-f'
    next
  end

  if body.prefix? 'rbx'
    rbx_flags << body
  else
    ext_flags << body
  end
end

while ARGV[0] and ARGV[0].prefix? "-"
  flags << ARGV.shift
end

file = ARGV.shift

unless file
  puts "Usage: compile [options] FILE|DIR"

  exit 1
end

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
  puts "Compiling extension #{file}..." if $VERBOSE

  ext = ExtensionCompiler.new(flags, ARGV)
  ext.add_file file
  ext.compile

else
  if File.exists?(file)

    out = ARGV.shift || "#{file}c"

    unless flags.include? '-f'
      if File.exists?(out) and File.mtime(out) > File.mtime(file)
        puts "Output '#{out}' is newer, no compile needed."
        exit 0
      end
    end

    if flags.include? "-e"
      puts "Compiling (external) #{file}..."
      require 'compiler/compiler'
      cm = Compiler.compile_file file, rbx_flags
      puts "Unable to compile '#{file}'" unless cm
      Rubinius::CompiledFile.dump cm, out
    else
      puts "Compiling #{file}..."
      compile(file, out, rbx_flags)
    end
  else
    puts "Unable to compile '#{file}'"
  end
end
