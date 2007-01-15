require 'rubygems' 
require 'ruby2ruby'
require 'fileutils'
require 'test/unit'

# $:.unshift(File.dirname(__FILE__) + '/../lib')
# require 'machine'

class RubiniusTargetError < RuntimeError; end

module RubiniusTarget
  def initialize(*args)
    super
    @use_rcompile = true if ENV["COMPILER"] == 'rcompile'
  end
  
  def example(src='', &block)
    raise ArgumentError, "you must pass a block" unless block_given?
    execute(source(src, &block))
  end

  #  Return the source of the given block by using ruby2ruby
  def code(&block)
    src = Module.new { define_method(:__block__, block) }
    src = RubyToRuby.translate(src, :__block__)
    src.gsub(/^(def __block__|end)\n?/, "")
  end
  
  def source(src, &block)
    make_cache_directory
    src = code(&src) if src.respond_to? :to_proc
    source = template % [src, code(&block)]
    name = cache_source_name(source)
    unless File.exists?(name) and source == File.read(name)
      File.open(name, "w") { |f| f << source }
    end

    if @use_rcompile
      `#{rubinius_path}/bin/obsolete.rcompile #{name}`
      unless $?.success?
        FileUtils.rm(name)
        raise RubiniusTargetError, "Unable to compile #{name}"
      end
      name << 'c'
    end

    name
  end
  
  def execute(file)
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_path}"
      
      exec "./shotgun/rubinius ./lib/kernel.rbc #{file}"
    }
    
    r2.close
    w.close
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      if status == 1
        raise RubiniusTargetError, out << "    ========================================="
      elsif !status or status > 100
        raise RubiniusTargetError, "Shotgun has crashed"
      end
    end
    r.close
    eval out.chomp!
  end
  
  def template
    @template ||= <<-CODE
Kernel::p lambda { %s; %s }.call
CODE
  end
  
  def cache_source_name(source)
    "#{cache_path}/#{caller_name}-#{source.hash.abs}.rb"
  end
  
  def caller_name(which=3)
    caller[which].match(/\/+(\w*).rb:(\d*)$/)[1..-1].join('-')
  end
  
  def rubinius_path
    File.expand_path(File.dirname(__FILE__) + '/..')
  end

  def cache_path
    rubinius_path + '/code-cache'
  end
  
  def make_cache_directory
    FileUtils.mkdir_p(cache_path) unless File.exists? cache_path
  end
end
