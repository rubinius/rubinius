require 'rubygems' 
require 'ruby2ruby'
require 'fileutils'
require 'test/unit'

# $:.unshift(File.dirname(__FILE__) + '/../lib')
# require 'machine'


# This must be fixed to protect against changing directories
$rubinius_path = File.expand_path(File.dirname(__FILE__) + '/../') 

class RubiniusTargetError < RuntimeError
  attr_reader :rubinius_exception, :rubinius_backtrace
  # takes the name of the Rubinius exception and it's backtrace
  def initialize(exc, bt)
    @rubinius_exception = exc
    @rubinius_backtrace = bt
    super(self.message)
  end

  def message
    "#{rubinius_exception} has occured in Rubinius:\n#{rubinius_backtrace}\n"
  end

  alias :inspect :message
end

class Object
  # if self is a RubiniusTargetError then we know more info
  def should_raise(exc)
    if RubiniusTargetError === self
      actual = self.rubinius_exception
      expected = exc.to_s
      unless actual == expected  # passing condition
        msg =  "should raise #{expected} but raised (#{actual}) instead:\n#{self.rubinius_backtrace}"
        Spec::Expectations.fail_with(msg)
      end
    else
      super(exc)
    end
  end
end

module RubiniusTarget
  def initialize(*args)
    super
    @use_rcompile = true if ENV["COMPILER"] == 'rcompile'
  end
  
  def example(src='', &block)
    raise ArgumentError, "you must pass a block" unless block_given?
    execute(source(src, &block))
  rescue RubiniusTargetError => e
    e
  end

  #  Return the source of the given block by using ruby2ruby
  def code(&block)
    src = Module.new { define_method(:__block__, block) }
    src = RubyToRuby.translate(src, :__block__)
    src.gsub(/^(def __block__|end)\n?/, "")
  end
  
  def source(src, &block)
    make_cache_directory
    src = "#{@src}\n" # add setup @src
    src << code(&src) if src.respond_to? :to_proc
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
    
    output = ''
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_path}"

      exec "./shotgun/rubinius #{file}"
    }
    
    r2.close
    w.close
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      if status == 1
        out << "#{file} returned blank output, whats going on?\n" if out.empty?
        out << "========================================="
        raise RubiniusTargetError.new(out, '')
      elsif !status or status > 100
        raise RubiniusTargetError.new("Shotgun has crashed: ", '')
      end
    end
    r.close

    result, backtrace = nil, nil
    begin
      result, backtrace = eval(out)
    rescue Exception => e
      puts "!#{e.class}! The following output from shotgun caused eval() to fail:"
      puts out
      abort
    end

    if backtrace
      raise RubiniusTargetError.new(result, backtrace)
    end
    result
  end
  
  def template
    @template ||= <<-CODE
    def try(a, b=true)
      yield
      return nil
    rescue a
      return b
    end

    code = Proc.new do
      %s
      %s
    end

    rval = begin
      [code.call, nil] # nil means no backtrace
    rescue Exception => e
      [e.class.to_s, e.backtrace.show]
    end

    p rval
    CODE
  end
  
  def cache_source_name(source)
    "#{cache_path}/#{caller_name}-#{source.hash.abs}.rb"
  end
  
  def caller_name(which=3)
    caller[which].match(/\/+(\w*).rb:(\d*)$/)[1..-1].join('-')
  end
  
  def rubinius_path
# This vas causing problems
#    File.expand_path(File.dirname(__FILE__) + '/..')
    $rubinius_path
  end

  def cache_path
    rubinius_path + '/code-cache'
  end
  
  def make_cache_directory
    FileUtils.mkdir_p(cache_path) unless File.exists? cache_path
  end
end
