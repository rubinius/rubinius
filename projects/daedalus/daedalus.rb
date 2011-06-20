# -*- coding: utf-8 -*-
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
require 'thread'

module Daedalus

  class Logger
    def initialize(level=3)
      @count = 0
      @total = nil
      @level = level

      @thread_count = 0
      @count_mutex = Mutex.new
    end

    def thread_id
      id = Thread.current[:build_id]
      unless id
        @count_mutex.synchronize do
          @thread_count += 1
          id = Thread.current[:build_id] = @thread_count
        end
      end

      return id
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
        STDOUT.puts "#{thread_id}: #{kind} #{cmd}"
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

      @archiver = "ar"

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
      @log.command "#{@path} -o #{path} #{files.join(' ')} #{@libraries.join(' ')} #{@ldflags.join(' ')}"
    end

    def shared(path, files)
      @log.show "LIB", path
      @log.command "#{@path} -shared -W1,soname,#{File.basename(path)} -install_name #{Rubinius::BUILD_CONFIG[:lib_path]}/#{File.basename(path)} #{files.join(' ')} -o #{path} #{@libraries.join(' ')} #{@ldflags.join(' ')}"
    end

    def static(path, files)
      @log.show "AR", path
      @log.command "#{@archiver} rcs #{path} #{files.reverse.join(' ')}"
    end

    def calculate_deps(path)
      dirs = header_directories() + ["/usr/include"]
      flags = @cflags.join(' ')
      begin
        dep = DependencyGrapher.new [path], dirs, flags
        dep.process

        # This is a quick work-around for a craptastic bug that I can't figure
        # out. Sometimes this raises an exception saying it can't find a file
        # which is pretty obviously there. I've been unable to figure out
        # what causes this and thus how to fix.
        #
        # So as a temporary measure, if an exception is raised, I'm going to
        # just do it again. Previous results have shown that this should
        # work the 2nd time even though the first time failed.
        #
        # I know this sounds silly, but we need some fix for this.
      rescue Exception
        dep = DependencyGrapher.new [path], dirs, flags
        dep.process
      end

      dep.sources.first.dependencies.sort
    end
  end

  class Path
    def initialize(path)
      @path = path

      if File.exists?(data_path)
        @data = Marshal.load(File.read(data_path)) rescue {}
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
        deps = recalc_depedencies(ctx)
      end

      return deps + @static_deps
    end

    def recalc_depedencies(ctx)
      deps = ctx.calculate_deps(@path)

      @data[:dep_sha1] = ctx.sha1(@path)
      @data[:deps] = deps

      return deps
    end

    def sha1(ctx)
      sha1 = Digest::SHA1.new
      sha1 << ctx.sha1(@path)

      begin
        dependencies(ctx).each do |d|
          sha1 << ctx.sha1(d)
        end
      rescue StandardError
        recalc_depedencies(ctx)
      
        sha1 = Digest::SHA1.new
        sha1 << ctx.sha1(@path)

        dependencies(ctx).each do |d|
          begin
            sha1 << ctx.sha1(d)
          rescue StandardError => e
            raise "Unable to find dependency '#{d}' from #{@path}"
          end
        end
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

    def consider(ctx, tasks)
      tasks << self if out_of_date?(ctx)
    end

    def build(ctx)
      ctx.log.inc!

      if @autogen_builder
        ctx.log.show "GN", @path
        @autogen_builder.call(ctx.log)
      end

      @data[:sha1] = sha1(ctx)
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

    def consider(ctx, tasks)
      tasks.pre << self if out_of_date?(ctx)
    end

    def build(ctx)
      raise "Unable to build" unless @builder

      ctx.log.inc!

      ctx.log.show "LB", @build_dir

      Dir.chdir(@build_dir) do
        @builder.call(ctx.log)
      end

      @data[:sha1] = sha1()

      File.open(@data_file, "w") do |f|
        f << Marshal.dump(@data)
      end
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
      # This partitions the list into .o's first and .a's last. This
      # is because gcc on some platforms require that static libraries
      # be linked last. This is because gcc builds a list of undefined
      # symbols, and then when it hits a .a, looks in the archive
      # to try and resolve those symbols. So if a .o needs something
      # from a .a, the .a MUST come after the .o
      objects = []
      archives = []

      @files.each do |x|
        if x.respond_to? :object_path
          if File.extname(x.object_path) == ".a"
            archives << x.object_path
          else
            objects << x.object_path
          end
        else
          x.objects.each do |obj|
            if File.extname(obj) == ".a"
              archives << obj
            else
              objects << obj
            end
          end
        end
      end

      objects.sort + archives
    end

    def consider(ctx, tasks)
      @files.each { |x| x.consider(ctx, tasks) }
      tasks.post << self unless tasks.empty? and File.exists?(@path)
    end

    def build(ctx)
      ctx.log.inc!
      ctx.link @path, objects
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

  class SharedLibrary < Program
    def describe(ctx)
      puts "Shared Library: #{@path}"

      @files.each do |f|
        f.describe(ctx)
      end
    end

    def build(ctx)
      ctx.log.inc!
      ctx.shared @path, objects
    end
  end

  class StaticLibrary < Program
    def describe(ctx)
      puts "Static Library: #{@path}"

      @files.each do |f|
        f.describe(ctx)
      end
    end

    def build(ctx)
      ctx.log.inc!
      ctx.static @path, objects
    end
  end

  class Tasks
    def initialize
      @pre = []
      @default = []
      @post = []
    end

    attr_reader :pre, :default, :post

    def <<(obj)
      @default << obj
    end

    def empty?
      @pre.empty? and @default.empty? and @post.empty?
    end
  end

  class TaskRunner
    def initialize(compiler, tasks, max=nil)
      @max = TaskRunner.detect_cpus
      @tasks = tasks
      @compiler = compiler

      calculate_max(max)
    end

    def calculate_max(max)
      cpus = TaskRunner.detect_cpus

      case max
      when nil # auto
        case cpus
        when 1, 2
          @max = cpus
        when 4
          @max = 3
        else
          @max = 4
        end
      when Fixnum
        @max = max
      when "cpu"
        @max = cpus
      end
    end

    def self.detect_cpus
      if RUBY_PLATFORM =~ /windows/
        return 1
      else
        if RUBY_PLATFORM =~ /bsd/
          key = 'NPROCESSORS_CONF'
        else
          key = '_NPROCESSORS_CONF'
        end
        count = `getconf #{key} 2>&1`.to_i
        return 1 if $?.exitstatus != 0
        return count
      end
    end

    def start
      linear_tasks @tasks.pre
      perform_tasks @tasks.default
      linear_tasks @tasks.post
    end

    def linear_tasks(tasks)
      tasks.each do |task|
        task.build @compiler
      end
    end

    def perform_tasks(tasks)
      count = tasks.size

      puts "Running #{count} tasks using #{@max} parallel threads"
      start = Time.now

      queue = Queue.new
      threads = []

      @max.times do
        threads << Thread.new {
          while true
            task = queue.pop
            break unless task
            task.build @compiler
          end
        }
      end

      sync = []

      queue_tasks(queue, tasks, sync)

      # Kill off the builders
      threads.each do |t|
        queue << nil
      end

      threads.each do |t|
        t.join
      end

      sync.each do |task|
        task.build @compiler
      end

      puts "Build time: #{Time.now - start} seconds"
    end

    def queue_tasks(queue, tasks, sync)
      tasks.each do |task|
        if task.kind_of? Array
          queue_tasks queue, task[1..-1], sync
          sync << task[0]
        else
          queue.push task
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

    def shared_lib(name, *files)
      lib = SharedLibrary.new(name, files)
      @programs << lib
      lib
    end

    def static_lib(name, *files)
      lib = StaticLibrary.new(name, files)
      @programs << lib
      lib
    end

    def program(name, *files)
      @programs << Program.new(name, files)
    end

    def build(targets=[], jobs=nil)
      if !targets.empty?
        @programs.each do |x|
          if targets.include? x.path
            tasks = Tasks.new
            x.consider @compiler, tasks

            if tasks.empty?
              @compiler.log.info "Nothing to do for #{x.path}"
            else
              tr = TaskRunner.new @compiler, tasks, jobs
              tr.start
            end
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
