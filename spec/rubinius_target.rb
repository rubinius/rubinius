require 'rubygems' 
require 'ruby2ruby'
require 'fileutils'
require 'test/unit'
require 'sydparse'

$:.unshift(File.dirname(__FILE__) + '/../lib')
require 'machine'

class RubiniusTargetError < RuntimeError; end

class RubiniusTarget
  attr_reader :block, :source

  def setup
    make_cache_directory
  end

  def example(&block)
    raise ArgumentError, "you must pass a block" unless block_given?
    @block = block
    to_source
    execute(compile(save_source))
  end
  
  def compile(source)
    name = cache_compiled_name
    Machine.new.compile_file(source, false)
    name
  end
  
  def execute(compiled_file)
    r, w = IO.pipe
    r2, w2 = IO.pipe
    
    pid = fork {
      r.close
      w2.close
      STDOUT.reopen(w)
      STDIN.reopen(r2)
      Dir.chdir "#{rubinius_path}"
      
      exec "./shotgun/rubinius ./lib/kernel.rbc #{compiled_file}"
    }
    
    r2.close
    w.close
    w2.close
    Process.wait(pid)
    out = r.read
    status = $?.exitstatus
    if !status or status != 0
      if status == 1
        raise RubiniusError, out << "    ========================================="
      elsif !status or status > 100
        raise RubiniusError, "Shotgun has crashed"
      end
    end
    r.close
    out.chomp!
  end
  
  def template
    @template ||= <<-CODE
%s
RubiniusTarget::SpecExample.new.__example__
CODE
  end
  
  def to_source
    SpecExample.send(:define_method, :__example__, block)
    @source = template % RubyToRuby.translate(SpecExample)
  end
  
  def save_source
    name = cache_source_name
    File.open(name, "w") do |f|
      f << source
    end
    name
  end
  
  def cache_source_name
    "#{cache_path}/#{caller_name}-#{source.hash.abs}.rb"
  end
  
  def cache_compiled_name
    cache_source_name + 'c'
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

  private
  class SpecExample; end
end

# The big idea:
# 1) Using ParseTree, convert the block passed to example to an sexp
#   a) Create a new class
#   b) Define a method on the class that is the block as a Proc
#   c) Parse the class into an sexp
#   d) Extract from the sexp only the sexp for the block body
# 2) Pass the sexp for the block body to the ByteCode::Compiler
# 3) Pass the compiled script to the Machine to execute
# 4) Marshal the return value to a file (put this in the block?)
# 5) In the Host, read in the marshalled object
# 6) In the Host, complete execution of spec expectation (e.g. .should == <blah>)

