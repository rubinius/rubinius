=begin

Daedalus |ˈdedl-əs| Greek Mythology
  a craftsman, considered the inventor of carpentry, who is said to have
  built the labyrinth for Minos, king of Crete. Minos imprisoned him and
  his son Icarus, but they escaped using wings that Daedalus made and
  fastened with wax. Icarus, however, flew too near the sun and was killed.


In other words, he built things for a living.

=end

require 'digest/sha1'
require 'projects/daedalus/dependency_grapher'
require 'rakelib/instruction_parser'

module Daedalus

  class Logger
    def initialize(level=3)
      @count = nil
      @total = nil
      @level = level
    end

    def start(count)
      @count = 0
      @total = count
    end

    def stop
      @total = nil
    end

    def inc!
      @count += 1
    end

    def show(kind, cmd)
      if @total
        STDOUT.puts "[%3d/%3d] #{kind} #{cmd}" % [@count, @total]
      else
        STDOUT.puts "#{kind} #{cmd}"
      end
    end

    def command(cmd)
      system cmd
      if $?.exitstatus != 0
        STDOUT.puts "Error: #{cmd}"
        raise "Error compiling"
      end
    end

    def verbose(str)
      if @level >= 5
        STDOUT.puts "daedalus: #{str}"
      end
    end

    def info(str)
      if @level >= 3
        STDOUT.puts "daedalus: #{str}"
      end
    end
  end

  class FancyLogger < Logger
    HEADER =   "Daedalus building:"

    def initialize(level=0)
      super 0
    end

    def start(count)
      super
      STDOUT.sync = true
      STDOUT.puts HEADER
    end

    def show(kind, cmd)
      @count += 1
      perc = (100 * (@count.to_f / @total)).to_i
      bar_size = (30 * (@count.to_f / @total)).to_i

      bar = "#{'=' * bar_size}#{' ' * (30 - bar_size)}"

      if cmd.size > 38
        cmd = "..#{cmd[-38,38]}"
      else
        cmd = cmd.ljust(40)
      end

      STDOUT.print "\r[%3d%% #{bar}] #{kind} #{cmd}" % perc.to_i
    end

    def command(cmd)
      output = IO.popen "sh -c '#{cmd} 2>&1'", "r"

      begin
        str = output.read
      rescue Exception
        Process.kill 'SIGINT', output.pid
        STDOUT.puts "\nInterrupt compiling."
        raise "Stopped compiling"
      end

      Process.wait output.pid

      if $?.exitstatus != 0
        STDOUT.puts "Error compiling: #{cmd}"
        STDOUT.puts "Output:\n#{str}"
        raise "Error compiling"
      end
    end

    def stop
      super
      STDOUT.puts
    end
  end

  class Compiler
    def initialize(path, logger, blueprint)
      @path = path
      @cflags = []
      @ldflags = []
      @libraries = []
      @log = logger
      @blueprint = blueprint

      @mod_times = Hash.new do |h,k|
        h[k] = (File.exists?(k) ? File.mtime(k) : Time.at(0))
      end

      @sha1_mtimes = {}
      @sha1s = Hash.new do |h,k|
        if File.exists?(k)
          @log.verbose "computing SHA1: #{k}"
          @sha1_mtimes[k] = File.mtime(k)
          h[k] = Digest::SHA1.file(k).hexdigest
        else
          h[k] = ""
        end
      end
    end

    def header_directories
      dirs = []
      @cflags.each do |fl|
        fl.split(/\s+/).each do |part|
          if part.index("-I") == 0
            dirs << part[2..-1]
          end
        end
      end
      dirs
    end

    attr_reader :path, :cflags, :ldflags, :log

    def add_library(lib)
      if f = lib.cflags
        @cflags += f
      end

      if f = lib.ldflags
        @ldflags += f
      end
    end

    def mtime(path)
      @mod_times[path]
    end

    def sha1(path)
      if @sha1s.key?(path)
        if File.mtime(path) > @sha1_mtimes[path]
          @sha1s.delete(path)
        end
      end

      @sha1s[path]
    end

    def compile(source, object)
      @log.show "CC" , source
      @log.command "#{@path} #{@cflags.join(' ')} -c -o #{object} #{source}"
    end

    def link(path, files)
      @log.show "LD", path
      @log.command "#{@path} #{@ldflags.join(' ')} -o #{path} #{files.join(' ')} #{@libraries.join(' ')}"
    end

    def calculate_deps(path)
      dirs = header_directories() + ["/usr/include"]
      flags = @cflags.join(' ')
      dep = DependencyGrapher.new [path], dirs, flags
      dep.process
      dep.sources.first.dependencies.sort
    end
  end

  class Path
    def initialize(path)
      @path = path

      if File.exists?(data_path)
        @data = Marshal.load(File.read(data_path))
      else
        @data = {}
      end
    end

    attr_reader :data, :path

    def basename
      File.basename @path
    end

    def artifacts_path
      dir = File.join File.dirname(@path), "artifacts"
      Dir.mkdir dir unless File.directory?(dir)
      return dir
    end

    def data_path
      File.join artifacts_path, "#{basename}.data"
    end

    def save!
      File.open(data_path, "w") do |f|
        f << Marshal.dump(data)
      end
    end
  end

  class SourceFile < Path

    def initialize(path)
      super
      @static_deps = []
      @autogen_builder = nil
    end

    def depends_on(static_deps)
      @static_deps = static_deps
    end

    def autogenerate(&builder)
      @autogen_builder = builder
    end

    def object_path
      File.join artifacts_path, "#{basename}.o"
    end

    def dependencies(ctx)
      deps = @data[:deps]

      if ctx.sha1(@path) != @data[:dep_sha1] or !deps
        deps = ctx.calculate_deps(@path)

        @data[:dep_sha1] = ctx.sha1(@path)
        @data[:deps] = deps
      end

      return deps + @static_deps
    end

    def sha1(ctx)
      sha1 = Digest::SHA1.new
      sha1 << ctx.sha1(@path)

      dependencies(ctx).each do |d|
        sha1 << ctx.sha1(d)
      end

      sha1.hexdigest
    end

    def newer_dependencies(ctx)
      dependencies(ctx).find_all do |x|
        ctx.mtime(x) > ctx.mtime(object_path)
      end
    end

    def out_of_date?(ctx)
      unless File.exists?(@path)
        return true if @autogen_builder
        raise Errno::ENOENT, "Missing #{@path}"
      end

      return true unless File.exists?(object_path)
      return true unless @data[:sha1] == sha1(ctx)
      return false
    end

    def build(ctx)
      ctx.log.inc!

      if File.exists?(@path)
        hash = sha1(ctx)

        if File.exists?(object_path) and @data[:sha1] == hash
          ctx.log.show "RE", object_path
          save!
        end
      end

      if @autogen_builder
        ctx.log.show "GN", @path
        @autogen_builder.call(ctx.log)
        # building assuming regenerates the hash
        hash = sha1(ctx)
      end

      @data[:sha1] = hash
      ctx.compile path, object_path
      save!
    end

    def clean
      File.unlink object_path if File.exists?(object_path)
      File.unlink data_path if File.exists?(data_path)

      Dir.rmdir artifacts_path if Dir.entries(artifacts_path).empty?
    end

    def describe(ctx)
      if !File.exists?(object_path)
        puts "#{@path}: unbuilt"
      else
        if @data[:sha1] != sha1(ctx)
          puts "#{@path}: out-of-date"
        end

        deps = newer_dependencies(ctx)

        unless deps.empty?
          puts "#{@path}: dependencies changed"
          deps.each do |d|
            puts "  - #{d}"
          end
        end
      end
    end

    def info(ctx)
      puts @path
      puts "  object: #{object_path}"
      puts "  last hash: #{@data[:sha1]}"
      puts "  curr hash: #{sha1(ctx)}"

      puts "  dependencies:"
      dependencies(ctx).each do |x|
        puts "    #{x}"
      end
    end
  end

  class ExternalLibrary
    def initialize(path)
      @path = path

      @cflags = nil
      @ldflags = nil
      @objects = nil

      @build_dir = path
      @builder = nil
      @data = nil
    end

    attr_accessor :path, :cflags, :ldflags, :objects

    def to_build(&blk)
      @builder = blk

      @data_file = "#{@build_dir}.data"

      if File.exists?(@data_file)
        @data = Marshal.load(File.read(@data_file)) rescue {}
      else
        @data = {}
      end

    end

    def file(f)
      File.join(@path, f)
    end

    def sha1
      sha1 = Digest::SHA1.new

      Dir["#{@build_dir}/*"].each do |f|
        sha1.file(f) if File.file?(f)
      end

      Dir["#{@build_dir}/**/*"].each do |f|
        sha1.file(f) if File.file?(f)
      end

      sha1.hexdigest
    end

    def have_objects
      return true unless @objects
      @objects.each do |o|
        return false unless File.exists?(o)
      end

      return true
    end

    def out_of_date?(ctx)
      return true  unless have_objects
      return false unless @builder
      return false if @data and @data[:sha1] == sha1
      return true
    end

    def build(ctx)
      return "" unless @builder

      hash = sha1()

      ctx.log.inc!

      if have_objects and @data[:sha1] == hash
        ctx.log.show "RE", @build_dir
      else
        ctx.log.show "LB", @build_dir
        Dir.chdir(@build_dir) do
          @builder.call(ctx.log)
        end

        # building can change the sha1, so recompute it.
        hash = sha1()
        @data[:sha1] = hash

        File.open(@data_file, "w") do |f|
          f << Marshal.dump(@data)
        end
      end

      hash
    end

    def describe(ctx)
    end
  end

  class Program < Path
    def initialize(path, files)
      super path
      @files = files.sort_by { |x| x.path }
    end

    def objects
      objects = []

      @files.each do |x|
        if x.respond_to? :object_path
          objects << x.object_path
        else
          objects.concat x.objects
        end
      end

      objects.sort
    end

    def build(ctx)
      to_build = @files.find_all { |x| x.out_of_date?(ctx) }

      ctx.log.start to_build.size + 1

      to_build.each { |x| x.build(ctx) }

      sha1 = Digest::SHA1.new

      objs = objects()
      objs.each { |x| sha1.file(x) }

      hex = sha1.hexdigest

      ctx.log.inc!

      if File.exists?(@path) and @data[:sha1] == hex
        ctx.log.show "RE", @path
        save!
      else
        @data[:sha1] = hex

        ctx.link @path, objs
        save!
      end

      ctx.log.stop
    end

    def clean
      @files.each do |f|
        f.clean if f.respond_to? :clean
      end

      File.unlink @path if File.exists?(@path)
      File.unlink data_path if File.exists?(data_path)
      Dir.rmdir artifacts_path if Dir.entries(artifacts_path).empty?
    end

    def describe(ctx)
      puts "Program: #{@path}"

      @files.each do |f|
        f.describe(ctx)
      end
    end

    def file_info(ctx, files)
      files.each do |n|
        obj = @files.find { |x| x.path == n }
        if obj
          obj.info(ctx)
        else
          puts "Unable to find file: #{n}"
        end
      end
    end
  end

  class Blueprint
    def initialize
      @programs = []
      @compiler = nil
    end

    def external_lib(path)
      ex = ExternalLibrary.new(path)
      yield ex
      ex
    end

    def gcc!
      @compiler = Compiler.new(ENV['CC'] || "gcc", Logger.new, self)
    end

    def source_files(*patterns)
      files = []

      patterns.each do |t|
        Dir[t].each do |f|
          files << SourceFile.new(f)
        end
      end

      files
    end

    def source_file(file)
      sf = SourceFile.new(file)
      yield sf if block_given?
      sf
    end

    def program(name, *files)
      @programs << Program.new(name, files)
    end

    def build(targets=[])
      if !targets.empty?
        @programs.each do |x|
          if targets.include? x.path
            x.build @compiler
          end
        end
      else
        @programs.each { |x| x.build @compiler }
      end
    end

    def clean(targets=[])
      if !targets.empty?
        @programs.each do |x|
          if targets.include? x.path
            x.clean
          end
        end
      else
        @programs.each { |x| x.clean }
      end
    end

    def describe(targets=[])
      if !targets.empty?
        @programs.each do |x|
          if targets.include? x.path
            x.describe @compiler
          end
        end
      else
        @programs.each { |x| x.describe @compiler }
      end
    end

    def file_info(files)
      @programs.each do |x|
        x.file_info @compiler, files
      end
    end
  end

  def self.blueprint
    b = Blueprint.new
    yield b
    b
  end

  def self.load(file)
    eval File.read(file)
  end
end
